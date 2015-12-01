/*
 * FAAD - Freeware Advanced Audio Decoder
 * Copyright (C) 2001 Menno Bakker
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: in_aac.c,v 1.40 2001/10/07 14:55:59 menno Exp $
 */

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#include <faad.h>
#include <stdio.h>
#include "aacinfo.h"
#include "filestream.h"
#include "id3v2tag.h"

#include "in2.h"

// post this to the main window at end of file (after playback as stopped)
#define WM_WA_AAC_EOF WM_USER+2

faadAACInfo file_info;

faacDecHandle hDecoder;

In_Module mod; // the output module (declared near the bottom of this file)
char *lastfn; // currently playing file (used for getting info on the current file)
int file_length; // file length, in bytes
int decode_pos_ms; // current decoding position, in milliseconds
int paused; // are we paused?
int seek_needed; // if != -1, it is the point that the decode thread should seek to, in ms.

char *sample_buffer; // sample buffer
unsigned char *buffer; // input buffer
int *seek_table;
long buffercount, fileread, bytecount;

int killPlayThread=0; // the kill switch for the decode thread
int PlayThreadAlive=0; // 1=play thread still running
HANDLE play_thread_handle=INVALID_HANDLE_VALUE; // the handle to the decode thread
FILE_STREAM *infile;

/* Function definitions */
int id3v2_tag(unsigned char *buffer);
DWORD WINAPI PlayThread(void *b); // the decode thread procedure

BOOL CALLBACK config_dialog_proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
FILE *dat_file = NULL;
long priority = 5;

// 1= use vbr display, 0 = use average bitrate. This value only controls what shows up in the
// configuration form. Also- Streaming uses an on-the-fly bitrate display regardless of this value.
long variable_bitrate_display=0;

int vbr_display=0; // This is what controls what actually shows up in the main winamp window for bitrates. 
char format_string[50] = {0};

char *priority_text[] = {	"",
							"Decode Thread Priority: Lowest",
							"Decode Thread Priority: Lower",
							"Decode Thread Priority: Normal",
							"Decode Thread Priority: Higher",
							"Decode Thread Priority: Highest (default)"
						};

long priority_table[] = {0, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_BELOW_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST};

void config(HWND hwndParent)
{
    /*MessageBox(hwndParent,
        "AAC files without an ADIF or ADTS header must be 44100kHz\n"
        "and must be encoded using MAIN or LOW profile.\n"
        "When an ADIF or ADTS header is present, configuration\n"
        "will be done automatically.",
        "Configuration",MB_OK);*/

	DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_CONFIG), hwndParent, config_dialog_proc);
}

