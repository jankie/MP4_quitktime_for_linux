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
 * $Id: aacinfo.c,v 1.2 2001/10/20 20:31:17 rageomatic Exp $
 */

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include "filestream.h"
#include "aacinfo.h"

#define ADIF_MAX_SIZE 30 /* Should be enough */
#define ADTS_MAX_SIZE 10 /* Should be enough */

const int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};

static int read_ADIF_header(FILE_STREAM *file, faadAACInfo *info)
{
    int bitstream;
    unsigned char buffer[ADIF_MAX_SIZE];
    int skip_size = 0;
    int sf_idx;

    /* Get ADIF header data */
    info->headertype = 1;

    if(read_buffer_filestream(file, buffer, ADIF_MAX_SIZE) < 0)
		return -1;

    /* copyright string */
    if(buffer[4] & 128)
        skip_size += 9; /* skip 9 bytes */

    bitstream = buffer[4 + skip_size] & 16;
    info->bitrate = ((unsigned int)(buffer[4 + skip_size] & 0x0F)<<19)|
        ((unsigned int)buffer[5 + skip_size]<<11)|
        ((unsigned int)buffer[6 + skip_size]<<3)|
        ((unsigned int)buffer[7 + skip_size] & 0xE0);

    if (bitstream == 0) {
        info->object_type = ((buffer[9 + skip_size]&0x01)<<1)|((buffer[10 + skip_size]&0x80)>>7);
        sf_idx = (buffer[10 + skip_size]&0x78)>>3;
        info->channels = ((buffer[10 + skip_size]&0x07)<<1)|((buffer[11 + skip_size]&0x80)>>7);
    } else {
        info->object_type = (buffer[7 + skip_size] & 0x18)>>3;
        sf_idx = ((buffer[7 + skip_size] & 0x07)<<1)|((buffer[8 + skip_size] & 0x80)>>7);
        info->channels = (buffer[8 + skip_size]&0x78)>>3;
    }
    info->sampling_rate = sample_rates[sf_idx];

    return 0;
}

static int read_ADTS_header(FILE_STREAM *file, faadAACInfo *info, unsigned long **seek_table, int *seek_table_len, int tagsize)
{
    /* Get ADTS header data */
    unsigned char buffer[ADTS_MAX_SIZE];
    int frames, framesinsec=0, t_framelength = 0, frame_length, sr_idx, ID;
    int second = 0, pos;
	int i;
    float frames_per_sec = 0;
    unsigned long bytes;
	unsigned long *tmp_seek_table = NULL;

    info->headertype = 2;

    /* Read all frames to ensure correct time and bitrate */
    for(frames=0; /* */; frames++, framesinsec++)
    {
		/* If streaming, only go until we hit 5 seconds worth */
		 if(file->http)
		 {
			 if(frames >= 43 * 5)
			 {
				break;
			 }
		 }

        pos = tell_filestream(file);

        /* 12 bit SYNCWORD */
        bytes = read_buffer_filestream(file, buffer, ADTS_MAX_SIZE);

        if(bytes != ADTS_MAX_SIZE)
        {
            /* Bail out if no syncword found */
            break;
        }

		/* check syncword */
        if (!((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)))
            break;

        if(!frames)
        {
            /* fixed ADTS header is the same for every frame, so we read it only once */
            /* Syncword found, proceed to read in the fixed ADTS header */
            ID = buffer[1] & 0x08;
            info->object_type = (buffer[2]&0xC0)>>6;
            sr_idx = (buffer[2]&0x3C)>>2;
            info->channels = ((buffer[2]&0x01)<<2)|((buffer[3]&0xC0)>>6);

            frames_per_sec = sample_rates[sr_idx] / 1024.f;
        }

        /* ...and the variable ADTS header */
        if (ID == 0) {
            info->version = 4;
            frame_length = (((unsigned int)buffer[4]) << 5) |
                ((unsigned int)buffer[5] >> 3);
        } else { /* MPEG-2 */
            info->version = 2;
            frame_length = ((((unsigned int)buffer[3] & 0x3)) << 11)
                | (((unsigned int)buffer[4]) << 3) | (buffer[5] >> 5);
        }

        t_framelength += frame_length;

		if(!file->http)
		{
			if(framesinsec == 43)
				framesinsec = 0;

			if(framesinsec == 0 && seek_table_len)
			{
				tmp_seek_table = (unsigned long *) realloc(tmp_seek_table, (second + 1) * sizeof(unsigned long));
				tmp_seek_table[second] = pos;
				second++;
			}
		}

		/* NOTE: While simply skipping ahead by reading may seem to be more work than seeking,
			it is actually much faster, and keeps compatibility with streaming */
		for(i=0; i < frame_length - ADTS_MAX_SIZE; i++)
			if(read_byte_filestream(file) < 0)
				break;
    }

	if(seek_table_len)
	{
		*seek_table_len = second;
		*seek_table = tmp_seek_table;
	}

    info->sampling_rate = sample_rates[sr_idx];
    info->bitrate = (int)(((t_framelength / frames) * (info->sampling_rate/1024.0)) +0.5)*8;

	if(file->http)
	{
		/* Since we only use 5 seconds of aac data to get a rough bitrate, we must use a different
		   method of calculating the overall length */
		if(filelength_filestream(file))
		{
			info->length = (int)((filelength_filestream(file)/(((info->bitrate*8)/1024)*16))*1000);
		}
		else
		{
			/* Since the server didnt tell us how long the file is, 
			   we have no way of determining length */
			info->length = 0;
		}
	}
	else
	{
		info->length = (int)((float)(frames/frames_per_sec))*1000;
	}

    return 0;
}

