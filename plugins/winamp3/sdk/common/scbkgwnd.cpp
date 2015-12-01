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

#include <windows.h>
#include <commctrl.h>
#include "../common/scbkgwnd.h"

#define SCROLLBAR_SEP       4
#define TIMER_SMOOTHSCROLLY 8873
#define TIMER_SMOOTHSCROLLX 8874
#define SMOOTH_STEPS        5

ScrlBkgWnd::ScrlBkgWnd() {
  inDestroy = FALSE;
	bmp = NULL;
  bgColor = CLR_NONE;
	hScroll = NULL;
	vScroll = NULL;
	hSep = NULL;
	vSep = NULL;
  scrollX = 0;
  scrollY = 0;
  dbbuffer=1;
  needSetSliders=FALSE;
  lineHeight = 16;
  wantsep=0;
  lastratio = 1.0;
  MEMSET(&smsqr, 0, sizeof(RECT));
}

ScrlBkgWnd::~ScrlBkgWnd() {
  inDestroy = TRUE;
	if (hScroll) delete hScroll;
	hScroll = NULL;
	if (vScroll) delete vScroll;
	vScroll = NULL;
	if (hSep) delete hSep;
	hSep = NULL;
	if (vSep) delete vSep;
	vSep = NULL;
}

ScrlBkgWnd::onInit() {

  SCRLBKGWND_PARENT::onInit();

	scrollY=0;
	scrollX=0;

	hScroll = new ScrollBar();
	hSep = new SepWnd();
	hSep->setOrientation(SEP_HORIZONTAL);

  hScroll->setBitmaps("studio.scrollbar.horizontal.left", "studio.scrollbar.horizontal.left.pressed", "studio.scrollbar.horizontal.left.hilite",
                      "studio.scrollbar.horizontal.right", "studio.scrollbar.horizontal.right.pressed", "studio.scrollbar.horizontal.right.hilite",
                      "studio.scrollbar.horizontal.button", "studio.scrollbar.horizontal.button.pressed", "studio.scrollbar.horizontal.button.hilite");

	vScroll = new ScrollBar();
	vSep = new SepWnd();
	vSep->setOrientation(SEP_VERTICAL);

  vScroll->setBitmaps("studio.scrollbar.vertical.left", "studio.scrollbar.vertical.left.pressed", "studio.scrollbar.vertical.left.hilite",
                      "studio.scrollbar.vertical.right", "studio.scrollbar.vertical.right.pressed", "studio.scrollbar.vertical.right.hilite",
                      "studio.scrollbar.vertical.button", "studio.scrollbar.vertical.button.pressed", "studio.scrollbar.vertical.button.hilite");

//	hScroll->setVertical(FALSE);
	vScroll->setVertical(TRUE);

	hScroll->setStartHidden(TRUE);  // prevent showing window at creation
	vScroll->setStartHidden(TRUE);
	hSep->setStartHidden(TRUE);
	vSep->setStartHidden(TRUE);

	hScroll->setParent(this);
	vScroll->setParent(this);
	hSep->setParent(this);
	vSep->setParent(this);

	hScroll->init(gethInstance(), gethWnd());
	vScroll->init(gethInstance(), gethWnd());
	hSep->init(gethInstance(), gethWnd());
	vSep->init(gethInstance(), gethWnd());

	hScroll->setPosition(0);
	vScroll->setPosition(0);

	setSlidersPosition();           // position sliders and show them if needed
  return 1;
}

void ScrlBkgWnd::setBgBitmap(char *b) {
  bmp = b;
  if (b)
	  setBgColor(CLR_NONE);
}

void ScrlBkgWnd::setBgColor(COLORREF rgb) {
  bgColor = rgb;
}

SkinBitmap *ScrlBkgWnd::getBgBitmap(void) {
  return bmp;
}

COLORREF ScrlBkgWnd::getBgColor(void) {
  return bgColor;
}

