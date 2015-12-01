#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <crtdbg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" 
{
#endif




#include "SQPluginplayer.h"
#include "aacinfo.h"
#include "..\include\faad.h"

typedef void *faacDecHandle;
faacDecHandle hDecoder;

FILE *infile; 

typedef struct {

	char				szFileName[2600];
	char				szTitle[1024];
	char				szFormat[512];
	char				szRate[256];
	unsigned long		lSongLength;
	CRITICAL_SECTION	cs;
	HANDLE				hThread;
	unsigned long		dwThreadID;
	BOOL				bPlay;
	faadAACInfo			AAC;
	BOOL				bDoSeek;
	unsigned long		seekOffset;
	BOOL				bAveBitrate;
	int					lastBitrate;

} UserStruct;

QueryInfo	 Vorbis_QueryStruct = { 
			0, 
			"AAC", 
			"AAC",
			"Copyright (c) 2001",
			0x00000005,
			"aac\0audio/x-aac\0",
			0,
			PS_CONFIGURE,//PS_USES_OWN_FILEIO,
			0 };



size_t read_func  (void *ptr, size_t size, size_t nmemb, void *datasource)
{
	InstanceInfo* IInfo = (InstanceInfo*)datasource;
	unsigned long BytesRead;
	if( !IInfo->Read((unsigned char*)ptr, size*nmemb, &BytesRead) )
		BytesRead = -1;
	return BytesRead;
}

int    seek_func  (void *datasource,  long offset, int whence)
{
	
	InstanceInfo* IInfo = (InstanceInfo*)datasource;
	long NewWhence;
	unsigned long OldOffset;

	switch(whence)
	{
	case SEEK_SET:
		NewWhence = FILE_BEGIN;
		break;
	case SEEK_END:
		NewWhence = FILE_END;
		break;
	case SEEK_CUR:
		NewWhence = FILE_CURRENT;
		break;
	}
	
	if( !IInfo->Seek((int)offset, NewWhence, &OldOffset) ||
		!IInfo->Seek(0, FILE_CURRENT, &OldOffset) )
		OldOffset = -1;
	return OldOffset;
	
}

int    close_func (void *datasource)
{
	return 0;
}

long   tell_func  (void *datasource)
{
	InstanceInfo* IInfo = (InstanceInfo*)datasource;
	unsigned long Pos;
	if( !IInfo->Seek(0, FILE_CURRENT, &Pos) )
		Pos = -1;
	return Pos;
}

int id3v2_tag(unsigned char *buffer)
{

	if (strncmp((const char*)buffer, "ID3", 3) == 0) {
		unsigned long tagsize;

		/* high bit is not used */
		tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
			(buffer[8] <<  7) | (buffer[9] <<  0);

		tagsize += 10;

		return tagsize;
	} else {
		return 0;
	}
}


unsigned char *buffer; 
int *seek_table;
char *sample_buffer; // sample buffer

unsigned long __stdcall AACPlayProc( LPVOID param )
{
	
	InstanceInfo*	thisInst	= (InstanceInfo*)	param;
	UserStruct*		pUS			= (UserStruct*)		thisInst->User;
	seek_table = (int*)malloc(10800*sizeof(int));
	get_AAC_format(pUS->szFileName,&pUS->AAC,seek_table);
	
	buffer = (unsigned char*)malloc(768*2*2);
	const int BufferSize = 1024*2;
	AudioInfoStruct				AIS;
	AIS.MillSec = 0;
		int previous_link = -1;
	int current_link = 0;
	thisInst->PluginID = -1;

	pUS->bDoSeek = FALSE;

	int id = thisInst->OpenAudio( pUS->AAC.sampling_rate, pUS->AAC.channels, FALSE );

	if( thisInst->PluginID == -1 )
	thisInst->PluginID = id;
	thisInst->MessageCallback(MSG_UPDATED_INFO, NULL);
	
	char fn[2600];
	strcpy(fn,pUS->szFileName);
	long filelength;	
	sample_buffer = (char*)malloc(1024*2*(16/8)*sizeof(char));
	memset(sample_buffer, 0, 1024*2*(16/8)*sizeof(char));
	long buffercount=0,bytecount=0,tagsize=0;
	infile = fopen(fn, "rb");
		
	fseek(infile, 0, SEEK_END);
	filelength = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	memset(buffer, 0, 768*2);
	fread(buffer, 1, 768*2, infile);
	tagsize = id3v2_tag(buffer);
	if (tagsize) {
		fseek(infile, tagsize, SEEK_SET);

		bytecount = tagsize;
		buffercount = 0;
		fread(buffer, 1, 768*2, infile);
	}

	hDecoder = faacDecOpen();

	buffercount = faacDecInit(hDecoder, buffer, (unsigned long *)&pUS->AAC.sampling_rate, (unsigned long *)&pUS->AAC.channels);
	
	if (buffercount > 0) 
	{
			bytecount += buffercount;

			for (int k = 0; k < (768*2 - buffercount); k++)
				buffer[k] = buffer[k + buffercount];

			fread(buffer + (768*2) - buffercount, 1, buffercount, infile);
			buffercount = 0;
	}
	

		
	int done=0;
	int last_frame = 0, bits;
    int l = 1024*2*pUS->AAC.channels;
	unsigned long bytesconsumed;
	thisInst->MessageCallback(MSG_UPDATED_INFO, NULL);

	while( pUS->bPlay )
	{
	//	EnterCriticalSection( &pUS->cs );
		if (buffercount > 0) {
		for (int k = 0; k < (768*2 - buffercount); k++)
		buffer[k] = buffer[k + buffercount];
		fread(buffer + (768*2) - buffercount, 1, buffercount, infile);
		buffercount = 0;
		}

		bits = faacDecDecode(hDecoder, buffer, &bytesconsumed, (short*)sample_buffer);
		
		AIS.pAudioData = sample_buffer;
		AIS.BufferLength = l;
		AIS.MaxMillSec   = pUS->lSongLength;
		AIS.MillSec      = (int)(((float)ftell(infile)/(float)filelength)*(float)pUS->lSongLength);
		thisInst->PutAIS( thisInst->PluginID, &AIS );
		buffercount += bytesconsumed;

		bytecount += bytesconsumed;
		
		if ((bytecount >= filelength) || (bits < 0))
		{	last_frame = 1, done = 1; }
		
		if( pUS->bDoSeek )
		{
			fseek(infile, seek_table[(int)((pUS->seekOffset/1000)+0.5)], SEEK_SET);
			bytecount = seek_table[(int)((pUS->seekOffset/1000)+0.5)];
			buffercount = 0;
			fread(buffer, 1, 768*2, infile);
			pUS->bDoSeek = FALSE;
			
		}
		
		if ( done ) break;
	}
		
		
			faacDecClose(hDecoder);
			fclose(infile);
			free(sample_buffer);
			free(buffer);
			free(seek_table);
			thisInst->DoneProcessing( thisInst->PluginID );
			Sleep(10);
			pUS->bPlay = FALSE;
			//LeaveCriticalSection( &pUS->cs );
		
	return TRUE;
}


BOOL  OpenFilename(  InstanceInfo* thisInst, char *FileName)
{
	UserStruct* pUS = (UserStruct*)thisInst->User;
	strcpy(pUS->szFileName, FileName);
	return TRUE;
}

BOOL  	Close(  InstanceInfo* thisInst)
{
	UserStruct* pUS = (UserStruct*)thisInst->User;

	return TRUE;
}

BOOL  	Play(  InstanceInfo* thisInst)
{
	UserStruct* pUS = (UserStruct*)thisInst->User;
	pUS->bPlay			= TRUE;
	pUS->hThread		= (HANDLE) CreateThread( NULL, NULL, &AACPlayProc, thisInst, NULL, &pUS->dwThreadID );
	
	return TRUE;
}
	
BOOL  	Stop(  InstanceInfo* thisInst)
{

	UserStruct* pUS = (UserStruct*)thisInst->User;

	BOOL bRet = FALSE;

	if( pUS->bPlay )
	{

		pUS->bPlay = FALSE;
		{
				if( WaitForSingleObject( pUS->hThread, INFINITE ) == WAIT_OBJECT_0 )
				{
					CloseHandle( pUS->hThread );
					pUS->hThread = NULL;
					Sleep(10);
				}
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL  	SeekTo(  InstanceInfo* thisInst, int unitnum)
{

	UserStruct* pUS = (UserStruct*)thisInst->User;

	BOOL bRet = FALSE;

		pUS->bDoSeek = true;
		pUS->seekOffset  = unitnum;
		bRet = TRUE;

	return bRet;
}

#define lenof(x) (sizeof(x)/sizeof(*(x)))
#define strlenof(x) (lenof(x)-1)

BOOL  	GetInfo(  InstanceInfo* thisInst, PlayInfo *pPInfo)
{
	UserStruct* pUS = (UserStruct*)thisInst->User;
	int *seek_table;
	seek_table = (int*)malloc(10800*sizeof(int));
	get_AAC_format(pUS->szFileName,&pUS->AAC,seek_table);
	free(seek_table);
	sprintf( pUS->szRate,   "%s %02.1fkhz", ((pUS->AAC.channels==2) ? "stereo" : "mono") , (float)pUS->AAC.sampling_rate/1000.0 );
	wsprintf( pUS->szFormat, "%s %dkbps", "AAC" , pUS->AAC.bitrate/1000 );

	pUS->lSongLength = pUS->AAC.length;
		pPInfo->Artist				= NULL;
		pPInfo->Title				= NULL;//us->szTitle;
		pPInfo->Rate				= pUS->szRate;
		pPInfo->Format				= pUS->szFormat;
		pPInfo->Length				= pUS->lSongLength;
		pPInfo->bStereo				= (pUS->AAC.channels==2);

		return TRUE;
}


// Exported..

__declspec( dllexport )QueryInfo *Query(void)
{
	return &Vorbis_QueryStruct;
}

__declspec( dllexport ) BOOL Initialise(InstanceInfo *IInfo)
{
	IInfo->OpenFilename	= OpenFilename;
	IInfo->Close		= Close;
	IInfo->Play			= Play;
	IInfo->Stop			= Stop;
	IInfo->SeekTo		= SeekTo;
	IInfo->GetInfo		= GetInfo;


	// Allocate some mem for the user struct
	IInfo->User		= GlobalAlloc( GMEM_FIXED, sizeof( UserStruct ) );
	InitializeCriticalSection( &((UserStruct*)IInfo->User)->cs );

	((UserStruct*)IInfo->User)->bPlay=FALSE;


//	LoadSettings(TRUE);

	return(TRUE);
}

__declspec( dllexport ) BOOL Shutdown(InstanceInfo *IInfo)
{
	IInfo->Stop(IInfo);
	//LoadSettings( FALSE );
	DeleteCriticalSection( &((UserStruct*)IInfo->User)->cs );
	GlobalFree( IInfo->User );
	return(TRUE);
}



#define IDC_TITLETEXT	000

long ConfigDialogProc(
	long hDialog, 
	long lMsg, 
	long lParam1, 
	long lParam2, 
	SendMessageProc pfnSendMessage )
{
	switch( lMsg )
	{
		case SQI_INIT:
			{
				int DecMethod = 0;

				long color;


						pfnSendMessage(hDialog, SQI_SETFONTSIZE | SQI_ITEMMESG, IDC_TITLETEXT,	12 );
				color = pfnSendMessage(hDialog, SQI_GET_SYSCOLOR | SQI_ITEMMESG, IDC_TITLETEXT, SQ_SYSCOLOR_STATIC );
						pfnSendMessage(hDialog, SQI_SETCOLOR	 | SQI_ITEMMESG, IDC_TITLETEXT, color );

			}
			break;

		case SQI_DESTROY:
			GlobalFree( ((SQDialog*) lParam1)->pNCI );
			GlobalFree( (void *)lParam1 );
			break;

		case SQI_CLICKED:
			{
	/*			switch( lParam1 )
				{
				}*/
			}
			break;
	}

	return(0);
}

__declspec( dllexport ) BOOL Configure(ConfigInfo *CInfo)
{
	NavControlItem* pNCI;

	int i;
	CInfo->pSQDialog = (SQDialog*) GlobalAlloc(GPTR, sizeof(SQDialog) );
	CInfo->pSQDialog->nNumNCI = 1;

	CInfo->pSQDialog->pNCI = (NavControlItem*) GlobalAlloc(GPTR, sizeof (NavControlItem) * CInfo->pSQDialog->nNumNCI);
	ZeroMemory( CInfo->pSQDialog->pNCI, sizeof( NavControlItem ) * CInfo->pSQDialog->nNumNCI );

	CInfo->pSQDialog->pfnCallBack = ConfigDialogProc;

	i=0;

// Note : 
	pNCI = &CInfo->pSQDialog->pNCI[i];
	pNCI->nStartPoint_x = -i*150-300;
	pNCI->nStartPoint_y =  i*100+100;
	pNCI->nEndPoint_x   = 20;
	pNCI->nEndPoint_y   = 25;
	pNCI->nWidth        = 200;
	pNCI->nHeight       = 100;
	pNCI->nControlType  = SQ_STATIC;
	pNCI->nControlID    = IDC_TITLETEXT;
	pNCI->szControlText = "Nic's AAC Player v1.5";

	return(TRUE);
}
#ifdef __cplusplus
}
#endif