BOOL CALLBACK config_dialog_proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char tmp[5];
	
	/*

	Data structure:
	All variables are longs.
	
	(1): Buffering amount
	(2): Thread Priority
	(3): Variable bitrate display

	*/

    switch (message)
	{
    case WM_INITDIALOG:
        
		SetDlgItemText(hwndDlg, IDC_FORMAT, format_string);

		/* Set priority slider range and previous position */
		SendMessage(GetDlgItem(hwndDlg, THREAD_PRIORITY_SLIDER), TBM_SETRANGE, TRUE, MAKELONG(1, 5)); 
		SendMessage(GetDlgItem(hwndDlg, THREAD_PRIORITY_SLIDER), TBM_SETPOS, TRUE, priority);
		SetDlgItemText(hwndDlg, IDC_STATIC2, priority_text[priority]);
		
		/* Put a limit to the amount of characters allowed in the buffer boxes */
		SendMessage(GetDlgItem(hwndDlg, LOCAL_BUFFER_TXT), EM_LIMITTEXT, 4, 0);
		SendMessage(GetDlgItem(hwndDlg, STREAM_BUFFER_TXT), EM_LIMITTEXT, 4, 0);

	    if(variable_bitrate_display)
			SendMessage(GetDlgItem(hwndDlg, VARBITRATE_CHK), BM_SETCHECK, BST_CHECKED, 0);

		itoa(local_buffer_size, tmp, 10);
		SetDlgItemText(hwndDlg, LOCAL_BUFFER_TXT, tmp);

		itoa(stream_buffer_size, tmp, 10);
		SetDlgItemText(hwndDlg, STREAM_BUFFER_TXT, tmp);

		return TRUE;

	case WM_HSCROLL:

		/* Thread priority slider moved */
		if(GetDlgItem(hwndDlg, THREAD_PRIORITY_SLIDER) == (HWND) lParam)
		{
			int tmp;
			tmp = SendMessage(GetDlgItem(hwndDlg, THREAD_PRIORITY_SLIDER), TBM_GETPOS, 0, 0);

			if(tmp > 0)
			{
				priority = tmp;

				fseek(dat_file, 8, SEEK_SET);
				fwrite(&priority, 1, 4, dat_file);
				SetDlgItemText(hwndDlg, IDC_STATIC2, priority_text[priority]);

				if(play_thread_handle)
					SetThreadPriority(play_thread_handle, priority_table[priority]);
			}
		}

		return TRUE;

    case WM_COMMAND:

		if(HIWORD(wParam) == BN_CLICKED)
		{
			if(GetDlgItem(hwndDlg, VARBITRATE_CHK) == (HWND) lParam)
			{
				/* Variable Bitrate checkbox hit */
				variable_bitrate_display = SendMessage(GetDlgItem(hwndDlg, VARBITRATE_CHK), BM_GETCHECK, 0, 0); 
				fseek(dat_file, 12, SEEK_SET);
				fwrite(&variable_bitrate_display, 1, 4, dat_file);
			}
		}

        switch (LOWORD(wParam))
		{
			case OK_BTN:
				/* User hit OK, save buffer settings (all others are set on command) */
				GetDlgItemText(hwndDlg, LOCAL_BUFFER_TXT, tmp, 5);
				local_buffer_size = atol(tmp);

				fseek(dat_file, 0, SEEK_SET);
				fwrite(&local_buffer_size, 1, 4, dat_file);

				GetDlgItemText(hwndDlg, STREAM_BUFFER_TXT, tmp, 5);
				stream_buffer_size = atol(tmp);

				fseek(dat_file, 4, SEEK_SET);
				fwrite(&stream_buffer_size, 1, 4, dat_file);

				GetDlgItemText(hwndDlg, IDC_FORMAT, format_string, 50);
				fseek(dat_file, 16, SEEK_SET);
				fwrite(format_string, 1, 50, dat_file);

			case IDCANCEL:
			case CANCEL_BTN:
				/* User hit Cancel or the X, just close without saving buffer settings */
				EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}

void about(HWND hwndParent)
{
    MessageBox(hwndParent,"Freeware AAC Player\nCompiled on: " __DATE__ "\nhttp://www.audiocoding.com","About AAC Player",MB_OK);
}

void init()
{
	char plugin_path[1024];
	int i;

    buffer = (unsigned char*)LocalAlloc(LPTR, 768*2);

    sample_buffer = (char*)LocalAlloc(LPTR, 1024*2*(16/8)*sizeof(char));

    /* 1 entry for every second, enough for 3 hours  */
    seek_table = (int*)LocalAlloc(LPTR, 10800*sizeof(int));
    lastfn = (char*)LocalAlloc(LPTR, 1024*sizeof(char));

    /* Initialize winsock, necessary for streaming */
    WinsockInit();

	/* Get the path for winamp, then edit so we get the plugin path name */
	GetModuleFileName(NULL, plugin_path, 1024);

	for(i=strlen(plugin_path); i > 0 && plugin_path[i] != '\\'; i--);
	
	memset(plugin_path + i + 1, 0, 1024 - (i + 1));
	strcat(plugin_path, "Plugins\\faad.dat");

	/* Open the file */
	dat_file = fopen(plugin_path, "r+b");

	if(dat_file == NULL)
	{
		/* The file doesn't exsist, so create a new one with default settings */
		dat_file = fopen(plugin_path, "w+b");

		fwrite(&local_buffer_size, 1, 4, dat_file);
		fwrite(&stream_buffer_size, 1, 4, dat_file);
		fwrite(&priority, 1, 4, dat_file);
		fwrite(&variable_bitrate_display, 1, 4, dat_file);
        lstrcpy(format_string, "%1 - %2");
		fwrite(format_string, 1, 50, dat_file);
	}
	else
	{
		fread(&local_buffer_size, 1, 4, dat_file);
		fread(&stream_buffer_size, 1, 4, dat_file);
		fread(&priority, 1, 4, dat_file);
		fread(&variable_bitrate_display, 1, 4, dat_file);
		fread(format_string, 1, 50, dat_file);
	}
}

void quit()
{
    if (lastfn) LocalFree(lastfn);
    if (seek_table) LocalFree(seek_table);
    if (sample_buffer) LocalFree(sample_buffer);
    if (buffer) LocalFree(buffer);

    /* Deallocate winsock */
    WinsockDeInit();

	if(dat_file) fclose(dat_file);
}

int isourfile(char *fn)
{
    /* Finally fixed */
    if(StringComp(fn + strlen(fn) - 3, "aac", 3) == 0)
    {
        return 1;
    }

    return 0;
}

int play(char *fn)
{
    int k;
    int maxlatency, tagsize;
    int thread_id;
    unsigned long samplerate, channels;

    ZeroMemory(buffer, 768*2);

    infile = open_filestream(fn);
    
	if (infile == NULL)
        return 1;

    fileread = filelength_filestream(infile);

    buffercount = bytecount = 0;
    read_buffer_filestream(infile, buffer, 768*2);

    tagsize = id3v2_tag(buffer);

	/* If we find a tag, run right over it */
    if(tagsize)
	{
		if(infile->http)
		{
			int i;
			/* Crude way of doing this, but I believe its fast enough to not make a big difference */
			close_filestream(infile);
			infile = open_filestream(fn);

			for(i=0; i < tagsize; i++)
				read_byte_filestream(infile);
		}
		else
		{
			seek_filestream(infile, tagsize, FILE_BEGIN);
		}

        bytecount = tagsize;
        buffercount = 0;
        read_buffer_filestream(infile, buffer, 768*2);
    }

    hDecoder = faacDecOpen();

	/* Copy the configuration dialog setting and use it as the default */
	/* However, for streaming files vbr display is used regardless */
	if(variable_bitrate_display)
		vbr_display = 1;
	else
		vbr_display = 0;

	/* initialize the decoder, and get samplerate and channel info */
    if(StringComp(fn, "http://", 7) == 0)
    {
        if((buffercount = faacDecInit(hDecoder, buffer, &samplerate, &channels)) < 0)
        {
            MessageBox(mod.hMainWindow, "Error opening input file\n", "FAAD Error", MB_OK);
            return 1;
        }

		// Bitrate for streaming is always displayed as vbr.
		vbr_display = 1;

        if (buffercount > 0)
		{
            bytecount += buffercount;

            for (k = 0; k < (768*2 - buffercount); k++)
                buffer[k] = buffer[k + buffercount];

            read_buffer_filestream(infile, buffer + (768*2) - buffercount, buffercount);
            buffercount = 0;
        }
    }
	else
	{
		// Bitrate display method for local files varies depends on user configuration.
        if((buffercount = faacDecInit(hDecoder, buffer, &samplerate, &channels)) < 0)
        {
            MessageBox(mod.hMainWindow, "Error opening input file\n", "FAAD Error", MB_OK);
            return 1;
        }

        if (buffercount > 0)
		{
            bytecount += buffercount;

            for (k = 0; k < (768*2 - buffercount); k++)
                buffer[k] = buffer[k + buffercount];

            read_buffer_filestream(infile, buffer + (768*2) - buffercount, buffercount);
            buffercount = 0;
        }
    }

	get_AAC_format(fn, &file_info, seek_table);

	if(infile->http)
	{
		/* No seeking in http streams */
		mod.is_seekable = 0;
	}
	else
	{
		if (file_info.headertype == 2) /* ADTS header - seekable */
			mod.is_seekable = 1;
		else
			mod.is_seekable = 0; /* ADIF or Headerless - not seekable */
	}

    strcpy(lastfn,fn);
    paused=0;
    decode_pos_ms=0;
    seek_needed=-1;

    maxlatency = mod.outMod->Open(file_info.sampling_rate, file_info.channels, 16, -1,-1);

    if (maxlatency < 0) // error opening device
    {
        return -1;
    }

    // initialize vis stuff
    mod.SAVSAInit(maxlatency, file_info.sampling_rate);
    mod.VSASetInfo(file_info.sampling_rate, file_info.channels);

    mod.SetInfo(file_info.bitrate/1000, file_info.sampling_rate/1000, file_info.channels,1);

    mod.outMod->SetVolume(-666); // set the output plug-ins default volume

    killPlayThread = 0;

    if((play_thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) PlayThread, (void *) &killPlayThread, 0, &thread_id)) == NULL)
    {
        MessageBox(mod.hMainWindow, "Fatal error: Cannot create playback thread\n", "FAAD Error", MB_OK);
        return -1;
    }

    // Note: This line seriously slows down start up time
	if(priority != 3) // if the priority in config window is set to normal, there is nothing to reset!
	    SetThreadPriority(play_thread_handle, priority_table[priority]);

    return 0;
}

