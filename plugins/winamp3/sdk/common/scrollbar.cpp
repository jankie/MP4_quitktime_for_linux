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
#include "scrollbar.h"
#include "canvas.h"

#define TIMER_ID  9871
#define TIMER_ID2 9872

#define FIRST_DELAY 350
#define NEXT_DELAY  75

ScrollBar::ScrollBar() {
  leftrgn = NULL;
  rightrgn = NULL;
  buttonrgn = NULL;

  position = 0;
  moving = 0;
  lefting = 0;
  righting = 0;
  clicked = 0;
  height = DEFAULT_HEIGHT;
  buttonx = 0;

  shiftleft = 0;
  shiftright = 0;

  curmouseposition = POS_NONE;
  clickmouseposition = POS_NONE;
  pageing = 0;
  timer = 0;
  npages = 100;
  pageway = PAGE_NONE;
  updown = 256;
  insetpos = 0;

  vertical = 0;
}

ScrollBar::~ScrollBar() {
  deleteResources();
}

void ScrollBar::deleteResources() {
  if (leftrgn) delete leftrgn; leftrgn = NULL;
  if (buttonrgn) delete buttonrgn; buttonrgn = NULL;
  if (rightrgn) delete rightrgn; rightrgn = NULL;
}

// this one is inherited
void ScrollBar::freeResources() {
  SCROLLBAR_PARENT::freeResources();
  deleteResources();
}

void ScrollBar::reloadResources() {
  SCROLLBAR_PARENT::reloadResources();
  loadBmps();
}


int ScrollBar::onMouseMove (int x, int y) {

  SCROLLBAR_PARENT::onMouseMove(x, y);
  lastx = x;
  lasty = y;

  if (clicked && clickmouseposition == POS_BUTTON) {

    POINT pt={x,y};
    int x;
    if (!vertical) 
      x = pt.x - clickpos.x;
    else
      x = pt.y - clickpos.y;

    RECT r;
    getClientRect(&r);
    int maxwidth;
    if (!vertical)
      maxwidth = (r.right-r.left)-(shiftright+shiftleft+bmpbutton.getWidth())+1;
    else
      maxwidth = (r.bottom-r.top)-(shiftright+shiftleft+bmpbutton.getHeight())+1;
    buttonx = min(max(clickbuttonx + x, 0), maxwidth);
    calcPosition();
    invalidate();

  } else {

    int oldposition = curmouseposition;
    curmouseposition = getMousePosition();
    if (oldposition != curmouseposition) invalidate();

    if (curmouseposition != POS_NONE && !getCapture())
      beginCapture();

    if (curmouseposition == POS_NONE && getCapture() && !clicked && !pageing)
      endCapture();
  }


  return 1;
}

int ScrollBar::getWidth() {
  if (!bmpbutton) return 0;
  if (!vertical)
    return bmpbutton.getHeight();
  else
    return bmpbutton.getWidth();
  return 0;
}

int ScrollBar::getMousePosition() {
  int v = POS_NONE;

  POINT pt={lastx, lasty};

  RECT c;
  getClientRect(&c);
  pt.x -= c.left;
  pt.y -= c.top;

  Region *l, *b, *r;
  l = leftrgn->clone();
  b = buttonrgn->clone();
  if (!vertical)
    b->offset(buttonx+shiftleft, 0);
  else
    b->offset(0, buttonx+shiftleft);
  r = rightrgn->clone();
  if (!vertical)
    r->offset(c.right-c.left-bmpleft.getWidth(), 0);
  else
    r->offset(0, c.bottom-c.top-bmpleft.getHeight());

  if (b->ptInRegion(&pt))
    v = POS_BUTTON;
  if (l->ptInRegion(&pt))
    v = POS_LEFT;
  if (r->ptInRegion(&pt))
    v = POS_RIGHT;

  delete l;
  delete b;
  delete r;

  return v;
}

