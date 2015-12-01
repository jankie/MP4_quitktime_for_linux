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

#ifndef _SVC_FILEREAD_H
#define _SVC_FILEREAD_H

#include "../../common/dispatch.h"
#include "services.h"

namespace SvcFileReader {
  enum {
    READ=1,
    WRITE=2,
    APPEND=3,
  };
};

class svc_fileReader : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::FILEREADER; }

  int open(const char *filename, int mode=SvcFileReader::READ) { return _call(OPEN, 0, filename, mode); }
  int read(char *buffer, int length) { return _call(READ, 0, buffer,length); }
  int write(const char *buffer, int length) { return _call(WRITE, 0, buffer,length); }
  void close() { _voidcall(CLOSE); }

  void abort() { _voidcall(ABORT); }
  
  int getLength() { return _call(GETLENGTH, -1); }
  int getPos() { return _call(GETPOS, 0); }
  
  int canSeek() { return _call(CANSEEK, 0); }
  int seek(int position) { return _call(SEEK, 0, position); }

  int hasHeaders() { return _call(HASHEADERS,0); }
  const char *getHeader(const char *header) { return _call(GETHEADER,(const char *)NULL,header); }

  int exists(const char *filename) { return _call(EXISTS,-1,filename); }

  int remove(const char *filename) { return _call(REMOVE,0,filename); }

  enum {
    OPEN=10,
    READ=20,
    WRITE=30,
    CLOSE=40,
    ABORT=50,
    GETLENGTH=60,
    GETPOS=70,
    CANSEEK=80,
    SEEK=90,
    HASHEADERS=100,
    GETHEADER=110,
    EXISTS=120,
    REMOVE=130,
  };
};

// derive from this one
class svc_fileReaderI : public svc_fileReader {
public:
  virtual int open(const char *filename, int mode=SvcFileReader::READ)=0; // return 1 on success
  virtual int read(char *buffer, int length)=0; // return number of bytes read (if < length then eof)
  virtual int write(const char *buffer, int length)=0; // return number of bytes written
  virtual void close()=0;

  virtual void abort() { } // tells the reader to abort its current prebuffering/reader

  virtual int getLength() { return -1; } // return -1 on unknown/infinite
  virtual int getPos()=0;

  virtual int canSeek() { return 0; }
  virtual int seek(int position) { return 0; }

  virtual int hasHeaders() { return 0; }
  virtual const char *getHeader(const char *header) { return (const char *)NULL; }

  virtual int exists(const char *filename) { return 0; } // return 1 if true, 0 if not, -1 if unknown

  virtual int remove(const char *filename) { return 0; } // return 1 on success, 0 on error

protected:
  RECVS_DISPATCH;
};

#endif