void pause() { paused=1; mod.outMod->Pause(1); }
void unpause() { paused=0; mod.outMod->Pause(0); }
int ispaused() { return paused; }

void stop()
{
    killPlayThread=1;

    if (play_thread_handle != INVALID_HANDLE_VALUE)
    {
        if (WaitForSingleObject(play_thread_handle, INFINITE) == WAIT_TIMEOUT)
            TerminateThread(play_thread_handle,0);
        CloseHandle(play_thread_handle);
        play_thread_handle = INVALID_HANDLE_VALUE;
    }

    faacDecClose(hDecoder);
    close_filestream(infile);
    mod.outMod->Close();
    mod.SAVSADeInit();
}

int getlength()
{
    return file_info.length;
}

int getoutputtime()
{
    return decode_pos_ms+(mod.outMod->GetOutputTime()-mod.outMod->GetWrittenTime());
}

void setoutputtime(int time_in_ms)
{
    if (file_info.headertype == 2)
        seek_needed=time_in_ms;
    else
        seek_needed = -1;
}

int aac_seek(int pos_ms, int *seek_table)
{
    double offset_sec;

    offset_sec = pos_ms / 1000.0;
    seek_filestream(infile, seek_table[(int)(offset_sec+0.5)], FILE_BEGIN);

    bytecount = seek_table[(int)(offset_sec+0.5)];
    buffercount = 0;
    read_buffer_filestream(infile, buffer, 768*2);

    return 0;
}

