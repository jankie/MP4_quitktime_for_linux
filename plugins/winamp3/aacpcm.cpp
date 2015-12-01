/*
 * FAAD - Winamp3 plugin
 * Copyright (C) 2001 Menno
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
 * $Id: aacpcm.cpp,v 1.8 2001/11/06 14:13:08 menno Exp $
 */

#include <stdio.h>
#include "aacpcm.h"

#define AAC_BUF_SIZE	(768*2)

AacPcm::AacPcm()
{
    hDecoder=0;
    buffer=0;
    bufout=0;
}

AacPcm::~AacPcm()
{
	if(hDecoder)
		faacDecClose(hDecoder);
	if(buffer)
		delete buffer;
	if(bufout)
		delete bufout;
//	fclose(fil);
}

#define ERROR_getInfos(str) \
	{ \
		bytes_into_buffer=-1; \
		infos->status(str); \
	    return 1; \
	}

int AacPcm::getInfos(MediaInfo *infos)
{
DWORD	tmp;

    infos->setTitle(Std::filename(infos->getFilename()));

	if(hDecoder)
	    infos->setInfo(StringPrintf("%ihz %ibps %ich", dwSamprate, 16, dwChannels));
	else
	{
	svc_fileReader *reader = infos->getReader();
		if (!reader)
			ERROR_getInfos("File doesn\'t exists")

	    buffer=new BYTE[AAC_BUF_SIZE];
		if(!buffer)
			ERROR_getInfos("Error: buffer=NULL")

		lSize=reader->getLength();
		if(lSize>AAC_BUF_SIZE)
			tmp=AAC_BUF_SIZE;
		else
			tmp=lSize;

		bytes_into_buffer=bytes_read=reader->read((char *)buffer, tmp);
		if(bytes_read!=tmp)
			ERROR_getInfos("Read failed!")

	    hDecoder = faacDecOpen();

	faacDecConfigurationPtr config;
		config = faacDecGetCurrentConfiguration(hDecoder);
		config->defSampleRate = 44100;
		faacDecSetConfiguration(hDecoder, config);

		if((bytes_consumed=faacDecInit(hDecoder, buffer, &dwSamprate, &dwChannels))<0)
			ERROR_getInfos("faacDecInit failed!")
		else
		    infos->setInfo(StringPrintf("%ihz %ibps %ich", dwSamprate, 16, dwChannels));

		bytes_into_buffer-=bytes_consumed;

		bufout=new short[1024*dwChannels];
		if(!bufout)
			ERROR_getInfos("Error: bufout=NULL")

//		fil=fopen("c:\\prova.wav","wb");
	}

    return 0;
}

#define ERROR_processData(str) \
	{ \
		bytes_into_buffer=-1; \
		if(str) \
			infos->status(str); \
	    chunk_list->setChunk("PCM", bufout, 0, ci); \
	    return 0; \
	}

int AacPcm::processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch)
{
DWORD		read,
			tmp,
			shorts_decoded=0;
long		result=0;
ChunkInfosI *ci = new ChunkInfosI();

	ci->addInfo("srate", dwSamprate); 
    ci->addInfo("bps", 16); 
	ci->addInfo("nch", dwChannels); 

	if(bytes_into_buffer<0)
		ERROR_processData(0);

svc_fileReader *reader = infos->getReader();
	if(!reader)
		ERROR_processData("File doesn\'t exists");

	do
	{
		if(bytes_consumed>0 && bytes_into_buffer>=0)
		{
			if(bytes_into_buffer)
				memcpy(buffer,buffer+bytes_consumed,bytes_into_buffer);

			if(bytes_read<lSize)
			{
				if(bytes_read+bytes_consumed<lSize)
					tmp=bytes_consumed;
				else
					tmp=lSize-bytes_read;
				read=reader->read((char *)buffer+bytes_into_buffer, tmp);
				if(read==tmp)
				{
					bytes_read+=read;
					bytes_into_buffer+=read;
				}
				else
					infos->status("Read failed!"); // continue until bytes_into_buffer<1
			}
			else
				if(bytes_into_buffer)
					memset(buffer+bytes_into_buffer, 0, bytes_consumed);
		}

		if(bytes_into_buffer<1)
			if(bytes_read<lSize)
				ERROR_processData("Buffer empty!")
			else
				return 0;

		result=faacDecDecode(hDecoder, buffer, &bytes_consumed, (short*)bufout, &shorts_decoded);
		bytes_into_buffer-=bytes_consumed;
	}while(!shorts_decoded || result==FAAD_OK_CHUPDATE);


	if(result==FAAD_FATAL_ERROR || result==FAAD_ERROR)
		ERROR_processData("FAAD_FATAL_ERROR or FAAD_ERROR");

	shorts_decoded*=sizeof(short);

    chunk_list->setChunk("PCM", bufout, shorts_decoded, ci);
//	fwrite(bufout,1,bytesDec,fil);

    if(!shorts_decoded)
		ERROR_processData(0);

    return 1;
}