int get_AAC_format(char *filename, faadAACInfo *info, unsigned long **seek_table, int *seek_table_len)
{
    unsigned long tagsize;
    FILE_STREAM *file;
	char buffer[10];
    unsigned long file_len;
    unsigned char adxx_id[5];
    unsigned long tmp;

    file = open_filestream(filename);

    if(file == NULL)
        return -1;

    file_len = filelength_filestream(file);

    /* Skip the tag, if it's there */
    tmp = read_buffer_filestream(file, buffer, 10);

    if (StringComp(buffer, "ID3", 3) == 0)
	{
		int i;

        /* high bit is not used */
        tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
            (buffer[8] <<  7) | (buffer[9] <<  0);

        for(i=0; i < tagsize; i++)
			if(read_byte_filestream(file) < 0)
				return -1;

        tagsize += 10;
    }
	else
	{
        tagsize = 0;

		/* Simple hack to reset to the beginning */
		file->buffer_offset = 0;
		file->file_offset = 0;
    }

	if(file_len)
	    file_len -= tagsize;

    tmp = read_buffer_filestream(file, adxx_id, 2);
    //seek_filestream(file, tagsize, FILE_BEGIN);

    adxx_id[5-1] = 0;
    info->length = 0;

	/* Determine the header type of the file, check the first two bytes */
    if(StringComp(adxx_id, "AD", 2) == 0)
    {
		/* We think its an ADIF header, but check the rest just to make sure */
		tmp = read_buffer_filestream(file, adxx_id + 2, 2);
		
		if(StringComp(adxx_id, "ADIF", 4) == 0)
		{
			read_ADIF_header(file, info);
		}
    }
    else
    {
		/* No ADIF, check for ADTS header */
        if ((adxx_id[0] == 0xFF)&&((adxx_id[1] & 0xF6) == 0xF0))
        {
			/* ADTS  header located */
			/* Since this routine must work for streams, we can't use the seek function to go backwards, thus 
			   we have to use a quick hack as seen below to go back where we need to. */
			
			if(file->buffer_offset >= 2)
			{
				// simple seeking hack, though not really safe, the probability of it causing a problem is low.
				file->buffer_offset -= 2;
				file->file_offset -= 2;
			}

            read_ADTS_header(file, info, seek_table, seek_table_len, tagsize);
        }
        else
        {
            /* Unknown/headerless AAC file, assume format: */
            info->version = 2;
            info->bitrate = 128000;
            info->sampling_rate = 44100;
            info->channels = 2;
            info->headertype = 0;
            info->object_type = 1;
        }
    }

    close_filestream(file);

    return 0;
}

int StringComp(char const *str1, char const *str2, unsigned long len)
{
    signed int c1 = 0, c2 = 0;

    while (len--) {
        c1 = *str1++;
        c2 = *str2++;

        if (c1 == 0 || c1 != c2)
            break;
    }

    return c1 - c2;
}
