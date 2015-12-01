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


#include "framewnd.h"
#include "notifmsg.h"
#include "../studio/assert.h"
#include "../studio/api.h"
#include "canvas.h"

#define SNAP 4

FrameWnd::FrameWnd() {
//  sizer = NULL;
  nchild = 0;
  for (int i = 0; i < MAXCHILD; i++) {
    children[i] = NULL;
    hidey[i] = 0;
    windowshaded[i] = 0;
  }
  vert = DIVIDER_UNDEFINED;
  divideside = SDP_FROMLEFT;
  pullbarpos = PULLBAR_HALF;
  minwidth = PULLBAR_QUARTER-PULLBAR_EIGHTH;
  maxwidth = PULLBAR_HALF;
  resizeable = 0;
  slidemode = FRAMEWND_SQUISH;
  prevpullbarpos = -1;

  MEMSET(&sizerRect, 0, sizeof(RECT));

  h_bitmap = "studio.framewnd.horizontaldivider";
  v_bitmap = "studio.framewnd.verticaldivider";
  h_grabber = "studio.framewnd.horizontalgrabber";
  v_grabber = "studio.framewnd.verticalgrabber";
  ws_bitmap = "studio.framewnd.windowshade";
}

FrameWnd::~FrameWnd() {
  if (getName() != NULL) {
    StringPrintf buf("FrameWnd/ws,%s", getName());
    api->setIntPrivate(buf, windowshaded[0]);
  }
  setName(NULL);
  for (int i = 0; i < nchild; i++) delete children[i];
}

int FrameWnd::onInit() {
  int i;

  FRAMEWND_PARENT::onInit();

  ASSERT(vert != DIVIDER_UNDEFINED || nchild == 0);

  // have to set children for frame windows

  // fill in members
  nchild = 0;

  // make children create their windows
  for (i = 0; i < MAXCHILD; i++) {
    if (children[i] != NULL) {
      if (children[i]->init(this) != 0) {
        children[i]->setParent(this);
        nchild++;
      }
    }
  }
  prevpullbarpos = pullbarpos;
  // size the fuckers
  onResize();

  if (nchild >= MAXCHILD) {
    int which = (divideside == SDP_FROMLEFT) ? 0 : 1;
    children[which]->moveToFront();
  }

  if (getName() != NULL) {
    StringPrintf buf("FrameWnd/ws,%s", getName());
    int ws = api->getIntPrivate(buf, /*children[0] && children[0]->childNotify(NULL, CHILD_WINDOWSHADE_CAPABLE)*/ 0);
    if (ws) {
      windowshade(0, !ws);
      windowshade(0, ws);
      pullbarpos = 0;
    }
  }

  return 1;
}

int FrameWnd::getCursorType(int x, int y) {
  if (y < getLabelHeight()) return BASEWND_CURSOR_POINTER;
  if (vert == DIVIDER_HORIZONTAL) return BASEWND_CURSOR_NORTHSOUTH;
  else return BASEWND_CURSOR_EASTWEST;
}

int FrameWnd::setChildren(BaseWnd *newchild1, BaseWnd *newchild2) {
  children[0] = newchild1;
  children[1] = newchild2;
  nchild = 0;
  if (children[0] != NULL) nchild++;
  if (children[1] != NULL) nchild++;

  ASSERTPR(nchild >= 1, "framewnd must have one or more children");

  onResize();
  return nchild;
}

BaseWnd *FrameWnd::enumChild(int which) {
  if (which < 0 || which >= MAXCHILD) return NULL;
  return children[which];
}

