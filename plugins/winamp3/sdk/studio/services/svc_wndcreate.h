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

#ifndef _SVC_WNDCREATE_H
#define _SVC_WNDCREATE_H

#include "../../common/dispatch.h"
#include "services.h"

class RootWnd;

namespace WindowTypes {
  enum {
    NONE=0,
    BUCKETITEM=100,	
  };
};


class svc_windowCreate : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::WINDOWCREATE; }

  int testGuid(GUID g);
  RootWnd *createWindowByGuid(GUID g, RootWnd *parent);
  RootWnd *createWindowOfType(int windowtype, int n, RootWnd *parent);
  int destroyWindow(RootWnd *w);

  enum {
    TESTGUID            =10,
    CREATEWINDOWBYGUID  =20,
    CREATEWINDOWOFTYPE  =30,
    DESTROYWINDOW       =40,
  };
};

inline int svc_windowCreate::testGuid(GUID g) {
  return _call(TESTGUID, 0, g);
}

inline RootWnd *svc_windowCreate::createWindowByGuid(GUID g, RootWnd *parent) {
  return _call(CREATEWINDOWBYGUID, (RootWnd*)0, g, parent);
}

inline int svc_windowCreate::destroyWindow(RootWnd *w) {
  return _call(DESTROYWINDOW, 0, w);
}

inline RootWnd *svc_windowCreate::createWindowOfType(int windowtype, int n, RootWnd *parent) {
  return _call(CREATEWINDOWOFTYPE, (RootWnd*)0, windowtype, n, parent);
}

class svc_windowCreateI : public svc_windowCreate {
public:
  virtual int testGuid(GUID g)=0;
  virtual RootWnd *createWindowByGuid(GUID g, RootWnd *parent)=0;
  virtual RootWnd *createWindowOfType(int windowtype, int n, RootWnd *parent)=0;
  virtual int destroyWindow(RootWnd *w)=0;

protected:
  RECVS_DISPATCH;
};

#endif
