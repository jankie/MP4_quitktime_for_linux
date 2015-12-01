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
 * $Id: psystrip.c,v 1.4 2001/06/22 14:05:10 menno Exp $
 */

/*
 * psystrip: Psytel AAC encoder stripper
 *
 *   Strips Psytel tags from AAC files which makes the file non standard conform. 
 *   They are at the end of the file and causing a
 *
 *     C:\> faad Hex.aac Hex.wav
 *     FAAD (Freeware AAC Decoder) Compiled on: Jun  9 2001
 *     FAAD homepage: http://www.audiocoding.com
 *     Busy decoding Hex.aac
 *     Fatal error decoding file
 *     Decoding Hex.aac took:  0.23 sec.
 *     C:\> _
 *
 *   after a successful decoding.
 *
 *   You can use it with two arguments, then the program works like a
 *   copy-and-modify tool, you still have the original source. Or you can
 *   use it with one argument, then it works in-place modifying the orignal
 *   source file.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#define TAG_HEADER   0xABCCABCC


int 
main ( int argc, char** argv )
{
    FILE*          inFile;
    FILE*          outFile;
    long           fileSize;
    unsigned long  offset;
    unsigned long  header;
    size_t         len;
    static char    buffer [24576];   /* can you increase if you have plenty of RAM, use RAM size/32 */

    /* usage if not the correct number of arguments */
    if ( argc != 2   &&  argc != 3 ) {
        fprintf ( stderr, "usage: %s infile.aac outfile.aac  converts infile.aac to outfile.aac\n"
                          "       %s inoutfile.aac           converts in-place\n", argv[0], argv[0] );
        return 1;
    }

    /* open input file */
    if ( (inFile = fopen (argv[1], "rb")) == NULL ) {
        fprintf ( stderr, "Error opening input file '%s'; %s\n", argv[1], strerror(errno) );
        return 2;
    }

    /* get filesize-8 and read the last 8 byte of the file as two 32 bit little endian numbers */
    fseek ( inFile, -8L, SEEK_END );
    fileSize = ftell (inFile);

    offset  = getc (inFile);
    offset += (unsigned long) getc (inFile) <<  8;
    offset += (unsigned long) getc (inFile) << 16;
    offset += (unsigned long) getc (inFile) << 24;

    header  = getc (inFile);
    header += (unsigned long) getc (inFile) <<  8;
    header += (unsigned long) getc (inFile) << 16;
    header += (unsigned long) getc (inFile) << 24;

    /* get back to the start of the file */
    rewind ( inFile );

    /* is there a Header tag? */
    if ( header != TAG_HEADER ) {
        fprintf ( stderr, "No PsyTEL AAC tag found.\n" );
    } 
    else {
        fprintf ( stderr, "PsyTEL AAC tag found.\n" );

        /* calculate new file size */
        fileSize -= offset;

        if ( argc == 2 ) {
	
            /* truncate in place via ftruncate */
            fclose ( inFile );
            if ( (inFile = fopen ( argv[1], "r+b" )) == NULL ) {
                fprintf ( stderr, "Error modifying file '%s': %s\n", argv[1], strerror(errno) );
                return 3;
            }
            if (ftruncate ( fileno(inFile), fileSize ) != 0) {
	        fprintf ( stderr, "Error truncating file '%s': %s\n", argv[1], strerror(errno) );
		return 4;
	    }
        }
        else {
	
            /* truncate by copying not all bytes */
            if ( (outFile = fopen ( argv[2], "wb" )) == NULL ) {
                fprintf ( stderr, "Error opening output file '%s': %s\n", argv[2], strerror(errno) );
                return 3;
            }

            /* fast copy using a buffer */
            while ( fileSize > 0 ) {
                len = fread  ( buffer, 1, fileSize < sizeof(buffer) ? fileSize : sizeof(buffer), inFile );
                len = fwrite ( buffer, 1, len, outFile );
                if ( len == 0 ) {
    	            fprintf ( stderr, "Error copying file '%s' to '%s': %s\n", argv[1], argv[2], strerror(errno) );
		    return 4;
	        }
                fileSize -= len;
            }
            fclose ( outFile );
        }

        fprintf ( stderr, "PsyTEL AAC tag removed\n" );
    }
    
    fclose ( inFile );
    return 0;
}

/* end of psystrip.c */
