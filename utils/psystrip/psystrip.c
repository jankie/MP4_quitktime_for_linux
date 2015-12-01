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
 * $Id: psystrip.c,v 1.5 2001/12/21 12:58:17 menno Exp $
 */

/*
 * PSYSTRIP
 *  Strips Psytel tags from AAC files
 *
 */

#include <stdio.h>

#define HDR 0xABCCABCC

void main(int argc, char *argv[])
{
    FILE *inFile, *outFile;
    int fileSize, newFileSize;
    int offset, header;

    if (argc < 3) {
        fprintf(stderr, "USAGE: %s infile.aac outfile.aac\n", argv[0]);
        return;
    }

    inFile = fopen(argv[1], "rb");
    if (inFile == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return;
    }

    fseek(inFile, 0, SEEK_END);
    fileSize = ftell(inFile);

    fseek(inFile, -8, SEEK_END);

    fread(&offset, sizeof(int), 1, inFile);
    fread(&header, sizeof(int), 1, inFile);

    if(header != HDR) {
        fprintf(stderr, "No PsyTEL AAC tag found\n");
    } else {
        int i, c;

        fprintf(stderr, "PsyTEL AAC tag found\n");

        newFileSize = fileSize - offset - 8;

        outFile = fopen(argv[2], "wb");
        if (outFile == NULL) {
            fprintf(stderr, "Error opening output file\n");
            return;
        }

        fseek(inFile, 0, SEEK_SET);

        for (i = 0; i < newFileSize; i++)
        {
            c = fgetc(inFile);
            fputc(c, outFile);
        }

        fprintf(stderr, "PsyTEL AAC tag removed\n");

        fclose(outFile);
    }
    fclose(inFile);
}