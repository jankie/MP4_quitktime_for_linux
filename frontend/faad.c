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
 * $Id: faad.c,v 1.27 2001/10/26 11:57:00 menno Exp $
 */

#ifdef _WIN32
    #define WIN32_MEAN_AND_LEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <faad.h>
#include "wav.h"

#ifndef min
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#endif

#define MAX_CHANNELS 8

/* globals */
char *progName;

int id3v2_tag(unsigned char *buffer)
{
    if (strncmp(buffer, "ID3", 3) == 0) {
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

char *get_filename(char *pPath)
{
    char *pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == '\\' || pPath[0] == ':') && pPath[1] && (pPath[1] != '\\'))
            pT = pPath + 1;
    }

    return pT;
}

void combine_path(char *path, char *dir, char *file)
{
    /* Should be a bit more sophisticated
       This code assumes that the path exists */

    /* Assume dir is allocated big enough */
#ifdef _WIN32
    if (dir[strlen(dir)-1] != '\\')
        strcat(dir, "\\");
#else
    if (dir[strlen(dir)-1] != '/')
        strcat(dir, "/");
#endif
    strcat(dir, get_filename(file));
    strcpy(path, dir);
}

void usage(void)
{
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr, "%s [options] infile.aac\n", progName);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -h    Shows this help screen.\n");
    fprintf(stderr, " -s X  Force the samplerate to X (for RAW files).\n");
    fprintf(stderr, " -o X  Sets the output directory to X.\n");
    fprintf(stderr, " -w    Write output to stdio instead of a file.\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "       faad -o aac infile.aac\n");
    return;
}