int FrameWnd::childNotify(RootWnd *which, int msg, int param1, int param2) {
//  ASSERT(which == children[0] || which == children[1] || which == NULL);
  switch (msg) {
    case CHILD_SETTITLEWIDTH:
      if (pullbarpos == param1) return 0;
//CUT      ASSERTPR(dividepostype == SDP_INPIXELS, "right now child windows can only set titlewidths if its an inpixels frame");
      ASSERT(param1 >= 0);
        if (which == children[0]) {
          // children[1]->invalidate(); //FG> removed due to change in redraw layout
          // children[1]->repaint();
          ASSERT(divideside == SDP_FROMLEFT);
        } else {
          // children[0]->invalidate();
          // children[0]->repaint();
          ASSERT(divideside == SDP_FROMRIGHT);
        }
      pullbarpos = param1;
      // do it
      onResize();
    return 1;

    case CHILD_HIDEYHIDEY:
      if (which == children[0]) hidey[0] = 1;
      else if (which == children[1]) hidey[1] = 1;
      which->setVisible(FALSE);
      onResize();
    return 1;

    case CHILD_UNHIDEYHIDEY:
      if (which == children[0]) hidey[0] = 0;
      else if (which == children[1]) hidey[1] = 0;
      which->setVisible(TRUE);
      onResize();
    return 1;

    case FRAMEWND_QUERY_SLIDE_MODE:
      return getSlideMode();

    case FRAMEWND_SET_SLIDE_MODE:
      setSlideMode((FrameWndSlideMode)param1);
    break;
    case CHILD_GOTFOCUS:
    case CHILD_KILLFOCUS:
      invalidateLabel();
    break;
  }

  return FRAMEWND_PARENT::childNotify(which, msg, param1, param2);
}

int FrameWnd::forceFocus() {
  if (!canShowFocus()) return 0;	// we aren't showing a label
  int v = 0;
  if (nchild > 0 && children[0] != NULL) {
    if (!children[0]->canShowFocus()) v |= children[0]->gotFocus();
  }
  if (nchild > 1 && children[1] != NULL) {
    if (!children[1]->canShowFocus()) v |= children[1]->gotFocus();
  }
  return v;
}

void FrameWnd::setDividerType(FrameWndDividerType type) {
  vert = type;
  ASSERT(vert == DIVIDER_VERTICAL || vert == DIVIDER_HORIZONTAL);
  onResize();
}

FrameWndDividerType FrameWnd::getDividerType() {
  return vert;
}

int FrameWnd::ConvertPixToProp() {
  RECT r;
  int w;
  getClientRect(&r);
  if(vert == DIVIDER_VERTICAL) {
    w = r.right-r.left;
  } else {
    w = r.bottom-r.top;
  }
  w = (pullbarpos * PULLBAR_FULL) / w;
  return w;
}

int FrameWnd::convertPropToPix(int prop) {
  RECT r;
  int w;

  getClientRect(&r);
  if(vert == DIVIDER_VERTICAL) {
    w = r.right-r.left;
  } else {
    w = r.bottom-r.top;
  }
  return (w * prop) / PULLBAR_FULL;
}

int FrameWnd::setDividerPosNoCfg(int from, int pos) {
  divideside = from;

  ASSERT(pos >= 0);
  pullbarpos = pos;
  onResize();
  return 1;
}

int FrameWnd::setDividerPos(int from, int pos) {
  if (getName() != NULL) {
    StringPrintf buf("FrameWnd/%s,p", getName());
    pos = api->getIntPrivate(buf, pos);
    if (pos <= 0) pos = 0;
    else if (pos >= PULLBAR_FULL) pos = PULLBAR_FULL;
  }
  return setDividerPosNoCfg(from, pos);
}

void FrameWnd::getDividerPos(int *from, int *pos) {
  if (from != NULL) *from = divideside;
  if (pos != NULL) *pos = pullbarpos;
}

int FrameWnd::setResizeable(int is) {
  int prev = resizeable;
  resizeable = is;
  return prev;
}

void FrameWnd::setMinWidth(int min) {
  ASSERT(min >= 0);
  minwidth = min;
}

void FrameWnd::setSlideMode(FrameWndSlideMode mode) {
  slidemode = mode;
  onResize();
}

FrameWndSlideMode FrameWnd::getSlideMode() {
  return slidemode;
}

int FrameWnd::dragEnter(RootWnd *sourceWnd) {
  BaseWnd *ch = getWindowShadedChild();
  if (ch == NULL) return FRAMEWND_PARENT::dragEnter(sourceWnd);
  return ch->dragEnter(sourceWnd);
}

