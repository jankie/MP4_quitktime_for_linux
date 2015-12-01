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
 * $Id: adtsfix.c,v 1.4 2001/06/22 14:05:10 menno Exp $
 */

/*
 * ADTSFIX
 *  This utility repairs the ADTS headers AAC file
 *  What it does currently is:
 *  - Set ID to 0 (e.g. make the file MPEG4)
 *  - Set channel config to 2 (e.g. make it a stereo file)
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

int 
main ( int argc, char** argv )
{
    unsigned long  frames;
    unsigned char  buffer [768 * 2];
    unsigned int   frame_length;
    FILE*          infile;
    FILE*          outfile;

    if (argc != 3) {
        fprintf ( stderr, "USAGE: adtsfix infile outfile\n" );
        return 1;
    }

    if ( (infile = fopen(argv[1], "rb")) == NULL ) {
        fprintf ( stderr, "%s: Can't open '%s': %s\n", argv[0], argv[1], strerror(errno) );
        return 2;
    }
    
    if ( (outfile = fopen(argv[2], "wb")) == NULL ) {
        fprintf ( stderr, "%s: Can't create '%s': %s\n", argv[0], argv[2], strerror(errno) );
        return 3;
    }

    for ( frames = 0; fread (buffer, 1, 8, infile) == 8; frames++ ) {
                                                                /* Read in complete ADTS header, 58 bit */
        frame_length = (buffer[4] << 5) | (buffer[5] >> 3);     /* Get the framelength */

        buffer[1] &= ~8;                                        /* Set ID = 0 */
        buffer[2] += 0x03 << 6;                                 /* Set object type to LTP */
        buffer[3]  = (buffer[3] & 0x3F) | 0x80;                 /* Set channel config to 2 */

        fread  ( buffer+8, 1, frame_length-8, infile  );
        fwrite ( buffer  , 1, frame_length  , outfile );
    }

    fclose (infile);
    fclose (outfile);
    return 0;
}

/* end of adtsfix.c */
