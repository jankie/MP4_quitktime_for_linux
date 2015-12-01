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

#ifndef _VIRTUALWND_H
#define _VIRTUALWND_H

#include "basewnd.h"

#define VIRTUALWND_PARENT BaseWnd
#define AUTOWH 0xFFFE

class COMEXP NOVTABLE VirtualWnd : public VIRTUALWND_PARENT {
protected:
  VirtualWnd();
  virtual ~VirtualWnd();
public:	
  virtual int init(RootWnd *parent, int nochild=FALSE);
  virtual int init(HINSTANCE hInstance, HWND parent, int nochild=FALSE);

  virtual void bringToFront();
  virtual void bringToBack();
  virtual void bringAbove(BaseWnd *w);
  virtual void bringBelow(BaseWnd *w);

#ifdef WIN32
  //NONPORTABLE--avoid prolonged use
  virtual HWND gethWnd();
  virtual HINSTANCE gethInstance();
#endif

protected:
  int setTimer(int id, int ms);
  int killTimer(int id);

public:
  virtual void resize(int x, int y, int w, int h);
  virtual void resize(RECT *r);
//CUT  virtual void resize(RECT *r);
  virtual void move(int x, int y);
  virtual void invalidate();
  virtual void invalidateRect(RECT *r);
  virtual void invalidateRgn(Region *reg);
  virtual void validate();
  virtual void validateRect(RECT *r);
  virtual void validateRgn(Region *reg);
  virtual void getClientRect(RECT *);
  virtual void getWindowRect(RECT *);
  virtual int beginCapture();
  virtual int endCapture();
  virtual int getCapture();
/*	virtual void setVirtualChildCapture(BaseWnd *child);
	virtual int setVirtualChildTimer(BaseWnd *child, int id, int ms);
	virtual int killVirtualChildTimer(BaseWnd *child, int id);*/
	virtual void setVisible(int show);
	virtual int isVisible();
	virtual void repaint();
  virtual void moveToFront();
  virtual void moveToBack();
/*  virtual int focusNextSibbling(int dochild);
  virtual int focusNextVirtualChild(BaseWnd *child);*/
  virtual int cascadeRepaint();
  virtual int cascadeRepaintRect(RECT *r);
  virtual int cascadeRepaintRgn(Region *r);
  virtual RootWnd *rootWndFromPoint(POINT *pt);
  virtual double getRenderRatio();
  virtual void postDeferredCallback(int param1, int param2);
  virtual int reparent(RootWnd *newparent);
  virtual int setVirtual(int i);
  virtual RootWnd *getRootParent();

public:	
	virtual int isVirtual() { return !bypassvirtual; }
	virtual int isInited();

protected:
	int virtualX,virtualY,virtualH,virtualW;
	int visible;
	int inited;
  int bypassvirtual;
};

#endif
