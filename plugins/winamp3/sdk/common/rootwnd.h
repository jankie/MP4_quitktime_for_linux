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

#ifndef _ROOTWND_H
#define _ROOTWND_H

#include "std.h"

#include "dispatch.h"

class CanvasBase;
class Region;

// only these methods are safe across dll boundaries
// this is the pointer you find in the GWL_USERDATA of a window

class DragInterface;

enum WndPreferences {
  SUGGESTED_X,
  SUGGESTED_Y,
  SUGGESTED_W,
  SUGGESTED_H,
};

class COMEXP NOVTABLE RootWnd : public Dispatchable {
protected:
  RootWnd() {}	// protect constructor
public:
#ifdef WIN32

  // this passes thru to the windows WndProc, if there is one -- NONPORTABLE
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)=0;

  // get the HWND (if there is one) -- NONPORTABLE
  HWND gethWnd();
#endif

  int init(RootWnd *parent, int nochild=FALSE);
  int isInited();	// are we post init() ? USE THIS INSTEAD OF gethWnd()==1

  // fetch the DragInterface of the RootWnd here, can be NULL
  DragInterface *getDragInterface();

  int getCursorType(int x, int y);

  // returns deepest child for point or yourself if no child there
  RootWnd *rootWndFromPoint(POINT *pt);

  void getClientRect(RECT *);
  void getNonClientRect(RECT *rect);
  // the onscreen coordinates
  void getWindowRect(RECT *r);

  void setVisible(int show);
  int isVisible();
  int getSkinId();

  int onMetricChange(int metricid, int param1=0, int param2=0);

  // painting stuff
  void invalidate();
  void invalidateRect(RECT *r);
  void invalidateRgn(Region *r);
  void invalidateFrom(RootWnd *who);
  void invalidateRectFrom(RECT *r, RootWnd *who);
  void invalidateRgnFrom(Region *r, RootWnd *who);
  void validate();
  void validateRect(RECT *r);
  void validateRgn(Region *reg);
  int onPaint(CanvasBase *canvas, Region *r);
  int paintTree(CanvasBase *canvas, Region *r);
  RootWnd *getParent();
  RootWnd *getRootParent();
  void setParent(RootWnd *newparent);
  int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx);
  int wantSiblingInvalidations();
  int cascadeRepaintFrom(RootWnd *who);
  int cascadeRepaintRgnFrom(Region *reg, RootWnd *who);
  int cascadeRepaintRectFrom(RECT *r, RootWnd *who);
  int cascadeRepaint();
  int cascadeRepaintRgn(Region *reg);
  int cascadeRepaintRect(RECT *r);
  void repaint();
  RootWnd *getBaseTextureWindow();
  int onDeferredCallback(int param1, int param2);
  int childNotify(RootWnd *child, int msg, int p1, int p2);
  int getPreferences(int what);
  virtual void setStartHidden(int sh)=0;

  virtual double getRenderRatio()=0;
  virtual void setRenderRatio(double r)=0;
  virtual void resize(int x, int y, int w, int h)=0;
  inline void resizeToRect(RECT *r) {
    resize(r->left, r->top, r->right - r->left, r->bottom - r->top);
  }
  virtual void move(int x, int y)=0;

  // virtual child stuff
  virtual void addVirtualChild(RootWnd *child)=0;
  virtual void removeVirtualChild(RootWnd *child)=0;
  virtual int isVirtual()=0;//FUCKO KILLME
  virtual RootWnd *getVirtualChild(int x, int y)=0;
  virtual RootWnd *getVirtualChild(int _enum)=0;
  virtual int getNumVirtuals()=0;
  virtual void bringVirtualToFront(RootWnd *w)=0;
  virtual void bringVirtualToBack(RootWnd *w)=0;
  virtual void bringVirtualAbove(RootWnd *w, RootWnd *b)=0;
  virtual void bringVirtualBelow(RootWnd *w, RootWnd *b)=0;

  virtual int beginCapture();
  virtual int endCapture();
  virtual int getCapture();	// returns 1 if this window has mouse/keyb captured
  virtual void setVirtualChildCapture(RootWnd *child)=0;
  virtual RootWnd *getVirtualChildCapture()=0;
  virtual int setVirtualChildTimer(RootWnd *child, int id, int ms)=0;
  virtual int killVirtualChildTimer(RootWnd *child, int id)=0;
  virtual void timerCallback(int id)=0;

  virtual BOOL mouseInRegion(int x, int y)=0;
  virtual int checkDoubleClick(int button, int x, int y)=0;
  virtual int onLeftButtonDblClk(int x, int y)=0;
  virtual int onRightButtonDblClk(int x, int y)=0;
  virtual int onLeftButtonUp(int x, int y)=0;
  virtual int onRightButtonUp(int x, int y)=0;
  virtual int onLeftButtonDown(int x, int y)=0;
  virtual int onRightButtonDown(int x, int y)=0;
  virtual int onMouseMove(int x, int y)=0;
  virtual int isClickThrough();

  virtual void clientToScreen(int *x, int *y); // so rootWndFromPoint can map ratio
  virtual void screenToClient(int *x, int *y); // ..		

  virtual int onActivate();
  virtual int onDeactivate();

  enum {
    GETHWND		= 100,
    GETDRAGINTERFACE	= 200,
    FROMPOINT		= 300,
    GETWINDOWRECT	= 400,
    ISVISIBLE		= 500,
    GETSKINID		= 600,
    ONMETRICCHANGE	= 700,
    ONPAINT		= 800,
    PAINTTREE = 900,
    GETPARENT = 1000,
    GETROOTPARENT = 1001,
    SETPARENT = 1002,
    ONDEFERREDCALLBACK = 1100,
    CHILDNOTIFY = 1200,
    GETPREFERENCES = 1300,
    BEGINCAPTURE = 1400,
    ENDCAPTURE = 1401,
    GETCAPTURE = 1402,
    CLICKTHROUGH = 1500,
    CLIENTSCREEN = 1600,
    SCREENCLIENT = 1601,
    INIT = 1700,
    GETCURSORTYPE = 1800,
    GETCLIENTRECT = 1900,
    GETNONCLIENTRECT = 1901,
    SETVISIBLE = 2000,
    INVALIDATE = 2100,
    INVALIDATERECT = 2101,
    INVALIDATERGN = 2102,
    INVALIDATEFROM = 2103,
    INVALIDATERECTFROM = 2104,
    INVALIDATERGNFROM = 2105,
    VALIDATE = 2200,
    VALIDATERECT = 2201,
    VALIDATERGN = 2202,
    ONSIBINVALIDATE = 2300,
    WANTSIBINVALIDATE = 2301,
    CASCADEREPAINTFROM = 2400,
    CASCADEREPAINTRECTFROM = 2401,
    CASCADEREPAINTRGNFROM = 2402,
    CASCADEREPAINT = 2403, 
    CASCADEREPAINTRECT = 2405,
    CASCADEREPAINTRGN = 2406,
    REPAINT = 2500,
    GETTEXTUREWND = 2600,
    ONACTIVATE = 2700,
    ONDEACTIVATE = 2800,
  };
};

