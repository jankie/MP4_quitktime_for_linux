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
 * $Id: wav.h,v 1.4 2001/06/08 18:00:27 menno Exp $
 */

#ifndef _WAV_H
#define _WAV_H

typedef struct {
    unsigned short  format_tag;
    unsigned short  channels;           /* 1 = mono, 2 = stereo */
    unsigned long   samplerate;         /* typically: 44100, 32000, 22050, 11025 or 8000*/
    unsigned long   bytes_per_second;   /* SamplesPerSec * BlockAlign*/
    unsigned short  blockalign;         /* Channels * (BitsPerSample / 8)*/
    unsigned short  bits_per_sample;    /* 16 or 8 */
} WAVEAUDIOFORMAT;

typedef struct {
    char info[4];
    unsigned long length;
} RIFF_CHUNK;

int CreateWavHeader(FILE *file, int channels, int samplerate, int resolution);
int UpdateWavHeader(FILE *file);

#endif
