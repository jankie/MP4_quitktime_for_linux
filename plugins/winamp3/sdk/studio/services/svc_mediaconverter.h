/*

  Nullsoft WASABI Source File License

  Copyright 1999-2001 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


  Brennan Underwood
  brennan@nullsoft.com

*/

#ifndef _SVC_MEDIACONVERTER_H
#define _SVC_MEDIACONVERTER_H

#include "../../common/dispatch.h"
#include "services.h"
#include "../chunklist.h"
#include "../mediainfo.h"
#include "../corecb.h"

// do NOT derive from these ones
class svc_mediaConverter : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::MEDIACONVERTER; }

  // test if the converter can handle that stream, filename or chunktype
  
  // if your converter can accept streams depending on the data stream you should
  // test the file thru the reader interface (like to test if the file header is ok)
  // if your converter can accept files thru a test on the filename string, you should
  // test the name string (like tone://... or *.wav)
  // if your converter can accept depending on the chunktype, you should test the chunktype
  // (like MP3, PCM, etc...)
  // returns 1 if ok
  int canConvertFrom(svc_fileReader *reader, const char *name, const char *chunktype) { return _call(CANCONVERTFROM,0,reader,name,chunktype); }

  // returns the chunk type that the converter will convert to
  // (PCM, MP3, etc...)
  const char *getConverterTo() { return _call(GETCONVERTERTO,""); }

  // fill up the infos class
  int getInfos(MediaInfo *infos) { return _call(GETINFOS,0,infos); }

  // process current file data
  // returns 1 if ok, 0 when file/stream has ended
  int processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch) { return _call(PROCESSDATA,0,infos,chunk_list,killswitch); }

  // returns the current position in ms
  // usually the position is automatically calculated with the amount of PCM data converters send back to the core
  // so you don't have to override this function. however, if you want to force the position, return a value other than -1
  int getPosition(void) { return _call(GETPOSITION,-1); }

  // returns the latency of the converter in ms
  int getLatency(void) { return _call(GETLATENCY,0); }

  // internally used by wasabi
  CoreCallback *getCoreCallback(void) { return _call(GETCORECALLBACK,(CoreCallback *)0); }

  enum {
    CANCONVERTFROM=10,
    GETCONVERTERTO=20,
    GETINFOS=30,
    PROCESSDATA=40,
    GETPOSITION=50,
    GETLATENCY=60,
    GETCORECALLBACK=70,
  };
};

class svc_mediaConverterNI : public svc_mediaConverter {
public:
  virtual int canConvertFrom(svc_fileReader *reader, const char *name, const char *chunktype)=0;
  virtual const char *getConverterTo()=0;

  virtual int getInfos(MediaInfo *infos)=0;

	virtual int processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch)=0;

  virtual int getPosition(void) { return -1; }

  virtual int getLatency(void) { return 0; }

  virtual CoreCallback *getCoreCallback(void)=0;

protected:
  RECVS_DISPATCH;
};

// derive from this one
class svc_mediaConverterI : public svc_mediaConverterNI, public CoreCallbackI {
public:
  virtual int canConvertFrom(svc_fileReader *reader, const char *name, const char *chunktype)=0;
  virtual const char *getConverterTo()=0;

  virtual int getInfos(MediaInfo *infos)=0;

	virtual int processData(MediaInfo *infos, ChunkList *chunk_list, bool *killswitch)=0;

  virtual int getPosition(void) { return -1; }

  virtual int getLatency(void) { return 0; }

  virtual CoreCallback *getCoreCallback(void) { return this; }
};

#endif