// inline helper functions definitions
#ifdef WIN32
inline HWND RootWnd::gethWnd() {
  return _call(GETHWND, (HWND)NULL);
}
#endif
inline DragInterface *RootWnd::getDragInterface() {
  return _call(GETDRAGINTERFACE, (DragInterface*)0);
}

inline RootWnd *RootWnd::rootWndFromPoint(POINT *pt) {
  return _call(FROMPOINT, (RootWnd*)0, pt);
}

inline void RootWnd::getWindowRect(RECT *r) {
  if (!_voidcall(GETWINDOWRECT, r)) Std::setRect(r, 0, 0, 0, 0);
}

inline int RootWnd::isVisible() {
  return _call(ISVISIBLE, 0);
}

inline int RootWnd::getSkinId() {
  return _call(GETSKINID, 0);
}

inline int RootWnd::onMetricChange(int metricid, int param1, int param2) {
  return _call(ONMETRICCHANGE, 0, metricid, param1, param2);
}

inline int RootWnd::onPaint(CanvasBase *canvas, Region *r) {
  return _call(ONPAINT, 0, canvas, r);
}

inline int RootWnd::paintTree(CanvasBase *canvas, Region *r) {
  return _call(PAINTTREE, 0, canvas, r);
}

inline RootWnd *RootWnd::getParent() {
  return _call(GETPARENT, (RootWnd *)0);
}