int main(int argc, char *argv[])
{
    int k;
    short sample_buffer[1024*MAX_CHANNELS];
    int writeToStdio = 0, out_dir_set = 0;
    int def_sr_set = 0;
    int def_srate;
    int showHelp = 0;
    char *fnp;
    int result;
    int first_time = 1;
    char out_dir[255];
    char aacFileName[255];
    char audioFileName[255];

    FILE *sndfile;
    FILE *infile;
    faacDecHandle hDecoder;
    faacDecConfigurationPtr config;

    char percent[200];

    long buffercount;
    unsigned long bytesconsumed, samples, fileread, bytecount;
    unsigned char *buffer;

    unsigned long samplerate, channels, tagsize;

/* System dependant types */
#ifdef _WIN32
    long begin, end;
#else
    clock_t begin;
#endif

    fprintf(stderr, "FAAD (Freeware AAC Decoder) Compiled on: " __DATE__ "\n");
    fprintf(stderr, "FAAD homepage: %s\n", "http://www.audiocoding.com");

    /* begin process command line */
    progName = argv[0];
    while (1) {
        int c = -1;
        int option_index = 0;
        static struct option long_options[] = {
            { "outputdir",  0, 0, 'o' },
            { "samplerate", 0, 0, 's' },
            { "stdio",      0, 0, 'w' },
            { "help",       0, 0, 'h' }
        };

        c = getopt_long(argc, argv, "o:s:wh",
            long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 'o': {
            if (optarg) {
                char dr[255];
                if (sscanf(optarg, "%s", dr) < 1) {
                    out_dir_set = 0;
                } else {
                    out_dir_set = 1;
                    strcpy(out_dir, dr);
                }
            } else {
                out_dir_set = 0;
            }
            break;
        }
        case 's': {
            if (optarg) {
                char dr[255];
                if (sscanf(optarg, "%s", dr) < 1) {
                    def_sr_set = 0;
                } else {
                    def_sr_set = 1;
                    def_srate = atoi(dr);
                }
            } else {
                out_dir_set = 0;
            }
            break;
        }
        case 'w': {
            writeToStdio = 1;
            break;
        }
        case 'h': {
            showHelp = 1;
            break;
        }
        default:
            break;
        }
    }

    /* check that we have at least two non-option arguments */
    /* Print help if requested */
    if (((argc - optind) < 1) || showHelp)
    {
        usage();
        return 1;
    }

    /* point to the specified file name */
    strcpy(aacFileName, argv[optind]);

#ifdef _WIN32
    begin = GetTickCount();
#else
    begin = clock();
#endif

    buffer = (unsigned char*)malloc(768*MAX_CHANNELS);
    memset(buffer, 0, 768*MAX_CHANNELS);

    infile = fopen(aacFileName, "rb");
    if (infile == NULL)
    {
        /* unable to open file */
        fprintf(stderr, "Error opening file: %s\n", aacFileName);
        return 1;
    }
    fseek(infile, 0, SEEK_END);
    fileread = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    buffercount = bytecount = 0;
    fread(buffer, 1, 768*MAX_CHANNELS, infile);

    tagsize = id3v2_tag(buffer);
    if (tagsize) {
        fseek(infile, tagsize, SEEK_SET);

        bytecount = tagsize;
        buffercount = 0;
        fread(buffer, 1, 768*MAX_CHANNELS, infile);
    }

    hDecoder = faacDecOpen();

    /* Set the default object type and samplerate */
    /* This is useful for RAW AAC files */
    config = faacDecGetCurrentConfiguration(hDecoder);
    if (def_sr_set)
        config->defSampleRate = def_srate;

    faacDecSetConfiguration(hDecoder, config);


    if((buffercount = faacDecInit(hDecoder, buffer, &samplerate, &channels)) < 0)
    {
        /* If some error initializing occured, skip the file */
        fprintf(stderr, "Error initializing decoder library.\n");
        return 1;
    }

    if (buffercount > 0) {
        bytecount += buffercount;

        for (k = 0; k < (768*MAX_CHANNELS - buffercount); k++)
            buffer[k] = buffer[k + buffercount];

        fread(buffer + (768*MAX_CHANNELS) - buffercount, 1, buffercount, infile);
        buffercount = 0;
    }

    fprintf(stderr, "Busy decoding %s", aacFileName);
    if (tagsize)
        fprintf(stderr, " (has ID3v2)\n");
    else
        fprintf(stderr, "\n");

    /* Only calculate the path and open the file for writing if we are not writing to stdout. */
    if(!writeToStdio)
    {
        if (out_dir_set)
            combine_path(audioFileName, out_dir, aacFileName);
        else
            strcpy(audioFileName, aacFileName);

        fnp = (char *)strrchr(audioFileName,'.');

        if (fnp)
            fnp[0] = '\0';

        strcat(audioFileName, ".wav");
    }

    first_time = 1;

    do
    {
        if (buffercount > 0) {
            for (k = 0; k < (768*MAX_CHANNELS - buffercount); k++)
                buffer[k] = buffer[k + buffercount];

            fread(buffer + (768*MAX_CHANNELS) - buffercount, 1, buffercount, infile);
            buffercount = 0;
        }

        result = faacDecDecode(hDecoder, buffer, &bytesconsumed, sample_buffer, &samples);
        if (result == FAAD_FATAL_ERROR)
            fprintf(stderr, "Fatal error decoding file\n");
        if (result == FAAD_ERROR)
            fprintf(stderr, "Error decoding frame\n");

        buffercount += bytesconsumed;

        bytecount += bytesconsumed;
        if (bytecount >= fileread)
            result = 9999; /* to make sure it stops now */

        sprintf(percent, "%.2f decoding %s.",
            min((double)(bytecount*100)/fileread,100), aacFileName);
        fprintf(stderr, "%s\r", percent);
#ifdef _WIN32
        SetConsoleTitle(percent);
#endif

        if ((result == FAAD_OK) && (samples > 0))
        {
            if(first_time)
            {
                first_time = 0;

                if(!writeToStdio)
                {
                    sndfile = fopen(audioFileName, "w+b");

                    if (sndfile==NULL)
                    {
                        fprintf(stderr, "Unable to create the file << %s >>.\n", audioFileName);
                        continue;
                    }

                    CreateWavHeader(sndfile, channels, samplerate, 16);
                }
                else
                {
                    sndfile = stdout;
                }
            }

            fwrite(sample_buffer, sizeof(short), 1024*channels, sndfile);
        }

    } while (result <= FAAD_OK_CHUPDATE);

    faacDecClose(hDecoder);

    fclose(infile);

    if(!writeToStdio)
        UpdateWavHeader(sndfile);

    /* Only close if we are not writing to stdout */
    if(!writeToStdio)
        if(sndfile)
            fclose(sndfile);

#ifdef _WIN32
    end = GetTickCount();
    fprintf(stderr, "Decoding %s took: %d sec.\n", aacFileName, (end-begin)/1000);
    SetConsoleTitle("FAAD");
#else
    /* clock() grabs time since the start of the app but when we decode multiple files,
       each file has its own starttime (begin). */
    fprintf(stderr, "Decoding %s took: %5.2f sec.\n", aacFileName,
        (float)(clock() - begin)/(float)CLOCKS_PER_SEC);
#endif

    if (buffer) free(buffer);

    return 0;
}
