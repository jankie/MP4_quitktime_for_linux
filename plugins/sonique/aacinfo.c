
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aacinfo.h"

#define ADIF_MAX_SIZE 30 /* Should be enough */
#define ADTS_MAX_SIZE 30 /* Should be enough */

const int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};

static int read_ADIF_header(FILE *file, faadAACInfo *info)
{
	int bitstream;
	unsigned char buffer[ADIF_MAX_SIZE];
	int skip_size = 0;
	int sf_idx;

	char n[80];

	/* Get ADIF header data */
	
	info->headertype = 1;

	fread(buffer, 1, ADIF_MAX_SIZE, file);

	/* copyright string */	
	if(buffer[4] & 128)
		skip_size += 9; /* skip 9 bytes */

	bitstream = buffer[4 + skip_size] & 16;
	info->bitrate = 128000;/*((unsigned int)(buffer[4 + skip_size] & 0x0F)<<19)|
		((unsigned int)buffer[5 + skip_size]<<11)|
		((unsigned int)buffer[6 + skip_size]<<3)|
		((unsigned int)buffer[7 + skip_size] & 0xE0);
*/
	if (bitstream == 0) {
		info->object_type = buffer[12 + skip_size] & 0x18;
		sf_idx = ((buffer[12 + skip_size] & 0x07)<<1)|((buffer[12 + skip_size] & 0x80)>>7);
	} else {
		info->object_type = ((buffer[8 + skip_size]&0x01)<<1)|((buffer[9 + skip_size]&0x80)>>7);
		sf_idx = (buffer[9 + skip_size]&0x78)>>3;
	}
	
	itoa(sample_rates[sf_idx],n,10);
	//MessageBox(0,n,"hm",0);
	info->sampling_rate = 44100;//sample_rates[sf_idx];

	return 0;
}

static int read_ADTS_header(FILE *file, faadAACInfo *info, int *seek_table,
							int tagsize)
{
	/* Get ADTS header data */
	unsigned char buffer[ADTS_MAX_SIZE];
	int frames, t_framelength = 0, frame_length, sr_idx, ID;
	int second = 0, pos;
	double frames_per_sec = 0;

	info->headertype = 2;

	/* Seek to the first frame */
	fseek(file, tagsize, SEEK_SET);

	/* Read all frames to ensure correct time and bitrate */
	for(frames=0; /* */; frames++)
	{
		/* 12 bit SYNCWORD */
		if(fread(buffer, 1, ADTS_MAX_SIZE, file) != ADTS_MAX_SIZE)
		{
			/* Bail out if no syncword found */
			break;
		}

		if (!((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)))
			break;

		pos = ftell(file) - ADTS_MAX_SIZE;

		if(!frames)
		{
			/* fixed ADTS header is the same for every frame, so we read it only once */ 
			/* Syncword found, proceed to read in the fixed ADTS header */ 
			ID = buffer[1] & 0x08;
			info->object_type = (buffer[2]&0xC0)>>6;
			sr_idx = (buffer[2]&0x3C)>>2;
			info->channels = ((buffer[2]&0x01)<<2)|((buffer[3]&0xC0)>>6);

			frames_per_sec = sample_rates[sr_idx] / 1024.;
		}

		/* ...and the variable ADTS header */
		if (ID == 0) {
			frame_length = (((unsigned int)buffer[4]) << 5) |
				((unsigned int)buffer[5] >> 3);
		} else { /* MPEG-2 */
			frame_length = ((((unsigned int)buffer[3] & 0x3)) << 11)
				| (((unsigned int)buffer[4]) << 3) | (buffer[5] >> 5);
		}

		t_framelength += frame_length;

		if (frames > second*frames_per_sec)
		{
			seek_table[second] = pos;
			second++;
		}

		fseek(file, frame_length - ADTS_MAX_SIZE, SEEK_CUR);
	}

	info->sampling_rate = sample_rates[sr_idx];
	info->bitrate = (int)(((t_framelength / frames) * (info->sampling_rate/1024.0)) +0.5)*8;
	info->length = (int)((double)(frames/frames_per_sec))*1000;

	return 0;
}

static int f_id3v2_tag(FILE *file)
{
	unsigned char buffer[10];

	fread(buffer, 1, 10, file);

	if (strncmp(buffer, "ID3", 3) == 0) {
		unsigned long tagsize;

		/* high bit is not used */
		tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
			(buffer[8] <<  7) | (buffer[9] <<  0);

		tagsize += 10;

		fseek(file, tagsize, SEEK_SET);

		return tagsize;
	} else {
		fseek(file, 0, SEEK_SET);

		return 0;
	}
}

int get_AAC_format(char *filename, faadAACInfo *info, int *seek_table)
{
	unsigned int tagsize;
	FILE *file;
	unsigned long file_len;
	char adxx_id[5];

	if((file = fopen(filename, "rb")) == NULL)
		return 1;

	fseek(file, 0, SEEK_END);
	file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	tagsize = f_id3v2_tag(file); /* Skip the tag, if it's there */
	file_len -= tagsize;

	fread(adxx_id, 1, 4, file);
	fseek(file, tagsize, SEEK_SET);

	adxx_id[5-1] = 0;

	info->length = 0;

	if(strncmp(adxx_id, "ADIF", 4) == 0)
	{
		read_ADIF_header(file, info);
	}
	else
	{
		if ((fgetc(file) == 0xFF)&&((fgetc(file) & 0xF6) == 0xF0))
		{
			fseek(file, tagsize, SEEK_SET);
			read_ADTS_header(file, info, seek_table, tagsize);
		}
		else
{
			/* Unknown/headerless AAC file, assume format: */
			info->bitrate = 128000;
			info->sampling_rate = 44100;
			info->channels = 2;
			info->headertype = 0;
}
	}

	if(info->headertype == 1)
	{
		int chans;
		char n[100];
		fseek(file, 7 + tagsize, SEEK_SET);
		chans = (fgetc(file)<<2)>>5;

		itoa(chans,n,10);
		//MessageBox(0,n,"hm2",0);
		if(chans == 1)
			info->channels = 2; /* stereo */
		else
			info->channels = 1; /* mono */
		info->channels = 2; /* stereo */
	}

	if (info->length == 0)
		info->length = (int)((file_len/(((info->bitrate*8)/1024)*16))*1000);

	fclose(file);

	return 0;
}
