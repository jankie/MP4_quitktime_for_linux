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

#include "tabsheet.h"
#include "notifmsg.h"

#include "std.h"

#include "../studio/api.h"

#define DEFAULT_TAB_ROW_HEIGHT 20

TabSheet::TabSheet() {
  tabrowheight = DEFAULT_TAB_ROW_HEIGHT;
  tabrowwidth = 0;
  leftscroll = rightscroll = NULL;
  background = NULL;
  tabrowmargin = 0;
  active = NULL;
}

TabSheet::~TabSheet() {
  int nchildren;
  ASSERT(tabs.getNumItems() == children.getNumItems());
  if ((nchildren = children.getNumItems()) != 0) {
    for (int i = 0; i < nchildren; i++) {
      ASSERT(children[i] != NULL);
      delete children[i];
      ASSERT(tabs[i] != NULL);
      delete tabs[i];
      if (tips[i]!=(void *)-1)
        FREE(tips[i]);
    }
  }
  delete leftscroll;
  delete rightscroll;
  delete background;
}

int TabSheet::onInit() {
  TABSHEET_PARENT::onInit();
  int i, nchildren;

  if (leftscroll != NULL) {
    leftscroll->init(this);
    leftscroll->setParent(this);
  }
  if (rightscroll != NULL) {
    rightscroll->init(this);
    rightscroll->setParent(this);
  }

  nchildren = children.getNumItems();
//  ASSERT(nchildren > 0);
  ASSERT(tabs.getNumItems() == 0);
  for (i = 0; i < nchildren; i++) {
    int r = children[i]->init(this);
    ASSERT(r);
    children[i]->setParent(this);
    TabButton *tb = tabs.addItem(new TabButton());
    r = tb->init(this);
    if (tips[i]!=(void *)-1)
      tb->setTip(tips[i]);
    ASSERT(r);
    tb->setParent(this);
    tb->linkWnd(children[i]);

    if (instances[i] != NULL) {
      tb->setBitmaps(instances[i], normal[i], pushed[i], hilite[i], -1);
    } else {
      tb->setButtonText(children[i]->getName());
    }

    children[i]->setVisible(i == 0);
		tabs[i]->setVisible(showed.enumItem(i));
  }
  if (nchildren) {
    active = children[0];
    tabs[0]->setHilite(TRUE);
  }
  ASSERT(tabs.getNumItems() == children.getNumItems());
  ASSERT(tabs.getNumItems() == nchildren);
  onResize();
  return 1;
}

void TabSheet::getClientRect(RECT *r) {
  TABSHEET_PARENT::getClientRect(r);
  r->top += tabrowheight;
}

void TabSheet::setBackgroundBmp(const char *name) {
  if (background) delete background;
  background = NULL;
  if (name && *name)
    background = new SkinBitmap(name);
}

SkinBitmap *TabSheet::getBackgroundBitmap() {
  return background;
}

int TabSheet::onPaint(Canvas *canvas) {

  PaintBltCanvas paintcanvas;
  if (canvas == NULL) {
    if (!paintcanvas.beginPaintNC(this)) return 0;
    canvas = &paintcanvas;
  }
  TABSHEET_PARENT::onPaint(canvas);
 
  RECT r;
  TABSHEET_PARENT::getClientRect(&r);

  r.bottom = r.top + tabrowheight;

  // FUCKO: be smarter about asking buttons if their bitmaps have alpha
//  api->skin_renderBaseTexture(canvas, r);

  RECT br = r;
  if (leftscroll) br.left += leftscroll->getWidth();
  if (rightscroll) br.right -= rightscroll->getWidth();

  if (br.right <= br.left) return 1;

  if (background != NULL) {
#if 0
    int i, x = tilex;
    for (i = 0; ; i++) {
      tile->stretch(canvas, x, 0, tile->getWidth(), tabrowheight);
      x += tile->getWidth();
      if (x >= r.right) break;
    }
#else
#if 0
    if (background->getAlpha()) api->skin_renderBaseTexture(canvas, br);
    background->stretchToRectAlpha(canvas, &br);
#else
    background->stretchToRect(canvas, &br);
#endif
#endif
  } else {
#if 0
    r.top = 0;
    r.bottom = tabrowheight;
    r.left = tilex;
    r.right = tilex + tilew;
    canvas->fillRect(&r, RGB(64, 64, 64));
#else
//    api->skin_renderBaseTexture(canvas, r);
#endif
  }

  return 1;
}

int TabSheet::childNotify(RootWnd *which, int msg, int param1, int param2) {
  if (which == leftscroll && msg == CHILD_NOTIFY_LEFTPUSH) {
  }
  if (which == rightscroll && msg == CHILD_NOTIFY_LEFTPUSH) {
  }
  return 0;
}

void TabSheet::setTabRowHeight(int newheight) {
  ASSERT(newheight > 0);
  tabrowheight = newheight;
  onResize();
}

void TabSheet::setTabRowMargin(int newmargin) {
  ASSERT(newmargin >= 0);
  tabrowmargin = newmargin;
  onResize();
}

int TabSheet::addChild(BaseWnd *newchild, HINSTANCE hInst, int _normal, int _pushed, int _hilite) {
return addChild(newchild, hInst, _normal, _pushed, _hilite, NULL);
}

