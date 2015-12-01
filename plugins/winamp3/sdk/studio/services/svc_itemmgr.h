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

#ifndef _SVC_ITEMMGR_H
#define _SVC_ITEMMGR_H

#include "../../common/dispatch.h"
#include "services.h"

class svc_itemMgr : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::ITEMMANAGER; }

  int isMine(const char *playstring) { return _call(ISMINE, 0, playstring); }

  int optimizePlaystring(char *playstring) {
    return _call(OPTIMIZEPLAYSTRING, 0, playstring);
  }
  int createInitialName(const char *playstring, char *buf, int buflen) {
    return _call(CREATEINITIALNAME, 0, playstring, buf, buflen);
  }

  int onDatabaseAdd(const char *playstring) {
    return _call(ONDATABASEADD, 0, playstring);
  }
  int onDatabaseDel(const char *playstring) {
    return _call(ONDATABASEDEL, 0, playstring);
  }

  //return 1 if changed
  int onTitleChange(const char *playstring, const char *newtitle) {
    return _call(ONTITLECHANGE, 0, playstring, newtitle);
  }
  int onTitle2Change(const char *playstring, const char *newtitle) {
    return _call(ONTITLE2CHANGE, 0, playstring, newtitle);
  }

  void onNextFile(const char *playstring) {
    _voidcall(ONNEXTFILE, playstring);
  }

  void onFileComplete(const char *playstring) {
    _voidcall(ONFILECOMPLETE, playstring);
  }

  int wantScanData(const char *playstring) {
    return _call(WANTSCANDATA, 1, playstring);
  }

  enum {
    ISMINE=100,
    OPTIMIZEPLAYSTRING=200,
    CREATEINITIALNAME=300,
    ONDATABASEADD=400,
    ONDATABASEDEL=401,
    ONTITLECHANGE=600,
    ONTITLE2CHANGE=601,
    ONNEXTFILE=700,
    ONFILECOMPLETE=800,
    WANTSCANDATA=900,
  };
};

// derive from this one
class svc_itemMgrI : public svc_itemMgr {
public:
  virtual int isMine(const char *playstring)=0;
  virtual int optimizePlaystring(char *playstring) { return 0; }
  virtual int createInitialName(const char *playstring, char *buf, int buflen) { return 0; }
  virtual int onDatabaseAdd(const char *playstring) { return 0; }
  virtual int onDatabaseDel(const char *playstring) { return 0; }
  virtual int onTitleChange(const char *playstring, const char *newtitle) { return 0; }
  virtual int onTitle2Change(const char *playstring, const char *newtitle) { return 0; }
  virtual void onNextFile(const char *playstring) { }
  virtual void onFileComplete(const char *playstring) { }

  virtual int wantScanData(const char *playstring) { return 1; }

protected:
  RECVS_DISPATCH;
};

#endif