int FrameWnd::dragOver(int x, int y, RootWnd *sourceWnd) {
  BaseWnd *ch = getWindowShadedChild();
  if (ch == NULL) return FRAMEWND_PARENT::dragOver(x, y, sourceWnd);
  return ch->dragOver(-1, -1, sourceWnd);
}

int FrameWnd::dragLeave(RootWnd *sourceWnd) {
  BaseWnd *ch = getWindowShadedChild();
  if (ch == NULL) return FRAMEWND_PARENT::dragLeave(sourceWnd);
  return ch->dragLeave(sourceWnd);
}

int FrameWnd::dragDrop(RootWnd *sourceWnd, int x, int y) {
  BaseWnd *ch = getWindowShadedChild();
  if (ch == NULL) return FRAMEWND_PARENT::dragDrop(sourceWnd, x, y);
  return ch->dragDrop(sourceWnd, x, y);
}

int FrameWnd::onResize() {
  FRAMEWND_PARENT::onResize();
  RECT r;
  int sizerwidth = SIZERWIDTH;

  if (!isInited()) {
    prevpullbarpos = pullbarpos;
    return 1;	// no window to resize
  }

  getClientRect(&r);

  ASSERT(nchild >= 0);
  if (nchild == 0) {
    prevpullbarpos = pullbarpos;
    return 1;
  }

  if (hidey[0] && hidey[1]) return 0;	// both windows are hiding

  // if we have only one child, it takes up all the room
  if (hidey[0]) {
    children[1]->resize(&r);
    return 1;
  } else if (hidey[1]) {
    children[0]->resize(&r);
    return 1;
  }

  if (nchild == 1) {
    if (children[0] != NULL) children[0]->resize(&r);
    else if (children[1] != NULL) children[1]->resize(&r);
    return 1;
  }

#ifdef ASSERTS_ENABLED
  for (int i = 0; i < nchild; i++) {
    ASSERT(children[i] != NULL);
  }
#endif

  if (!resizeable) sizerwidth = 0;

  // resize the subwindows

  int w, h;
  if (vert == DIVIDER_VERTICAL) {
    w = r.right-r.left;
    h = r.bottom-r.top;
  } else {
    w = r.bottom-r.top;
    h = r.bottom-r.top;
  }
  int clientwidth = w;	// the logical width

  w = pullbarpos;

  if (divideside == SDP_FROMRIGHT) {
    w = (clientwidth - w);
  }

  RECT r1, r2;

  if (slidemode == FRAMEWND_COVER) { // cover mode

    ASSERTPR(vert == DIVIDER_VERTICAL, "finish implementing");

    if (divideside == SDP_FROMRIGHT) {
      SetRect(&r1, r.left, r.top, r.right-r.left, r.bottom-r.top); //FG> delay resize
      SetRect(&r2, r.left+w, r.top, r.left+clientwidth - w, r.bottom-r.top);
    } else {
      SetRect(&r1, r.left, r.top, r.left+w, r.bottom-r.top); //FG> delay resize
      SetRect(&r2, r.left, r.top, r.right-r.left, r.bottom-r.top);
    }

    sizerRect.top = r.top;
    sizerRect.bottom = r.bottom;
    sizerRect.left = w;
    sizerRect.right = w+sizerwidth;

  } else { // squish mode
    // left-right
    if (vert == DIVIDER_VERTICAL) {
      sizerRect.top = r.top;
      sizerRect.bottom = r.bottom;
      if (divideside == SDP_FROMLEFT) { // from left
        SetRect(&r1, r.left, r.top, r.left+w, r.bottom-r.top);
        SetRect(&r2, w+sizerwidth, r.top, (r.right-r.left)-(w+sizerwidth), r.bottom-r.top);
        sizerRect.left = w;
        sizerRect.right = sizerRect.left + sizerwidth;
      } else {	// from right
        SetRect(&r1, r.left, r.top, (r.left+w)-sizerwidth, r.bottom-r.top);
        SetRect(&r2, w, r.top, (r.right-r.left)-w, r.bottom-r.top);
        sizerRect.left = w-sizerwidth;
        sizerRect.right = w;
      }
    } else {
      // top-bottom
      SetRect(&r1, r.left, r.top, r.right, r.top+w); //FG> delay resize
      SetRect(&r2, r.left, r.top+w+sizerwidth, r.right-r.left, (r.bottom-r.top)-(w+sizerwidth));
      sizerRect.top = w;
      sizerRect.bottom = w+sizerwidth;
      sizerRect.left = r.left;
      sizerRect.right = r.right;
    }
  }

  //FG> Choose resizing order. optimizes redraw by avoiding temporary overlap of children
  BOOL reverse = FALSE;
  if (vert == DIVIDER_VERTICAL) {
    RECT o;
    children[0]->getNonClientRect(&o);
    reverse = (r1.right > o.right);
  } else {
    RECT o;
    children[0]->getNonClientRect(&o);
    reverse = (r1.bottom > o.bottom);    
  }

  //FG> actually resize children
  if (reverse) {
    children[1]->resize(r2.left, r2.top, r2.right, r2.bottom);
    children[0]->resize(r1.left, r1.top, r1.right, r1.bottom);
  } else {
    children[0]->resize(r1.left, r1.top, r1.right, r1.bottom);
    children[1]->resize(r2.left, r2.top, r2.right, r2.bottom);
  }

  RECT ri = sizerRect;
#if 0
  if (vert == DIVIDER_HORIZONTAL) {
    ri.left -= 2;
    ri.right += 2;
  } else {
    ri.top -= 2;
    ri.bottom += 2;
  }
#endif
  invalidateRect(&ri);

  prevpullbarpos = pullbarpos;

  return 1;
}