// Scroll to a specified Y-pixels
void ScrlBkgWnd::scrollToY(int y, int signal) {
  WndCanvas *canvas=NULL;
  RECT r;
  int offset;

  if (ABS(getRenderRatio() - 1.0) > 0.01) {
	  scrollY=y;
    getClientRect(&r);
  	invalidateRect(&r);
    return;
  }

  if (y>scrollY) { // tree scrolling up, scroller going down. invalidating from the bottom. bitblting from bottom to top
    int lines = y-scrollY;
    offset=-lines;
    getClientRect(&r);
    canvas = new WndCanvas();
    canvas->attachToClient(this);

    Region *reg = new Region();
    makeWindowOverlayMask(reg);
    Region *clip = new Region(&r);
    reg->offset(0, offset);
    clip->subtract(reg);
    canvas->selectClipRgn(clip);

    canvas->blit(r.left, r.top+lines, canvas, r.left, r.top, r.right-r.left, r.bottom-r.top-lines);

    canvas->selectClipRgn(NULL);
	  if (!reg->isEmpty())
	    invalidateRgn(reg);
    delete clip;
	  delete reg;

    r.top = r.bottom-lines;
  }
  if (y<scrollY) { // tree scrolling down, scroller going up. invalidating from the top. bitblting from top to bottom
    int lines = scrollY-y;
    offset=lines;
    getClientRect(&r);
    canvas = new WndCanvas();
    canvas->attachToClient(this);

    Region *reg = new Region();
    makeWindowOverlayMask(reg);
    Region *clip = new Region(&r);
    reg->offset(0, offset);
    clip->subtract(reg);
    canvas->selectClipRgn(clip);

    canvas->blit(r.left, r.top, canvas, r.left, r.top+lines, r.right-r.left, r.bottom-r.top-lines);

    canvas->selectClipRgn(NULL);
	  if (!reg->isEmpty())
  	  invalidateRgn(reg);
    delete clip;
	  delete reg;

    r.bottom = r.top+lines+1;
  }
  if (canvas) {
	  delete canvas;
	  scrollY=y;

    // in case we have a virtualCanvas, we need to tell BaseWnd to call us to paint on it next time it's needed coz we blited directly to the screen
    RECT cr;
    getClientRect(&cr);
    RECT screenblit;
    SubtractRect(&screenblit, &cr, &r);
    deferedInvalidateRect(&screenblit);

    // then invalidate what's needed
	  invalidateRect(&r);

	  dbbuffer = 1;
	  repaint();
  }

	if (signal) 
	  updateVScroll(y);
}

// Scroll to a specified X-pixel
void ScrlBkgWnd::scrollToX(int x, int signal) {
  WndCanvas *canvas=NULL;
  RECT r;
  int offset;

  if (ABS(getRenderRatio() - 1.0) > 0.01) {
	  scrollX=x;
    getClientRect(&r);
  	invalidateRect(&r);
    return;
  }

  if (x>scrollX) { // tree scrolling left, scroller going right. invalidating from the right. bitblting from right to left
    int lines = x-scrollX;
    offset=-lines;
    getClientRect(&r);
    r.top-=getHeaderHeight();
    canvas = new WndCanvas();
    canvas->attachToClient(this);

    Region *reg = new Region();
    makeWindowOverlayMask(reg);
    Region *clip = new Region(&r);
    reg->offset(offset, 0);
    clip->subtract(reg);
    canvas->selectClipRgn(clip);

    canvas->blit(r.left+lines, r.top, canvas, r.left, r.top, r.right-r.left-lines, r.bottom-r.top);

    canvas->selectClipRgn(NULL);
	  if (!reg->isEmpty())
  	  invalidateRgn(reg);
    delete clip;
	  delete reg;

    r.left = r.right-lines-1;
  }
  if (x<scrollX) { // tree scrolling right, scroller going left. invalidating from the left. bitblting from left to right
    int lines = scrollX-x;
    offset=lines;
    getClientRect(&r);
    r.top-=getHeaderHeight();
    canvas = new WndCanvas();
    canvas->attachToClient(this);

    Region *reg = new Region();
    makeWindowOverlayMask(reg);
    Region *clip = new Region(&r);
    reg->offset(offset, 0);
    clip->subtract(reg);
    canvas->selectClipRgn(clip);

    canvas->blit(r.left, r.top, canvas, r.left+lines, r.top, r.right-r.left-lines, r.bottom-r.top);

    canvas->selectClipRgn(NULL);
	  if (!reg->isEmpty())
  	  invalidateRgn(reg);
    delete clip;
	  delete reg;

    r.right = r.left+lines+1;
  }
  if (canvas) {
	  delete canvas;
	  scrollX=x;

    // in case we have a virtualCanvas, we need to tell BaseWnd to call us to paint on it next time it's needed coz we blited directly to the screen
    RECT cr;
    getClientRect(&cr);
    cr.top-=getHeaderHeight();
    RECT screenblit;
    SubtractRect(&screenblit, &cr, &r);
    deferedInvalidateRect(&screenblit);

    // then invalidate what's needed
	  invalidateRect(&r);

	  dbbuffer = 1;
	  repaint();
  }
  
  if (signal)
    updateHScroll(x);
}

