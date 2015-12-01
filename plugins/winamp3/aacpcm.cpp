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
 * $Id: aacpcm.cpp,v 1.6 2001/10/26 11:57:00 menno Exp $
 */

#include "aacpcm.h"

AacPcm::AacPcm()
{
    hDecoder = faacDecOpen();
    buffercount = 0;
    bytecount = 0;
    init_called = 0;

    buffer = new char[768*2];
    memset(buffer, 0, 768*2);
}

AacPcm::~AacPcm()
{
    delete buffer;
    faacDecClose(hDecoder);
}

int AacPcm::getInfos(MediaInfo *infos)
{
    /*
    svc_fileReader *reader = infos->getReader();
    if (!reader)
        return 0;
    */

    infos->setTitle(Std::filename(infos->getFilename()));
    infos->setInfo(StringPrintf("%ihz %ibps %ich", 44100, 16, 2));

    return 0;
}

int AacPcm::processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch)
{
    unsigned long bytesconsumed;
    unsigned long sr, ch;
    int k, result;
    int last_frame = 0;
    char *samplebuffer;

    svc_fileReader *reader = infos->getReader();
    if (!reader)
        return 0;

    // I assume that it lets me read from the beginning of the file here
    if (!init_called)
    {
        buffercount = 0;
        reader->read(buffer, 768*2);
        bytecount += 768*2;

        buffercount = faacDecInit(hDecoder, (unsigned char*)buffer, &sr, &ch);

        init_called = 1;
    }

    if (buffercount > 0)
    {
        bytecount += buffercount;

        for (k = 0; k < (768*2 - buffercount); k++)
            buffer[k] = buffer[k + buffercount];

        reader->read(buffer + (768*2) - buffercount, buffercount);
        buffercount = 0;
    }

    samplebuffer = new char[4096];

    result = faacDecDecode(hDecoder, (unsigned char*)buffer, &bytesconsumed, (short*)samplebuffer);
    if (result == FAAD_FATAL_ERROR) {
        last_frame = 1;
    }

    buffercount += bytesconsumed;
    bytecount += bytesconsumed;

    // getLength returns the size in shorts
    if (bytecount >= reader->getLength()*2)
        last_frame = 1;

    ChunkInfosI *ci = new ChunkInfosI();
    /*
    ci->addInfo("srate", samplerate);
    ci->addInfo("bps", bps);
    ci->addInfo("nch", nch);
    */
    ci->addInfo("srate", 44100);
    ci->addInfo("bps", 16);
    ci->addInfo("nch", 2);

    chunk_list->setChunk("PCM", samplebuffer, 4096, ci);

    if (last_frame)
        return 0;
    return 1;
}