int FrameWnd::onPaint(Canvas *canvas) {

  RECT d;
  getClientRect(&d);
  if ((d.left >= d.right) || (d.top >= d.bottom)) {
    return FRAMEWND_PARENT::onPaint(canvas);
  }

  RECT cr;
//  PaintBltCanvas paintcanvas;
  PaintCanvas paintcanvas;

  // if only 1 child, we don't paint anything
  if (nchild <= 1) return FRAMEWND_PARENT::onPaint(canvas);

  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  FRAMEWND_PARENT::onPaint(canvas);

  getClientRect(&cr);

  if (resizeable) {
    RECT r = sizerRect;
    if (vert == DIVIDER_HORIZONTAL) {
      r.left -= 2;
      r.right += 2;
    } else {
      r.top -= 2;
      r.bottom += 2;
    }

    renderBaseTexture(canvas, r);
    AutoSkinBitmap &bitmap = (vert == DIVIDER_VERTICAL) ? v_bitmap : h_bitmap;
    bitmap.stretchToRectAlpha(canvas, &r);

    AutoSkinBitmap &grabber = (vert == DIVIDER_VERTICAL) ? v_grabber : h_grabber;
    int h = sizerRect.bottom - sizerRect.top;
    int gh = grabber.getHeight();
    if (h > gh) {
      RECT rr = sizerRect;
      rr.top += (h - gh) / 2;
      rr.bottom -= (h - gh) / 2;
      grabber.stretchToRectAlpha(canvas, &rr);
    }

    if (windowshaded[0]) {
      RECT wr = cr;
      if (vert == DIVIDER_VERTICAL) {
        wr.right = r.left;
      } else if (vert == DIVIDER_HORIZONTAL) {
        wr.bottom = r.top;
      }

      ws_bitmap.stretchToRect(canvas, &wr);
    }

  }

  return 0;
}

int FrameWnd::onLeftButtonDown(int x, int y) {
  FRAMEWND_PARENT::onLeftButtonDown(x, y);
  if (!resizeable) return 1;
  POINT p = { x, y };
  if (Std::pointInRect(&sizerRect, p)) {
    beginCapture();
    return 1;
  }
  return 0;
}