void ScrlBkgWnd::setSlidersPosition() {

  if (!gethWnd()) return;

  RECT d;
  getClientRect(&d);
  if ((d.left >= d.right) || (d.top >= d.bottom))
    return;

	RECT r;
	if (inDestroy) return;
	if (!isVisible()) {
	  needSetSliders=TRUE;
	  return;
	}

  needSetSliders=FALSE;

	if (needHScroll()) {
		getNonClientRect(&r);
		r.top = r.bottom - getScrollbarWidth();
		if (needVScroll())
			r.right -= getScrollbarWidth() + (wantsep ? SCROLLBAR_SEP : 0);
		RECT z; hScroll->getNonClientRect(&z);
		if (!EqualRect(&r, &z)) {
  		hScroll->resizeToRect(&r);
  		RECT s=r;
  		s.bottom = s.top;
  		s.top -= (wantsep ? SCROLLBAR_SEP : 0);
  		hSep->resizeToRect(&s);
		}
		if (!hScroll->isVisible()) {
  		hScroll->setVisible(TRUE);
  		if (wantsep) hSep->setVisible(TRUE);
  		onHScrollToggle(TRUE);
		}
		hScroll->setNPages(((int)(getContentsWidth() / (r.right-r.left)))+1);
		hScroll->setUpDownValue((int)(((float)lineHeight / (getContentsWidth()-(r.right-r.left)))*SCROLLBAR_FULL));
		hScroll->setPosition((int)((float)scrollX / getMaxScrollX() * SCROLLBAR_FULL));
	} else {
		if (hScroll->isVisible()) {
		  hScroll->setVisible(FALSE);
  		if (wantsep) hSep->setVisible(FALSE);
      onHScrollToggle(FALSE);
		}
		hScroll->setPosition(0);
		scrollToX(0);
	}

	if (needVScroll()) {
		getNonClientRect(&r);
		r.left = r.right - getScrollbarWidth();
    if (needHScroll())
      r.bottom -= getScrollbarWidth();
		RECT z; vScroll->getNonClientRect(&z);
		if (!EqualRect(&r, &z)) {
		  vScroll->resizeToRect(&r);
  		RECT s=r;
  		s.right = s.left;
  		s.left -= (wantsep ? SCROLLBAR_SEP : 0);
  		vSep->resizeToRect(&s);
		}
		if (!vScroll->isVisible()) {
		  vScroll->setVisible(TRUE);
  		if (wantsep) vSep->setVisible(TRUE);
		  onVScrollToggle(TRUE);
		}
		vScroll->setNPages(((int)(getContentsHeight() / (r.bottom-r.top)))+1);
		vScroll->setUpDownValue((int)(((float)lineHeight / (getContentsHeight()-(r.bottom-r.top)))*SCROLLBAR_FULL));
		vScroll->setPosition((int)((float)scrollY / getMaxScrollY() * SCROLLBAR_FULL));
	} else {
		if (vScroll->isVisible()) {
  		vScroll->setVisible(FALSE);
  		if (wantsep) vSep->setVisible(FALSE);
		  onVScrollToggle(FALSE);
		}
		vScroll->setPosition(0);
		scrollToY(0);
	}

  if (hSep) hSep->invalidate();
  if (vSep) vSep->invalidate();

  if (needHScroll() && needVScroll()) {
    getNonClientRect(&smsqr);
    smsqr.left = smsqr.right - getScrollbarWidth();
    smsqr.top = smsqr.bottom - getScrollbarWidth();
    invalidateRect(&smsqr);
  }
}

