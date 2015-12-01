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

#ifndef __SCRLBKGWND_H
#define __SCRLBKGWND_H

#include "../common/canvas.h"
#include "../common/autobitmap.h"
#include "../common/labelwnd.h"
#include "../common/scrollbar.h"
#include "../common/sepwnd.h"

#define SCRLBKGWND_PARENT LabelWnd

class COMEXP ScrlBkgWnd : public SCRLBKGWND_PARENT {
protected:
  ScrlBkgWnd();
public:
  virtual ~ScrlBkgWnd();
  
  virtual int onInit();
  virtual int onPaint(Canvas *c);
  virtual void drawBackground(Canvas *canvas);
  virtual int onEraseBkgnd(HDC dc);
  virtual int childNotify(RootWnd *child, int msg, int param1, int param2);
  virtual int onResize();
  virtual void getClientRect(RECT *r);
  virtual void getNonClientRect(RECT *r);
  virtual int getHeaderHeight();
  virtual void timerCallback (int id);

  virtual void onHScrollToggle(BOOL set);
  virtual void onVScrollToggle(BOOL set);
  virtual void onSetVisible(int show);
  virtual int wantHScroll() { return 1; }
  virtual int wantVScroll() { return 1; }
  void makeWindowOverlayMask(Region *r);

  SkinBitmap *getBgBitmap(void);
  void setBgBitmap(char *b);
  void setBgColor(COLORREF rgb);
  virtual COLORREF getBgColor(void);

  virtual int getContentsWidth();
  virtual int getContentsHeight();

  void setLineHeight(int h);
  int getLinesPerPage();

  int getScrollX();
  int getScrollY();
  int getScrollbarWidth();
  void freeResources();
  void reloadResources();

protected:

  void scrollToY(int y, int signal=TRUE);
  void scrollToX(int x, int signal=TRUE);
  void setSlidersPosition();
  int needDoubleBuffer();
  BOOL needHScroll();
  BOOL needVScroll();
  int getMaxScrollY();
  int getMaxScrollX();
  void updateScrollY(BOOL smooth=FALSE);
  void updateScrollX(BOOL smooth=FALSE);
  void smoothScrollToY(int y);
  void smoothScrollToX(int x);
  void updateVScroll(int y);
  void updateHScroll(int x);

  AutoSkinBitmap bmp;
  
  int dbbuffer;
  BOOL inDestroy;

  ScrollBar *hScroll;
  ScrollBar *vScroll;
	SepWnd *hSep;
	SepWnd *vSep;

	COLORREF bgColor;

	int scrollX;
	int scrollY;

	BOOL needSetSliders;
	BOOL wantsep;

	int lineHeight;

	float smoothScrollYInc, smoothScrollXInc;
	float smoothScrollYCur, smoothScrollXCur;
	int smoothScrollYTimerCount, smoothScrollXTimerCount;
	int smoothYTimer, smoothXTimer;
  void killSmoothYTimer();
  void killSmoothXTimer();
  double lastratio;
  RECT smsqr;
  
};

#endif