int FrameWnd::onMouseMove(int x, int y) {
  int pos, mpos;
  RECT r;

  if (!getCapture()) return 1;

  FRAMEWND_PARENT::onMouseMove(x,y);

  prevpullbarpos = pullbarpos;
  
  getClientRect(&r);
  if (vert == DIVIDER_VERTICAL) {
    pos = r.right - r.left;
    mpos = x - (x % SNAP);
  } else {
    pos = r.bottom - r.top;
    mpos = y - (y % SNAP);
  }
  ASSERT(pos != 0);
  if (mpos < 0) mpos = 0;
  if (mpos > pos) mpos = pos;

  if(divideside == SDP_FROMLEFT) {
    pullbarpos = mpos;
  } else {
    pullbarpos = pos-mpos;
  }

  if (divideside == SDP_FROMLEFT) {
    if (pullbarpos < convertPropToPix(minwidth)) {
      if (children[0] != NULL && children[0]->childNotify(NULL, CHILD_WINDOWSHADE_CAPABLE)) {
        pullbarpos = 0;
        windowshade(0, TRUE);
      } else {
        pullbarpos = convertPropToPix(minwidth);
      }
    } else {
      windowshade(0, FALSE);
    }
  } else if (divideside == SDP_FROMRIGHT) {
    if (pullbarpos < convertPropToPix(minwidth)) {
      if (children[1] != NULL /* && children[1]->childNotify(NULL, CHILD_WINDOWSHADE_CAPABLE) */) {
        pullbarpos = /*convertPropToPix(PULLBAR_FULL)-*/0;
        windowshade(1, TRUE);
      } else {
        pullbarpos = convertPropToPix(PULLBAR_FULL-PULLBAR_EIGHTH);
      }
    } else {
      windowshade(1, FALSE);
    }
  }

  if (!windowshaded[0] && !windowshaded[1]) {
//    if (pullbarpos > pos-convertPropToPix(minwidth))
//      pullbarpos = pos-convertPropToPix(minwidth);
      if (pullbarpos > convertPropToPix(maxwidth))
        pullbarpos = convertPropToPix(maxwidth);
  }

  ASSERT(pullbarpos >= 0);

  if (pullbarpos != prevpullbarpos)
    onResize();

  return 1;
}

int FrameWnd::onLeftButtonUp(int x, int y) {
  FRAMEWND_PARENT::onLeftButtonUp(x, y);
  if (getCapture()) {
    endCapture();
    if (getName() != NULL) {
      StringPrintf buf("FrameWnd/%s,p", getName());
      api->setIntPrivate(buf, pullbarpos);
    }
    return 1;
  }
  return 0;
}

int FrameWnd::onLeftButtonDblClk(int x, int y) {
  screenToClient(&x, &y);
  if (windowshaded[0]) {
    if (vert == DIVIDER_HORIZONTAL) {
      if (y < sizerRect.bottom) {
        childNotify(children[0], CHILD_SETTITLEWIDTH, 128, 0);
        windowshade(0, FALSE);
      }
    } else if (vert == DIVIDER_VERTICAL) {
      if (x < sizerRect.right) {
        childNotify(children[0], CHILD_SETTITLEWIDTH, 128, 0);
        windowshade(0, FALSE);
      }
    }
  }
  return 1;
}

void FrameWnd::windowshade(int which, int shaded) {
  ASSERT(which == 0 || which == 1);
  if (!!windowshaded[which] == !!shaded) return;
  if (children[which] == NULL) return;
  children[which]->childNotify(NULL, CHILD_WINDOWSHADE_ENABLE, shaded);
  windowshaded[which] = shaded;
  children[which]->setVisible(!shaded);
}

BaseWnd *FrameWnd::getWindowShadedChild() {
  if (nchild != 2) return NULL;
  if (!(windowshaded[0] | windowshaded[1])) return NULL;
  return windowshaded[0] ? children[0] : children[1];
}

void FrameWnd::onSetVisible(int show) {
  FRAMEWND_PARENT::onSetVisible(show);
  if (children[0]) children[0]->setVisible(show);
  if (children[0]) children[0]->setVisible(show);
}