int ScrollBar::onLeftButtonDown(int x, int y) {
  clickmouseposition = getMousePosition();
  if (!pageing && clickmouseposition != POS_NONE) {
    clicked = 1;
    if (clickmouseposition == POS_LEFT || clickmouseposition == POS_RIGHT)
      handleUpDown();
    if (clickmouseposition) {
      clickpos.x = lastx;
      clickpos.y = lasty;
      clickbuttonx = buttonx;
    }
  } else {
    clicked = 0;
    pageing = 1;
    handlePageUpDown();
  }
  invalidate();
  return 1;
}

void ScrollBar::handleUpDown() {
  setTimer(TIMER_ID2, FIRST_DELAY);
  timer2 = 1;
  firstdelay = 1;

  checkUpDown();
}

int ScrollBar::checkUpDown() {
  if (!clicked) {
    if (timer2) {
      killTimer(TIMER_ID2);
      timer2 = 0;
      return 1;
    }
  }

  if (getMousePosition() == clickmouseposition)
    upDown(clickmouseposition);

  return 1;

}

void ScrollBar::handlePageUpDown() {

  setTimer(TIMER_ID, FIRST_DELAY);
  timer = 1;
  firstdelay = 1;

  checkPageUpDown();
}

int ScrollBar::checkPageUpDown() {

  if (!pageing) {
    if (timer) {
      killTimer(TIMER_ID);
      timer = 0;
      pageway = PAGE_NONE;
      return 1;
    }
  }

  POINT pt={lastx,lasty};
  RECT c;
  getClientRect(&c);
  pt.x -= c.left;
  pt.y -= c.top;
  
  if (!vertical) {
    int middlebutton = shiftleft + buttonx + bmpbutton.getWidth()/2;
    Region *r = buttonrgn->clone();
    r->offset(buttonx+shiftleft, 0);
    if (pt.x > middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_DOWN)
      pageUp();
    if (pt.x < middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_UP)
      pageDown();
    delete r;
  } else {
    int middlebutton = shiftleft + buttonx + bmpbutton.getHeight()/2;
    Region *r = buttonrgn->clone();
    r->offset(0, buttonx+shiftleft);
    if (pt.y > middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_DOWN)
      pageUp();
    if (pt.y < middlebutton && !r->ptInRegion(&pt) && pageway != PAGE_UP)
      pageDown();
    delete r;
  }
  return 1;

}

int ScrollBar::onLeftButtonUp(int x, int y) {
  clicked = 0;
  clickmouseposition = POS_NONE;
  curmouseposition = POS_NONE;
  onMouseMove(x,y);
  if (pageing) {
    pageing = 0;
    checkPageUpDown();
  }
  onSetFinalPosition();
  invalidate();
  return 1;
}

int ScrollBar::onRightButtonDown(int x, int y) {
  return 1;
}

int ScrollBar::onRightButtonUp(int x, int y) {
  return 1;
}

int ScrollBar::onMouseWheelUp(int clicked, int lines) {
  return 1;
}

int ScrollBar::onMouseWheelDown(int clicked, int lines) {
  return 1;
}

int ScrollBar::onPaint(Canvas *canvas) {
  AutoSkinBitmap &thisleft = curmouseposition == POS_LEFT ? (clicked ? bmplpressed : bmplhilite) : bmpleft;
  AutoSkinBitmap &thisbutton = curmouseposition == POS_BUTTON ? (clicked ? bmpbpressed : bmpbhilite) : bmpbutton;
  AutoSkinBitmap &thisright = curmouseposition == POS_RIGHT ? (clicked ? bmprpressed : bmprhilite) : bmpright;

  if (curmouseposition != clickmouseposition && clicked) {
    thisleft = bmpleft;
    thisbutton = bmpbutton;
    thisright = bmpright;
  }

  RECT r;
  PaintBltCanvas paintcanvas;

  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  SCROLLBAR_PARENT::onPaint(canvas);

  getClientRect(&r);

  renderBaseTexture(canvas, r);

  if (!vertical) {
    RECT c;

    c.left = r.left + buttonx+shiftleft;
    c.top = r.top + 0;
    c.right = r.left + buttonx+thisbutton.getWidth()+shiftleft;
    c.bottom = r.top + getWidth();

    thisbutton.stretchToRectAlpha(canvas, &c);

    c.left = r.left;
    c.top = r.top;
    c.right = r.left + thisleft.getWidth();
    c.bottom = r.top + getWidth();

    thisleft.stretchToRectAlpha(canvas, &c);

    c.left = r.right-thisright.getWidth();
    c.top = r.top;
    c.right = r.right;
    c.bottom = r.top+getWidth();

    thisright.stretchToRectAlpha(canvas, &c);
  } else {
    RECT c;
    c.left = r.right - thisleft.getWidth();
    c.top = r.top+buttonx + shiftleft;
    c.right = r.right;
    c.bottom = r.top+buttonx+thisbutton.getHeight() + shiftleft;

    thisbutton.stretchToRectAlpha(canvas, &c);

    c.left = r.right - thisleft.getWidth();
    c.top = r.top;
    c.right = r.right;
    c.bottom = r.top+thisleft.getHeight();

    thisleft.stretchToRectAlpha(canvas, &c);

    c.left = r.right-thisright.getWidth();
    c.top = r.bottom-thisright.getHeight();
    c.right = r.right;
    c.bottom = r.bottom;

    thisright.stretchToRectAlpha(canvas, &c);
  }

  return 1;
}

