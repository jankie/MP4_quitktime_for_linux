# Makefile for project faac


# Edit this path to let it point to the location of the source on your HDD
SRCDIR=d:\aac\cvsrep\faad

# Edit this path to let it point to the location of your LCC installation
LCCDIR=c:\lcc

# No editing needed after this point

CFLAGS=-I$(LCCDIR)\include -I$(SRCDIR)\include -I$(SRCDIR)\common\getopt  -O 
CC=$(LCCDIR)\bin\lcc.exe
LINKER=$(LCCDIR)\bin\lcclnk.exe
OBJS=\
	util.obj \
	transfo.obj \
	tns.obj \
	stereo.obj \
	pns.obj \
	nok_lt_prediction.obj \
	monopred.obj \
	intensity.obj \
	hufftables.obj \
	huffinit.obj \
	huffdec.obj \
	fastfft.obj \
	dolby_adapt.obj \
	decoder.obj \
	decdata.obj \
	config.obj \
	block.obj \
	bits.obj \
	wav.obj \
	faad.obj \
	getopt.obj

LIBS=

faad.exe:	$(OBJS) Makefile
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\faad.exe $(OBJS) $(LIBS)

# Build UTIL.C
UTIL_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

util.obj: $(UTIL_C) $(SRCDIR)\libfaad\util.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\util.c

# Build TRANSFO.C
TRANSFO_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\transfo.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\fastfft.h\
	$(SRCDIR)\libfaad\transfo.h\

transfo.obj: $(TRANSFO_C) $(SRCDIR)\libfaad\transfo.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\transfo.c

# Build TNS.C
TNS_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\util.h\

tns.obj: $(TNS_C) $(SRCDIR)\libfaad\tns.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\tns.c

# Build STEREO.C
STEREO_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

stereo.obj: $(STEREO_C) $(SRCDIR)\libfaad\stereo.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\stereo.c

# Build PNS.C
PNS_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

pns.obj: $(PNS_C) $(SRCDIR)\libfaad\pns.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\pns.c

# Build NOK_LT_PREDICTION.C
NOK_LT_PREDICTION_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\nok_ltp_common_internal.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\util.h\

nok_lt_prediction.obj: $(NOK_LT_PREDICTION_C) $(SRCDIR)\libfaad\nok_lt_prediction.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\nok_lt_prediction.c

# Build MONOPRED.C
MONOPRED_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\util.h\

monopred.obj: $(MONOPRED_C) $(SRCDIR)\libfaad\monopred.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\monopred.c

# Build INTENSITY.C
INTENSITY_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

intensity.obj: $(INTENSITY_C) $(SRCDIR)\libfaad\intensity.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\intensity.c

# Build HUFFTABLES.C
HUFFTABLES_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

hufftables.obj: $(HUFFTABLES_C) $(SRCDIR)\libfaad\hufftables.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\hufftables.c

# Build HUFFINIT.C
HUFFINIT_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

huffinit.obj: $(HUFFINIT_C) $(SRCDIR)\libfaad\huffinit.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\huffinit.c

# Build HUFFDEC.C
HUFFDEC_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\util.h\

huffdec.obj: $(HUFFDEC_C) $(SRCDIR)\libfaad\huffdec.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\huffdec.c

# Build FASTFFT.C
FASTFFT_C=\
	$(SRCDIR)\libfaad\fastfft.h\
	$(SRCDIR)\libfaad\transfo.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\all.h\

fastfft.obj: $(FASTFFT_C) $(SRCDIR)\libfaad\fastfft.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\fastfft.c

# Build DOLBY_ADAPT.C
DOLBY_ADAPT_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\util.h\

dolby_adapt.obj: $(DOLBY_ADAPT_C) $(SRCDIR)\libfaad\dolby_adapt.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\dolby_adapt.c

# Build DECODER.C
DECODER_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\transfo.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\util.h\

decoder.obj: $(DECODER_C) $(SRCDIR)\libfaad\decoder.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\decoder.c

# Build DECDATA.C
DECDATA_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\

decdata.obj: $(DECDATA_C) $(SRCDIR)\libfaad\decdata.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\decdata.c

# Build CONFIG.C
CONFIG_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\util.h\

config.obj: $(CONFIG_C) $(SRCDIR)\libfaad\config.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\config.c

# Build BLOCK.C
BLOCK_C=\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\interface.h\
	$(SRCDIR)\libfaad\tns.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\monopred.h\
	$(SRCDIR)\libfaad\bits.h\
	$(SRCDIR)\libfaad\nok_lt_prediction.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\nok_ltp_common.h\
	$(SRCDIR)\libfaad\port.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\block.h\
	$(SRCDIR)\libfaad\kbd_win.h\
	$(SRCDIR)\libfaad\transfo.h\
	$(SRCDIR)\libfaad\all.h\
	$(SRCDIR)\libfaad\util.h\

block.obj: $(BLOCK_C) $(SRCDIR)\libfaad\block.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\block.c

# Build BITS.C
BITS_C=\
	$(SRCDIR)\libfaad\bits.h\

bits.obj: $(BITS_C) $(SRCDIR)\libfaad\bits.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\libfaad\bits.c

# Build WAV.C
WAV_C=\
	$(SRCDIR)\frontend\wav.h\

wav.obj: $(WAV_C) $(SRCDIR)\frontend\wav.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\frontend\wav.c

# Build FAAD.C
FAAD_C=\
	$(SRCDIR)\common\getopt\getopt.h\
	$(SRCDIR)\include\faad.h\
	$(SRCDIR)\frontend\wav.h\

faad.obj: $(FAAD_C) $(SRCDIR)\frontend\faad.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\frontend\faad.c

# Build GETOPT.C
GETOPT_C=\
	$(SRCDIR)\common\getopt\getopt.h\

getopt.obj: $(GETOPT_C) $(SRCDIR)\common\getopt\getopt.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common\getopt\getopt.c

link:
	$(LINKER)  -s -subsystem console -o $(SRCDIR)\lcc\faad.exe $(OBJS) $(LIBS)