void setvolume(int volume) { mod.outMod->SetVolume(volume); }
void setpan(int pan) { mod.outMod->SetPan(pan); }

char info_fn[255];

BOOL CALLBACK info_dialog_proc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    faadAACInfo format;
    char *tmp_string;
    char info[1024];
    LV_COLUMN lvc; 

    switch (message) {
    case WM_INITDIALOG:
        /* Set up the list control for the ID3 tag */
        
        /* Initialize the LV_COLUMN structure. */
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;

        /* Add the columns. */
        lvc.iSubItem = 0;
        lvc.cx = 100;
        lvc.pszText = "Frame";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_ID3LIST), 0, &lvc);
        lvc.iSubItem = 1;
        lvc.cx = 250;
        lvc.pszText = "Data";
        ListView_InsertColumn(GetDlgItem(hwndDlg, IDC_ID3LIST), 1, &lvc);

        /* get AAC info */
        get_AAC_format(info_fn, &format, seek_table);

        switch(format.headertype)
        {
        case 0: /* Headerless */
            tmp_string = "RAW"; break;
        case 1: /* ADIF */
            tmp_string = "ADIF"; break;
        case 2: /* ADTS */
            tmp_string = "ADTS"; break;
        }
        SetDlgItemText(hwndDlg, IDC_HEADER, tmp_string);

        if (format.object_type == 0 /* Main */)
            tmp_string = "Main";
        else if (format.object_type == 1 /* Low Complexity */)
            tmp_string = "Low Complexity";
        else if (format.object_type == 2 /* SSR */)
            tmp_string = "SSR (unsupported)";
        else if (format.object_type == 3 /* LTP */)
            tmp_string = "Main LTP";
        SetDlgItemText(hwndDlg, IDC_PROFILE, tmp_string);

        if (format.version == 2) tmp_string = "MPEG2";
        else tmp_string = "MPEG4";
        SetDlgItemText(hwndDlg, IDC_VERSION, tmp_string);

        wsprintf(info, "%d bps", format.bitrate);
        SetDlgItemText(hwndDlg, IDC_BITRATE, info);

        wsprintf(info, "%d Hz", format.sampling_rate);
        SetDlgItemText(hwndDlg, IDC_SAMPLERATE, info);

        wsprintf(info, "%d ch", format.channels);
        SetDlgItemText(hwndDlg, IDC_CHANNELS, info);

        FillID3List(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), info_fn);

        return TRUE;

    case WM_NOTIFY:
        
        /* Branch depending on the specific notification message. */
        switch (((LPNMHDR) lParam)->code) { 
            
        /* Process LVN_GETDISPINFO to supply information about */
        /* callback items. */
        case LVN_GETDISPINFO:
            List_OnGetDispInfo((LV_DISPINFO *)lParam);
            break;
        case NM_DBLCLK:
            List_EditData(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), info_fn);
            break;
        }
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_ADDSTFRAMES:
            List_AddStandardFrames(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            return TRUE;
        case IDC_ADDFRAME:
            List_AddFrame(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            return TRUE;
        case IDC_DELFRAME:
            List_DeleteSelected(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST));
            return TRUE;
        case IDC_EDITFRAME:
            List_EditData(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), info_fn);
            return TRUE;

        case IDC_CLOSE:
            List_SaveID3(hwndDlg, GetDlgItem(hwndDlg, IDC_ID3LIST), info_fn);
        case IDCANCEL:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}