int ScrollBar::getHeight() {
  return height;
}

void ScrollBar::setHeight(int newheight) {
  height = newheight;
}

int ScrollBar::onResize() {
  calcXPosition();
  invalidate();
  return 1;
}

int ScrollBar::onInit() {
  SCROLLBAR_PARENT::onInit();
  return 1;
}

void ScrollBar::setBitmaps(char *left, char *lpressed, char *lhilite,
                  char *right, char *rpressed, char *rhilite, 
                  char *button, char *bpressed, char *bhilite) {

  deleteResources();

  bmpleft = left;
  bmplpressed = lpressed;
  bmplhilite = lhilite;
  bmpright = right;
  bmprpressed = rpressed;
  bmprhilite = rhilite;
  bmpbutton = button;
  bmpbpressed = bpressed;
  bmpbhilite = bhilite;

  loadBmps();
}

void ScrollBar::loadBmps() {

  if (bmpleft) leftrgn = new Region(bmpleft);
  if (bmpbutton) buttonrgn = new Region(bmpbutton);
  if (bmpright) rightrgn = new Region(bmpright);

  calcOverlapping();
  calcXPosition();
}

void ScrollBar::setPosition(int pos) {
  setPrivatePosition(pos, FALSE);
}

void ScrollBar::setPrivatePosition(int pos, BOOL signal, BOOL smooth) {
  if (insetpos) return; // helps stupid people (like me)
  insetpos = 1;
  position = min(SCROLLBAR_FULL, pos);
  position = max(0, position);
  calcXPosition();
  if (signal) onSetPosition(smooth);
  if (isInited() && isVisible())
  invalidate();
  insetpos = 0;
}

int ScrollBar::getPosition() {
  return position;
}

int ScrollBar::onSetPosition(BOOL smooth) {
  notifyParent(UMSG_SCROLLBAR_SETPOSITION, smooth);
  return 1;
}

int ScrollBar::onSetFinalPosition() {
  notifyParent(UMSG_SCROLLBAR_SETFINALPOSITION);
  return 1;
}

