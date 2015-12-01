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

// the glue!

#include "assert.h"
#include "wac.h"

#ifdef WACLIENT_ICONSUPPORT
#include "../common/bitmap.h"
#include "../common/canvas.h"
#endif

extern WAComponentClient *the;

ComponentAPI *api;	// we do NOT delete this on shutdown!!!

WAComponentClient::WAComponentClient() {
  hInstance = NULL;
#ifdef WACLIENT_ICONSUPPORT
  tab_normal = NULL;
  tab_hilited = NULL;
  tab_selected = NULL;
#endif
}

int WAComponentClient::renderElement(int element, CanvasBase *cb, RECT *r, int alpha) {
#ifdef WACLIENT_ICONSUPPORT
  SkinBitmap *bmp = NULL;
  switch (element) {
    case WAC_RE_TAB_NORMAL: bmp = tab_normal; break;
    case WAC_RE_TAB_HOVER: bmp = tab_hilited; break;
    case WAC_RE_TAB_SELECTED: bmp = tab_selected; break;
  }
  if (bmp == NULL) return 0;

  // we have a valid bitmap, render it

  BaseCloneCanvas clone(cb);

  RECT pr = *r;
  pr.bottom = pr.top + bmp->getHeight();
  if (pr.bottom > r->bottom) pr.bottom = r->bottom;
  pr.top += (r->bottom - pr.bottom)/2;
  bmp->stretchToRectAlpha(&clone, &pr, alpha);

  return 1;
#else
  return 0;
#endif
}

void WAComponentClient::registerServices(ComponentAPI *_api) {
  api = _api;
  onRegisterServices();
}

void WAComponentClient::onDestroy() {
#ifdef WACLIENT_ICONSUPPORT
  delete tab_normal; tab_normal = NULL;
  delete tab_hilited; tab_hilited = NULL;
  delete tab_selected; tab_selected = NULL;
#endif
}

int WAComponentClient::getMetrics(int metricsid) {
  switch (metricsid) {
    case WAC_GM_SUGGESTED_WIDTH: return 275;
    case WAC_GM_SUGGESTED_HEIGHT: return 220;
    case WAC_GM_MAXIMUM_WIDTH: return -1; // no max
    case WAC_GM_MAXIMUM_HEIGHT: return -1; //no max
    case WAC_GM_MINIMUM_WIDTH: return 275;
    case WAC_GM_MINIMUM_HEIGHT: return 110;
    default: return -1;
  }
}

int WAComponentClient::onDbAllowOperation(GUID *whom, const char *playstring, int op, char *field, void *data, int data_len) {
  switch (op) {
    case DB_READ: 
    case DB_GETSCANNER:
      return DB_ALLOW;
    default: return DB_DENY;
  }
}

extern "C" {

__declspec(dllexport) UINT WAC_getVersion(void) {
  return WA_COMPONENT_VERSION;
}

__declspec(dllexport) int WAC_init(HINSTANCE dllInstance) {
  the->sethInstance(dllInstance);
  return TRUE;
}

__declspec(dllexport) WaComponent *WAC_enumComponent(int n) {
  return the;
}

} // end extern "C"


#define CBCLASS WAComponentClient
START_DISPATCH;
  CB(GETNAME, getName);
  CB(GETINFO, getInfo);
  CB(GETGUID, getGUID);
  VCB(REGISTERSERVICES, registerServices);
  CB(RENDERELEMENT, renderElement);
  VCB(ONCREATE, onCreate);
  VCB(ONDESTROY, onDestroy);
  CB(CREATEWINDOW, createWindow);
  CB(APPENDTOPOPUP, appendToPopup);
  CB(APPENDTOSYSTRAY, appendToSystray);
  CB(ONNOTIFY, onNotify);
  CB(ONCOMMAND, onCommand);
  CB(GETDRAGINTERFACE, getDragInterface);
  CB(GETCFGINTERFACE, getCfgInterface);
  VCB(SETHINSTANCE, sethInstance);
  CB(GETHINSTANCE, gethInstance);
END_DISPATCH;
#undef CBCLASS