void ScrlBkgWnd::onHScrollToggle(BOOL set) {
}

void ScrlBkgWnd::onVScrollToggle(BOOL set) {
}

int ScrlBkgWnd::onPaint(Canvas *canvas) {
  RECT d;
  getClientRect(&d);
  if ((d.left >= d.right) || (d.top >= d.bottom)) {
    return SCRLBKGWND_PARENT::onPaint(canvas);
  }
  
  if (needSetSliders) setSlidersPosition();

  RECT z;
  GetUpdateRect(gethWnd(), &z, FALSE);

  PaintCanvas paintcanvas;
  PaintBltCanvas paintbcanvas;

  if (canvas == NULL) {
    if (dbbuffer) {
      if (!paintbcanvas.beginPaintNC(this)) return 0;
      canvas = &paintbcanvas;
    } else {
      if (!paintcanvas.beginPaint(this)) return 0;
      canvas = &paintcanvas;
    }
  }
  dbbuffer=1;
  SCRLBKGWND_PARENT::onPaint(canvas);

  Region *smsq = NULL;

  if (needHScroll() && needVScroll()) {
    renderBaseTexture(canvas, smsqr);
    smsq = new Region(&smsqr);
  }

  RECT r;
  getNonClientRect(&r);
  RECT c={r.left,0,r.right,r.top}; // create label rect

  Region *reg = new Region(&c);
  Region *clip = new Region();
  if (canvas->getClipRgn(clip) == 0) { 
    delete clip;
    clip = new Region(&r);
    if (smsq) clip->subtract(smsq);
    canvas->selectClipRgn(clip);
  } else {
    clip->subtract(reg);
    if (smsq) clip->subtract(smsq);
    canvas->selectClipRgn(clip);
  }
  if (smsq) delete smsq;

  drawBackground(canvas);
  delete clip;
  delete reg;
  if (getRenderRatio() != lastratio) { invalidate(); lastratio = getRenderRatio(); } // todo: make that an event
  return 1;
}

int ScrlBkgWnd::needDoubleBuffer() {
  return dbbuffer;
}

int ScrlBkgWnd::onEraseBkgnd(HDC dc) {

/*	DCCanvas canvas;
	canvas.cloneDC(dc);

	drawBackground(&canvas);*/
	
	return 1;
}

// Draws tiled background
void ScrlBkgWnd::drawBackground(Canvas *canvas) {

	RECT r;
	getClientRect(&r);
  if (bmp.getBitmap() && bgColor == CLR_NONE) {
  	r.top-=scrollY%bmp.getBitmap()->getHeight();
	  r.left-=scrollX%bmp.getBitmap()->getWidth();
  	bmp.getBitmap()->blitTile(canvas, &r);
  } else if (bgColor != CLR_NONE) {
    canvas->fillRect(&r, bgColor);
  }
}

BOOL ScrlBkgWnd::needHScroll() {
  if (!vScroll || !wantHScroll()) return FALSE;
  RECT r;
  getNonClientRect(&r);
  if (vScroll->isVisible())
    r.right -= getScrollbarWidth();
  return (getContentsWidth() > r.right - r.left);
}

BOOL ScrlBkgWnd::needVScroll() {
  if (!hScroll || !wantVScroll()) return FALSE;
  RECT r;
  getNonClientRect(&r);
  r.top += getHeaderHeight();
  if (hScroll->isVisible())
    r.bottom -= getScrollbarWidth();
  return (getContentsHeight() > r.bottom - r.top);
}

