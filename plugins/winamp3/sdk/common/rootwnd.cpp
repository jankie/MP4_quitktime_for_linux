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


#include "rootwnd.h"
#include "canvas.h"

#define CBCLASS RootWndI
START_DISPATCH;
  CB(GETHWND, gethWnd);
  CB(GETDRAGINTERFACE, getDragInterface);
  CB(FROMPOINT, rootWndFromPoint);
  VCB(GETWINDOWRECT, getWindowRect);
  CB(ISVISIBLE, isVisible);
  CB(GETSKINID, getSkinId);
  CB(ONMETRICCHANGE, onMetricChange);
  CB(ONPAINT, rootwnd_onPaint);
  CB(PAINTTREE, rootwnd_paintTree);
  CB(GETPARENT, getParent);
  CB(GETROOTPARENT, getRootParent);
  VCB(SETPARENT, setParent);
  CB(ONDEFERREDCALLBACK, onDeferredCallback);
  CB(CHILDNOTIFY, childNotify);
  CB(GETPREFERENCES, getPreferences);
  CB(BEGINCAPTURE, beginCapture);
  CB(ENDCAPTURE, endCapture);
  CB(GETCAPTURE, getCapture);
  CB(CLICKTHROUGH, isClickThrough);
  CB(INIT, init);
  CB(GETCURSORTYPE, getCursorType);
  VCB(GETCLIENTRECT, getClientRect);
  VCB(GETNONCLIENTRECT, getNonClientRect);
  VCB(SETVISIBLE, setVisible);
  VCB(INVALIDATE, invalidate);
  VCB(INVALIDATERECT, invalidateRect);
  VCB(INVALIDATERGN, invalidateRgn);
  VCB(INVALIDATEFROM, invalidateFrom);
  VCB(INVALIDATERECTFROM, invalidateRectFrom);
  VCB(INVALIDATERGNFROM, invalidateRgnFrom);
  VCB(VALIDATE, validate);
  VCB(VALIDATERECT, validateRect);
  VCB(VALIDATERGN, validateRgn);
  CB(ONSIBINVALIDATE, onSiblingInvalidateRgn);
  CB(WANTSIBINVALIDATE, wantSiblingInvalidations);
  CB(CASCADEREPAINTFROM, cascadeRepaintFrom);
  CB(CASCADEREPAINTRGNFROM, cascadeRepaintRgnFrom);
  CB(CASCADEREPAINTRECTFROM, cascadeRepaintRectFrom);
  CB(CASCADEREPAINT, cascadeRepaint);
  CB(CASCADEREPAINTRGN, cascadeRepaintRgn);
  CB(CASCADEREPAINTRECT, cascadeRepaintRect);
  VCB(REPAINT, repaint);
  CB(GETTEXTUREWND, getBaseTextureWindow);
  CB(ONACTIVATE, onActivate);
  CB(ONDEACTIVATE, onDeactivate);
END_DISPATCH;