int TabSheet::addChild(BaseWnd *newchild, HINSTANCE hInst, int _normal, int _pushed, int _hilite, const char *_tip) {
  children.addItem(newchild);
  instances.addItem(hInst);
  normal.addItem(_normal);
  pushed.addItem(_pushed);
  hilite.addItem(_hilite);
  showed.addItem(TRUE);
  tips.addItem(_tip ? STRDUP(_tip) : (char *)-1);

  return children.getNumItems()-1;
}

int TabSheet::setButtonVisible(int i, BOOL state) {
  showed.setItem(i, state);
  if (!state && tabs.getNumItems() >= i+1)
    tabs[i]->setVisible(state);
  invalidate();
  return 1;
}

int TabSheet::addChild(BaseWnd *newchild) {
  return addChild(newchild, NULL, -1, -1, -1);
}

int TabSheet::addChild(BaseWnd *newchild, const char *tip) {
  return addChild(newchild, NULL, -1, -1, -1, tip);
}

void TabSheet::activateChild(BaseWnd *newactive) {
  BaseWnd *prevactive = active;

  if (prevactive == newactive) return;	// not a switch

  RECT r;
  getClientRect(&r);

#if 0
  int w = r.right - r.left + 1;
  int h = r.bottom - r.top + 1;
#endif

  int prevpos=-1, nextpos=-1;

  for (int i = 0; i < children.getNumItems(); i++) {
    if (prevactive == children[i]) prevpos = i;
    if (newactive == children[i]) nextpos = i;
  }

  tabs[prevpos]->setHilite(FALSE);
  tabs[nextpos]->setHilite(TRUE);

#if 0
  // reveal tha new winder
  newactive->setVisible(TRUE);

  enable(FALSE);
  prevactive->enable(FALSE);
  newactive->enable(FALSE);

#define STEPS 6

  // find which window is now active
  for (int c = 0; c < STEPS; c++) {
    int x;
    if (prevpos > nextpos) x = (w * c) / STEPS;	// right to left
    else x = (w * (STEPS - c)) / STEPS;		// left to right
    int y = r.top;
    if (prevpos > nextpos) {
      newactive->move(x - w, y);
      prevactive->move(x, y);
    } else {
      newactive->move(x, y);
      prevactive->move(x - w, y);
    }
    newactive->repaint();
    prevactive->repaint();
    Sleep(15);
  }
#endif

  newactive->setVisible(TRUE);
  newactive->move(r.left, r.top);

  prevactive->setVisible(FALSE);
  prevactive->move(r.left, r.top);

  enable(TRUE);
  prevactive->enable(TRUE);
  newactive->enable(TRUE);

  active = newactive;
}

int TabSheet::onResize() {
  TABSHEET_PARENT::onResize();

  int i, x=0;

  if (!isInited()) return 1;

  RECT r;
  getClientRect(&r);

  tilew = r.right - r.left + 1;

  tilex = 0;

  if (leftscroll != NULL) {
    leftscroll->resize(x, 0, leftscroll->getWidth(), tabrowheight);
    x += leftscroll->getWidth();
    tilew -= leftscroll->getWidth();
    tilex = x;
  }

  if (rightscroll != NULL) {
    rightscroll->resize(r.right - rightscroll->getWidth(), 0, rightscroll->getWidth(), tabrowheight);
    tilew -= rightscroll->getWidth();
  }

  if (children.getNumItems() == 0 || tabs.getNumItems() == 0) return 1;
  for (i = 0; i < children.getNumItems(); i++) {
    ASSERT(tabs[i] != NULL);
		if (!showed.enumItem(i)) continue;
    int tabwidth = tabs[i]->getWidth();
    if (tabwidth < 0) tabwidth = 128;
    int h = (tabrowheight - tabs[i]->getHeight())/2;
    tabs[i]->resize(x, h, tabwidth, tabs[i]->getHeight());
    x += tabwidth + tabrowmargin;
//    tilew -= tabwidth + tabrowmargin;
    children[i]->resize(&r);
  }
  tabrowwidth = x;

  invalidate();
  if (leftscroll)
    leftscroll->invalidate();
  if (rightscroll)
    rightscroll->invalidate();
//  repaint();

  return 1;
}

BaseWnd *TabSheet::enumChild(int child) {
  if (child < children.getNumItems())
    return children[child];
  else
    return NULL;
}

int TabSheet::getNumChild() {
  return children.getNumItems();
}

int TabSheet::getNumButton() {
  return children.getNumItems();
}

TabButton *TabSheet::enumButton(int i) {
  if (i < tabs.getNumItems())
    return tabs[i];
  else
    return NULL;
}


// TabButton

TabButton::TabButton() {
  linked = NULL;
  setFolderStyle(TRUE);
}

void TabButton::onLeftPush(int x, int y) {
  TabSheet *par;
  par = (TabSheet *)getParent();
  ASSERT(par != NULL);
  ASSERT(linked != NULL);
  par->activateChild(linked);
}

void TabButton::linkWnd(BaseWnd *control) {
  ASSERT(control != NULL);
  linked = control;
}
