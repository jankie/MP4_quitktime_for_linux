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

//NONPORTABLE - HWND
#ifndef _WAC_H
#define _WAC_H

#include "api.h"//CUT

#include "../common/compdb.h"//CUT

// if you comment this out you can skip linking skinbitmap.cpp
#define WACLIENT_ICONSUPPORT

#ifdef WACLIENT_NOICONSUPPORT
#undef WACLIENT_ICONSUPPORT
#endif

class CanvasBase;	// see ../common/canvas.h
class CfgItem;		// see ../attribs/cfgitem
class ComponentAPI;	// see api.h
class DragInterface;	// see ../common/drag.h
class SkinBitmap;	// see ../common/bitmap.h
class RootWnd;		// see ../common/rootwnd.h
class PlayItem;		// see playitem.h

// this tells wasabi.exe what version of the SDK we're compiled with
// this number will be incremented every once in a while, you can mostly
// ignore it in general
#define WA_COMPONENT_VERSION	1

#define DB_DENY  0
#define DB_ALLOW 1

// commands for onNotify below

#define WAC_NOTIFY_NOP			0

#define WAC_NOTIFY_ACTIVATE		10
#define WAC_NOTIFY_DEACTIVATE		20

#define WAC_NOTIFY_FULLSCREEN		30
#define WAC_NOTIFY_RESTORE		40

#define WAC_NOTIFY_ENTERRESIZE		50
#define WAC_NOTIFY_LEAVERESIZE		51

#define WAC_NOTIFY_SYSTRAYDONE		60
#define WAC_NOTIFY_POPUPDONE  		61

#define WAC_NOTIFY_BASETEXTUREWINDOW	78

#define WAC_NOTIFY_MENU_COMMAND		10000

// this message is sent when service runlevel notifies are sent
// param1 = msg
// param2 = param1
// param3 = param2
#define WAC_NOTIFY_SERVICE_NOTIFY	100

#define WAC_NOTIFY_COMPONENT_SHOW 210
#define WAC_NOTIFY_COMPONENT_HIDE 220
#define WAC_NOTIFY_LINK_RATIO 230 // links embedwnd component container's ratio with component ratio

#define WAC_NOTIFY_SKINELEMENTSLOADED	300
#define WAC_NOTIFY_SKINGUILOADED	    301
#define WAC_NOTIFY_SKINLOADED	        302

// codes for getInfo

enum {
  WAC_GI_NOP=0,
  WAC_GI_SHORTDESC,
  WAC_GI_LONGDESC,
};

// codes for getMetrics

enum {
  WAC_GM_SUGGESTED_WIDTH=0,
  WAC_GM_SUGGESTED_HEIGHT,
  WAC_GM_MINIMUM_WIDTH,
  WAC_GM_MINIMUM_HEIGHT,
  WAC_GM_MAXIMUM_WIDTH,
  WAC_GM_MAXIMUM_HEIGHT,
};

// -----------

// codes for renderElement
enum {
  WAC_RE_NOP=0,
  WAC_RE_TAB_NORMAL,
  WAC_RE_TAB_SELECTED,
  WAC_RE_TAB_HOVER,
};

#include "../common/dispatch.h"

class WaComponent : public Dispatchable {
public:
  // provide your component name & other info here
  const char *getName() { return _call(GETNAME, (char *)NULL); }
  const char *getInfo(int strnum=0) { return _call(GETINFO, "", strnum); }
  virtual int getMetrics(int metricid)=0;
  GUID getGUID() { return _call(GETGUID, INVALID_GUID); }

  void registerServices(ComponentAPI *a) { _voidcall(REGISTERSERVICES, a); }

  // override this one to hide your component from the component bar
  virtual int getDisplayComponent() { return TRUE; };
  int renderElement(int element, CanvasBase *cb, RECT *r, int alpha) {
    return _call(RENDERELEMENT, 0, element, cb, r, alpha);
  }

  void onCreate() { _voidcall(ONCREATE); }
  void onDestroy() { _voidcall(ONDESTROY); }
  virtual void onWindowLink(HWND hWnd) {} // here is your parent window
  virtual void onResize(int x, int y, int w, int h) {} // resize to your par
  RootWnd *createWindow(int n, RootWnd *parentWnd) { return _call(CREATEWINDOW, (RootWnd*)NULL, n, parentWnd); }

  int appendToPopup(void *item, const char *datatype, LPARAM lParam, bool you) {
    return _call(APPENDTOPOPUP, 0, item, datatype, lParam, you);
  }
  int appendToSystray(LPARAM lParam, BOOL right) {
    return _call(APPENDTOSYSTRAY, 0, lParam, right);
  }
  int onNotify(int cmd, int param1=0, int param2=0, int param3=0, int param4=0){
    return _call(ONNOTIFY, 0, cmd, param1, param2, param3, param4);
  }
  // everything after cmd is for future expansion
  int onCommand(const char *cmd, int param1, int param2, void *ptr, int ptrlen){
    return _call(ONCOMMAND, 0, cmd, param1, param2, ptr, ptrlen);
  }
  // this will be used if someone drops something on your thinger icon
  // or if another component wants to transfer pointers to you
  DragInterface *getDragInterface() {
    return _call(GETDRAGINTERFACE, (DragInterface*)NULL);
  }