int infoDlg(char *fn, HWND hwnd)
{
    lstrcpy(info_fn, fn);

    DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_INFO),
        hwnd, info_dialog_proc);

    return 0;
}

LPTSTR PathFindFileName(LPCTSTR pPath)
{
    LPCTSTR pT;

    for (pT = pPath; *pPath; pPath = CharNext(pPath)) {
        if ((pPath[0] == TEXT('\\') || pPath[0] == TEXT(':')) && pPath[1] && (pPath[1] != TEXT('\\')))
            pT = pPath + 1;
    }

    return (LPTSTR)pT;   // const -> non const
}

int getsonglength(char *fn)
{
    faadAACInfo tmp;

    get_AAC_format(fn, &tmp, seek_table);

    return tmp.length;
}

void getfileinfo(char *filename, char *title, int *length_in_ms)
{
    if (!filename || !*filename)  // currently playing file
    {
        if (length_in_ms)
            *length_in_ms=getlength();

        if (title)
            GetID3FileTitle(lastfn, title, format_string);
    }
    else // some other file
    {
        if (length_in_ms)
            *length_in_ms = getsonglength(filename);

        if (title)
            GetID3FileTitle(filename, title, format_string);
    }
}

void eq_set(int on, char data[10], int preamp)
{
}

DWORD WINAPI PlayThread(void *b)
{
    int done=0;
    int last_frame = 0, result;
    int l = 1024*2*file_info.channels;
    int k;
	int decoded_frames=0;
	int br_calc_frames=0;
	int br_bytes_consumed=0;
    unsigned long bytesconsumed;

	PlayThreadAlive = 1;

    while (! *((int *)b) )
    {
        if (seek_needed != -1)
        {
            int seconds;

            // Round off to a second
            seconds = seek_needed - (seek_needed%1000);
            mod.outMod->Flush(decode_pos_ms);
            seek_needed = -1;
            done=0;
            aac_seek(seconds, seek_table);
            decode_pos_ms = seconds;
			decoded_frames = 0;
			br_calc_frames = 0;
			br_bytes_consumed = 0;
        }

        if (done)
        {
            mod.outMod->CanWrite();

            if (!mod.outMod->IsPlaying())
            {
                PostMessage(mod.hMainWindow,WM_WA_AAC_EOF,0,0);
                PlayThreadAlive = 0;
                return 0;
            }

            Sleep(10);
        }
        //assume that max channels is 2.
        else if (mod.outMod->CanWrite() >= ((1024*file_info.channels*2)<<(mod.dsp_isactive()?1:0)))
        {
            if(last_frame)
            {
                done=1;
            }
            else
            {
                if (buffercount > 0) {
                    for (k = 0; k < (768*2 - buffercount); k++)
                        buffer[k] = buffer[k + buffercount];

                    read_buffer_filestream(infile, buffer + (768*2) - buffercount, buffercount);
                    buffercount = 0;
                }

                result = faacDecDecode(hDecoder, buffer, &bytesconsumed, (short*)sample_buffer);
                if (result == FAAD_FATAL_ERROR) {
                    last_frame = 1;
                }

                buffercount += bytesconsumed;

                bytecount += bytesconsumed;
                if (bytecount >= fileread)
                    last_frame = 1;

				decoded_frames++;
				br_calc_frames++;
				br_bytes_consumed += bytesconsumed;

				/* Update the variable bitrate about every second */
				if(vbr_display && br_calc_frames == 43)
				{
					int br;

					br = (br_bytes_consumed * 8) / (decoded_frames / 43.07);

					mod.SetInfo(br/1000, file_info.sampling_rate/1000, file_info.channels, 1);
					file_info.bitrate = br;
					file_info.length = (int)((filelength_filestream(infile)/(((file_info.bitrate*8)/1024)*16))*1000);
					br_calc_frames = 0;
				}

                if (!killPlayThread)
                {
                    mod.SAAddPCMData(sample_buffer,file_info.channels, 16, decode_pos_ms);
                    mod.VSAAddPCMData(sample_buffer,file_info.channels, 16, decode_pos_ms);
                    decode_pos_ms+=(1024*1000)/file_info.sampling_rate;

                    if (mod.dsp_isactive())
                        l=mod.dsp_dosamples((short *)sample_buffer,l/file_info.channels/(16/8),16,file_info.channels,file_info.sampling_rate)*(file_info.channels*(16/8));

                    mod.outMod->Write(sample_buffer,l);
                }
            }
        }
        else
        {
            Sleep(10);
        }
    }

    PlayThreadAlive = 0;

    return 0;
}

int id3v2_tag(unsigned char *buffer)
{
    if (StringComp(buffer, "ID3", 3) == 0)
	{
        unsigned long tagsize;

        /* high bit is not used */
        tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
            (buffer[8] <<  7) | (buffer[9] <<  0);

        tagsize += 10;

        return tagsize;
    }
	else
	{
        return 0;
    }
}

In_Module mod =
{
    IN_VER,
    "Freeware AAC Decoder: " __DATE__,
    0,  // hMainWindow
    0,  // hDllInstance
    "AAC\0AAC File (*.AAC)\0"
    ,
    1, // is_seekable
    1, // uses output
    config,
    about,
    init,
    quit,
    getfileinfo,
    infoDlg,
    isourfile,
    play,
    pause,
    unpause,
    ispaused,
    stop,

    getlength,
    getoutputtime,
    setoutputtime,

    setvolume,
    setpan,

    0,0,0,0,0,0,0,0,0, // vis stuff


    0,0, // dsp

    eq_set,

    NULL,       // setinfo

    0 // out_mod
};

__declspec( dllexport ) In_Module * winampGetInModule2()
{
    return &mod;
}