// Returns the current tree width in pixels
int ScrlBkgWnd::getContentsWidth() {
  /*RECT r;
  ScrlBkgWnd::getClientRect(&r);
  return r.right-r.left;*/
  return 10000;
}

// Returns the current tree height in pixels
int ScrlBkgWnd::getContentsHeight() {
  /*RECT r;
  ScrlBkgWnd::getClientRect(&r);
  return r.bottom-r.top;*/
  return 10000;
}

int ScrlBkgWnd::getMaxScrollY() {
  RECT r;
  getClientRect(&r);
  return max(0, getContentsHeight()-(r.bottom-r.top));
}

int ScrlBkgWnd::getMaxScrollX() {
  RECT r;
  getClientRect(&r);
  return max(0, getContentsWidth()-(r.right-r.left));
}

void ScrlBkgWnd::updateVScroll(int y) {
	int z = (int)((float)y/getMaxScrollY()*SCROLLBAR_FULL);
  vScroll->setPosition(z);
}

void ScrlBkgWnd::updateHScroll(int x) {
	int z = (int)((float)x/getMaxScrollX()*SCROLLBAR_FULL);
	hScroll->setPosition(z);
}

void ScrlBkgWnd::updateScrollY(int smooth) {
  int y = (int)((float)(vScroll->getPosition())/SCROLLBAR_FULL * getMaxScrollY());
  if (!smooth)
    scrollToY(y /*& ~3*/);
  else
    smoothScrollToY(y);
}

void ScrlBkgWnd::updateScrollX(int smooth) {
	int x = (int)((float)(hScroll->getPosition())/SCROLLBAR_FULL * getMaxScrollX());
  if (!smooth)
  	scrollToX(x /*& ~3*/);
  else
    smoothScrollToX(x);
}

void ScrlBkgWnd::smoothScrollToX(int x) {
  killSmoothXTimer();
  smoothScrollXInc = -(float)(scrollX - x) / SMOOTH_STEPS;
  smoothScrollXCur = (float)scrollX;
  smoothScrollXTimerCount = 0;
  smoothXTimer = 1;
  setTimer(TIMER_SMOOTHSCROLLX, 25);
}

void ScrlBkgWnd::killSmoothYTimer() {
  if (smoothYTimer) {
    killTimer(TIMER_SMOOTHSCROLLY);
    smoothScrollYCur += smoothScrollYInc * (SMOOTH_STEPS - smoothScrollYTimerCount);
    scrollToY((int)smoothScrollYCur);
    smoothYTimer = 0;
    updateVScroll(scrollY);
  }
}

void ScrlBkgWnd::killSmoothXTimer() {
  if (smoothXTimer) {
    killTimer(TIMER_SMOOTHSCROLLX);
    smoothScrollXCur += smoothScrollXInc * (SMOOTH_STEPS - smoothScrollXTimerCount);
    scrollToX((int)smoothScrollXCur);
    smoothXTimer = 0;
    updateHScroll(scrollX);
  }
}

void ScrlBkgWnd::smoothScrollToY(int y) {
  killSmoothYTimer();
  smoothScrollYInc = -(float)(scrollY - y) / SMOOTH_STEPS;
  smoothScrollYCur = (float)scrollY;
  smoothScrollYTimerCount = 0;
  smoothYTimer = 1;
  setTimer(TIMER_SMOOTHSCROLLY, 25);
}

void ScrlBkgWnd::timerCallback (int id) {
  switch (id) {
    case TIMER_SMOOTHSCROLLY:
      smoothScrollYCur += smoothScrollYInc;
      scrollToY((int)smoothScrollYCur, FALSE);
      if (++smoothScrollYTimerCount == SMOOTH_STEPS)
        killSmoothYTimer();
      return;
    case TIMER_SMOOTHSCROLLX:
      smoothScrollXCur += smoothScrollXInc;
      scrollToX((int)smoothScrollXCur, FALSE);
      if (++smoothScrollXTimerCount == SMOOTH_STEPS)
        killSmoothXTimer();
      return;
  }
  SCRLBKGWND_PARENT::timerCallback(id);
}