void ScrollBar::calcOverlapping() {

  if (!vertical) {

    shiftleft = bmpleft.getWidth();
    if (leftrgn && buttonrgn) {
      for (int i=shiftleft;i>=0;i--) {
        Region *reg = buttonrgn->clone();
        reg->offset(i, 0);
        if (leftrgn->intersects(reg)) {
          delete reg;
          i++;
          break;
        }
        delete reg;
      }
      if (i >= 0)
        shiftleft = i;
    }

    shiftright = bmpright.getWidth();
    if (rightrgn && buttonrgn) {
      for (int i=0;i>=-shiftright;i--) {
        Region *reg = rightrgn->clone();
        reg->offset(i+bmpbutton.getWidth(), 0);
        if (reg->intersects(buttonrgn)) {
          delete reg;
          i++;
          break;
        }
        delete reg;
      }
      if (i >= -shiftright)
        shiftright += i;
    }

  } else {

    shiftleft = bmpleft.getHeight();
    if (leftrgn && buttonrgn) {
      for (int i=shiftleft;i>=0;i--) {
        Region *reg = buttonrgn->clone();
        reg->offset(0, i);
        if (leftrgn->intersects(reg)) {
          delete reg;
          i++;
          break;
        }
        delete reg;
      }
      if (i >= 0)
        shiftleft = i;
    }

    shiftright = bmpright.getHeight();
    if (rightrgn && buttonrgn) {
      for (int i=0;i>=-shiftright;i--) {
        Region *reg = rightrgn->clone();
        reg->offset(0, i+bmpbutton.getHeight());
        if (reg->intersects(buttonrgn)) {
          delete reg;
          i++;
          break;
        }
        delete reg;
      }
      if (i >= -shiftright)
        shiftright += i;
    }

  }

}

void ScrollBar::calcXPosition() {

  if (!isInited()) return;

  RECT r;
  getClientRect(&r);

  int maxwidth;

  if (!vertical)
    maxwidth = (r.right-r.left)-(bmpbutton.getWidth()+shiftleft+shiftright)+1;
  else
    maxwidth = (r.bottom-r.top)-(bmpbutton.getHeight()+shiftleft+shiftright)+1;
  int oldx = buttonx;
  buttonx = (int)(((float)getPosition() / SCROLLBAR_FULL) * maxwidth);
  if (buttonx != oldx)
    invalidate();
}

void ScrollBar::calcPosition() {

  if (!isInited()) return;

  RECT r;
  getClientRect(&r);

  int maxwidth;

  if (!vertical)
    maxwidth = r.right-r.left-(bmpbutton.getWidth()+shiftleft+shiftright)+1;
  else
    maxwidth = r.bottom-r.top-(bmpbutton.getHeight()+shiftleft+shiftright)+1;
  setPrivatePosition((int)((float)buttonx / maxwidth * SCROLLBAR_FULL));
  //invalidate();
}

void ScrollBar::timerCallback(int id) {
  switch (id) {
    case TIMER_ID:
      if (firstdelay) {
        killTimer(TIMER_ID);
        setTimer(TIMER_ID, NEXT_DELAY);
        timer = 1;
        firstdelay = 0;
      }
      checkPageUpDown();
      break;
    case TIMER_ID2:
      if (firstdelay) {
        killTimer(TIMER_ID2);
        setTimer(TIMER_ID2, NEXT_DELAY);
        timer2 = 1;
        firstdelay = 0;
      }
      checkUpDown();
      break;
    default:
      SCROLLBAR_PARENT::timerCallback(id);
  }
}

int ScrollBar::pageUp() { 

  pageway = PAGE_UP;

  setPrivatePosition((int)max(0, (float)getPosition() + (float)SCROLLBAR_FULL / (npages-1)), TRUE, TRUE);

  return 1; 
};

int ScrollBar::pageDown() { 

  pageway = PAGE_DOWN;

  setPrivatePosition((int)min(SCROLLBAR_FULL, (float)getPosition() - (float)SCROLLBAR_FULL / (npages-1)), TRUE, TRUE);

  return 1; 
};

void ScrollBar::setNPages(int n) {
  //ASSERT(n >= 2);
  if (n < 2) n = 2;
  npages = n;
}

void ScrollBar::gotoPage(int page) {

  page = min(page, npages-1);
  page = max(page, 0);

  setPrivatePosition((int)((float)SCROLLBAR_FULL / (npages-1) * page), TRUE, FALSE);

}

void ScrollBar::setUpDownValue(int newupdown) {
  updown = newupdown;
}

int ScrollBar::upDown(int which) {
  switch (which) {
    case POS_LEFT:
      setPrivatePosition(getPosition()-updown);
      break;
    case POS_RIGHT:
      setPrivatePosition(getPosition()+updown);
      break;
  }
  return 1;
}

void ScrollBar::setVertical(int isvertical) {
  vertical = isvertical;
  calcOverlapping();
  if (isInited())
    invalidate();
}