inline RootWnd *RootWnd::getRootParent() {
  return _call(GETROOTPARENT, (RootWnd *)0);
}

inline void RootWnd::setParent(RootWnd *parent) {
  _voidcall(SETPARENT, parent);
}

inline int RootWnd::onDeferredCallback(int param1, int param2) {
  return _call(ONDEFERREDCALLBACK, 0, param1, param2);
}

inline int RootWnd::childNotify(RootWnd *child, int msg, int p1, int p2) {
  return _call(CHILDNOTIFY, 0, child, msg, p1, p2);
}

inline int RootWnd::getPreferences(int what) {
  return _call(GETPREFERENCES, 0, what);
}

inline int RootWnd::beginCapture() {
  return _call(BEGINCAPTURE, 0);
}

inline int RootWnd::endCapture() {
  return _call(ENDCAPTURE, 0);
}

inline int RootWnd::getCapture() {
  return _call(GETCAPTURE, 0);
}

inline int RootWnd::isClickThrough() {
  return _call(CLICKTHROUGH, 0);
}

inline void RootWnd::clientToScreen(int *x, int *y) {
  _voidcall(CLIENTSCREEN, x, y);
}

inline void RootWnd::screenToClient(int *x, int *y) {
  _voidcall(SCREENCLIENT, x, y);
}

inline int RootWnd::init(RootWnd *parent, int nochild) {
  return _call(INIT, 0, parent, nochild);
}

inline int RootWnd::getCursorType(int x, int y) {
  return _call(GETCURSORTYPE, 0, x, y);
}

inline  void RootWnd::getClientRect(RECT *r) {
  _voidcall(GETCLIENTRECT, r);
}

inline void RootWnd::getNonClientRect(RECT *rect) {
  _voidcall(GETNONCLIENTRECT, rect);
}

inline void RootWnd::setVisible(int show) {
  _voidcall(SETVISIBLE, show);
}

inline void RootWnd::invalidate() {
  _voidcall(INVALIDATE);
}
  
inline void RootWnd::invalidateRect(RECT *r) {
  _voidcall(INVALIDATERECT, r);
}

inline void RootWnd::invalidateRgn(Region *r) {
  _voidcall(INVALIDATERGN, r);
}

inline void RootWnd::invalidateFrom(RootWnd *who) {
  _voidcall(INVALIDATEFROM, who);
}

inline void RootWnd::invalidateRectFrom(RECT *r, RootWnd *who) {
  _voidcall(INVALIDATERECTFROM, r, who);
}

inline void RootWnd::invalidateRgnFrom(Region *r, RootWnd *who) {
  _voidcall(INVALIDATERGNFROM, r, who);
}

inline void RootWnd::validate() {
  _voidcall(VALIDATE);
}

inline void RootWnd::validateRect(RECT *r) {
  _voidcall(VALIDATERECT, r);
}

inline void RootWnd::validateRgn(Region *reg) {
  _voidcall(VALIDATERGN, reg);
}

inline int RootWnd::onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx) {
  return _call(ONSIBINVALIDATE, 0, r, who, who_idx, my_idx);
}

inline int RootWnd::wantSiblingInvalidations() {
  return _call(WANTSIBINVALIDATE, 0);
}

inline int RootWnd::cascadeRepaintFrom(RootWnd *who) {
  return _call(CASCADEREPAINTFROM, 0, who);
}