// Gets notification from sliders
int ScrlBkgWnd::childNotify(RootWnd *child, int msg, int param1, int param2) {
  switch (msg) {
    case UMSG_SCROLLBAR_SETPOSITION:
    	if (child == vScroll) {
        updateScrollY(param1);
        return 1;
    	}
    	if (child == hScroll) {
    	  updateScrollX(param1);
    	  return 1;
    	}
    	break;
  }

  return SCRLBKGWND_PARENT::childNotify(child, msg, param1, param2);
}

int ScrlBkgWnd::onResize() {
  SCRLBKGWND_PARENT::onResize();
  invalidateRect(&smsqr);
  setSlidersPosition();
  return 1;
}

void ScrlBkgWnd::onSetVisible(int show) {
  if (show)
    setSlidersPosition();
}

void ScrlBkgWnd::getClientRect(RECT *r) {
  SCRLBKGWND_PARENT::getClientRect(r);
  if (needVScroll())
    r->right-=getScrollbarWidth()+(wantsep ? SCROLLBAR_SEP : 0);
  if (needHScroll())
    r->bottom-=getScrollbarWidth()+(wantsep ? SCROLLBAR_SEP : 0);
  r->top += getHeaderHeight();
}

void ScrlBkgWnd::getNonClientRect(RECT *r) {
  SCRLBKGWND_PARENT::getClientRect(r); // my non client rect is my parent's client rect
  return;
}

int ScrlBkgWnd::getHeaderHeight() {
  return 0;
}

void ScrlBkgWnd::setLineHeight(int h) {
  lineHeight = h;
}

int ScrlBkgWnd::getLinesPerPage() {
  RECT r;
  getClientRect(&r);
  int h = r.bottom - r.top;
  return h / lineHeight;
}

int ScrlBkgWnd::getScrollX() {
  return scrollX;
}

int ScrlBkgWnd::getScrollY() {
  return scrollY;
}

int ScrlBkgWnd::getScrollbarWidth() {
  if (hScroll) return hScroll->getWidth();
  if (vScroll) return vScroll->getWidth();
  return 0;
}

/*void ScrlBkgWnd::clientToScreen(RECT *r) {
  POINT p;
  p.x = r->left;
  p.y = r->top;
  SCRLBKGWND_PARENT::clientToScreen((int *)&p.x, (int*)&p.y);
  r->left = p.x;
  r->top = p.y;

  p.x = r->right;
  p.y = r->bottom;
  SCRLBKGWND_PARENT::clientToScreen((int *)&p.x, (int*)&p.y);
  r->right = p.x;
  r->bottom = p.y;
}

void ScrlBkgWnd::clientToScreen(int *x, int *y) {
  SCRLBKGWND_PARENT::clientToScreen(x, y);
}

void ScrlBkgWnd::clientToScreen(POINT *p) {
  TREEWND_PARENT::clientToScreen((int *)&p->x, (int *)&p->y);
}*/

void ScrlBkgWnd::freeResources() {
  SCRLBKGWND_PARENT::freeResources();
}

void ScrlBkgWnd::reloadResources() {
  SCRLBKGWND_PARENT::reloadResources();
}

void ScrlBkgWnd::makeWindowOverlayMask(Region *r) {

  HDC dc = GetDC(gethWnd());

  if (getRandomRgn) {
    RECT cr;
    getClientRect(&cr);
    RECT wr;
    getWindowRect(&wr);

    Region *sr = new Region();
    getRandomRgn(dc, sr->getHRGN(), SYSRGN);
    sr->offset(-wr.left, -wr.top);

    r->setRect(&cr);
    r->subtract(sr);
    delete sr;
  }
  
  ReleaseDC(gethWnd(), dc);  
}