  // db callbacks
  virtual int onDbAllowOperation(GUID *whom, const char *playstring, int op, char *field, void *data, int data_len)=0;
  virtual int onDbEnumFields(int fieldN, char *field_name, int field_name_max, int *data_type, BOOL *indexed, BOOL *unique_indexed) { return 0; }

  // config ptr... you can either derive from CfgItemI & return a pointer
  // to yourself or instantiate a new CfgItemI & use that
  CfgItem *getCfgInterface(int n) {
    return _call(GETCFGINTERFACE, (CfgItem*)NULL, n);
  }

  // saves the HINSTANCE of your DLL for you	(NONPORTABLE)
  void sethInstance(HINSTANCE hInst) {
    _voidcall(SETHINSTANCE, hInst);
  }
  HINSTANCE gethInstance() {
    return _call(GETHINSTANCE, (HINSTANCE)0);
  }

  enum {
    GETNAME=1,
    GETINFO,
    GETGUID,
    REGISTERSERVICES,
    RENDERELEMENT,
    ONCREATE,
    ONDESTROY,
    CREATEWINDOW,
    APPENDTOPOPUP,
    APPENDTOSYSTRAY,
    ONNOTIFY,
    ONCOMMAND,
    GETDRAGINTERFACE,
    GETCFGINTERFACE,
    SETHINSTANCE,
    GETHINSTANCE,
#if 0
GETDISPLAYCOMPONENT,
GETITEMMANAGER,
ONWINDOWLINK,
ONRESIZE,
#endif
  };
};

class WAComponentClient : public WaComponent {
public:
  WAComponentClient();
  virtual ~WAComponentClient() {}

  // provide your component name & other info here
  virtual const char *getName()=0;
  virtual const char *getInfo(int strnum=0) { return NULL; }
  virtual int getMetrics(int metricid);
  virtual GUID getGUID()=0;	// be sure to override this one

  void registerServices(ComponentAPI *);	// don't override

  // override this one to hide your component from the component bar
  virtual int getDisplayComponent() { return TRUE; };
  virtual int renderElement(int element, CanvasBase *cb, RECT *r, int alpha);

  // override these to receive notifications. call down if you override
  virtual void onRegisterServices() {}
  virtual void onCreate() {}	// init stuff
  virtual void onDestroy();		// destroy everything here, not in ~
  virtual void onWindowLink(HWND hWnd) {} // here is your parent window
  virtual void onResize(int x, int y, int w, int h) {} // resize to your par
  virtual RootWnd *createWindow(int n, RootWnd *parentWnd) { return NULL; }

  virtual int appendToPopup(void *item, const char *datatype, LPARAM lParam, bool you) { return 0; }
  virtual int appendToSystray(LPARAM lParam, BOOL right) { return 0; }
  virtual int onNotify(int cmd, int param1, int param2, int param3, int param4) { return 0; }
  // everything after cmd is for future expansion
  virtual int onCommand(const char *cmd, int param1, int param2, void *ptr, int ptrlen) { return 0; }
  // this will be used if someone drops something on your thinger icon
  // or if another component wants to transfer pointers to you
  virtual DragInterface *getDragInterface() { return NULL; };

  // db callbacks
  virtual int onDbAllowOperation(GUID *whom, const char *playstring, int op, char *field, void *data, int data_len);
  virtual int onDbEnumFields(int fieldN, char *field_name, int field_name_max, int *data_type, BOOL *indexed, BOOL *unique_indexed) { return 0; }

  // config ptr... you can either derive from CfgItemI & return a pointer
  // to yourself or instantiate a new CfgItemI & use that
  virtual CfgItem *getCfgInterface(int n) { return NULL; }

  // saves the HINSTANCE of your DLL for you	(NONPORTABLE)
  void sethInstance(HINSTANCE hInst) { hInstance = hInst; };
  HINSTANCE gethInstance() { return hInstance; };

protected:
#ifdef WACLIENT_ICONSUPPORT
  // if you fill these in, renderElement will use them
  // they will be auto-deleted on shutdown if they are not NULL
  SkinBitmap *tab_normal, *tab_hilited, *tab_selected;
#endif

  RECVS_DISPATCH;

private:
  HINSTANCE hInstance;
};

extern "C" {
  typedef void (*WACINIT)(HINSTANCE);
  typedef UINT (*WACGETVERSION)(void); // returns the interface version component was compiled with
  typedef WaComponent *(*WACENUMCOMPONENT)(int n);
};

#endif