inline int RootWnd::cascadeRepaintRgnFrom(Region *reg, RootWnd *who) {
  return _call(CASCADEREPAINTRGNFROM, 0, reg, who);
}

inline int RootWnd::cascadeRepaintRectFrom(RECT *r, RootWnd *who) {
  return _call(CASCADEREPAINTRECTFROM, 0, r, who);
}

inline int RootWnd::cascadeRepaint() {
  return _call(CASCADEREPAINT, 0);
}

inline int RootWnd::cascadeRepaintRgn(Region *reg) {
  return _call(CASCADEREPAINTRGN, 0, reg);
}

inline int RootWnd::cascadeRepaintRect(RECT *r) {
  return _call(CASCADEREPAINTRECT, 0, r);
}

inline void RootWnd::repaint() {
  _voidcall(REPAINT);
}

inline RootWnd *RootWnd::getBaseTextureWindow() {
  return _call(GETTEXTUREWND, (RootWnd *)0);
}

inline int RootWnd::onActivate() {
  return _call(ONACTIVATE, 0);
}
  
inline int RootWnd::onDeactivate() {
  return _call(ONDEACTIVATE, 0);
}

class COMEXP NOVTABLE RootWndI : public RootWnd {
protected:
  RootWndI() {}	// protect constructor

public:
#ifdef WIN32
  virtual HWND gethWnd()=0;
#endif
  virtual DragInterface *getDragInterface()=0;
  virtual RootWnd *rootWndFromPoint(POINT *pt)=0;

  virtual void getWindowRect(RECT *r)=0;
  virtual int isVisible()=0;
  virtual int getSkinId()=0;
  virtual int onMetricChange(int metricid, int param1, int param2)=0;
  virtual int onDeferredCallback(int param1, int param2)=0;
  virtual int childNotify(RootWnd *child, int msg, int p1, int p2)=0;
  virtual int getPreferences(int what)=0;

  virtual int rootwnd_paintTree(CanvasBase *canvas, Region *r)=0;
  virtual RootWnd *getParent()=0;
  virtual RootWnd *getRootParent()=0;
  virtual void setParent(RootWnd *parent)=0;

  virtual int beginCapture()=0;
  virtual int endCapture()=0;
  virtual int getCapture()=0;

  virtual int rootwnd_onPaint(CanvasBase *canvas, Region *r)=0;
  virtual int isClickThrough()=0;

  virtual void clientToScreen(int *x, int *y)=0; 
  virtual void screenToClient(int *x, int *y)=0; 

  virtual int init(RootWnd *parent, int nochild)=0;
  virtual int getCursorType(int x, int y)=0;
  virtual void getClientRect(RECT *r)=0;
  virtual void getNonClientRect(RECT *rect)=0;
  virtual void setVisible(int show)=0;
  virtual void invalidate()=0;
  virtual void invalidateRect(RECT *r)=0;
  virtual void invalidateRgn(Region *r)=0;
  virtual void invalidateFrom(RootWnd *who)=0;
  virtual void invalidateRectFrom(RECT *r, RootWnd *who)=0;
  virtual void invalidateRgnFrom(Region *r, RootWnd *who)=0;
  virtual void validate()=0;
  virtual void validateRect(RECT *r)=0;
  virtual void validateRgn(Region *reg)=0;

  virtual int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx)=0;
  virtual int wantSiblingInvalidations()=0;
  virtual int cascadeRepaintFrom(RootWnd *who)=0;
  virtual int cascadeRepaintRgnFrom(Region *reg, RootWnd *who)=0;
  virtual int cascadeRepaintRectFrom(RECT *r, RootWnd *who)=0;
  virtual int cascadeRepaint()=0;
  virtual int cascadeRepaintRgn(Region *reg)=0;
  virtual int cascadeRepaintRect(RECT *r)=0;
  virtual void repaint()=0;
  virtual RootWnd *getBaseTextureWindow()=0;
  
  virtual int onActivate()=0;
  virtual int onDeactivate()=0;

protected:
  RECVS_DISPATCH;
};

#endif
