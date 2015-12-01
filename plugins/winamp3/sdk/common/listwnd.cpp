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


#include "listwnd.h"

#include "std.h"
#include "usermsg.h"
#include "ptrlist.h"
#include "skinclr.h"
#include "notifmsg.h"

#include "../studio/assert.h"
#include "../studio/api.h"
#include "xlatstr.h"

#define DEF_TEXT_SIZE 16	// Default text size

#define X_SHIFT 2
#define Y_SHIFT 2
#define DRAG_THRESHOLD 4
#define COLUMNS_DEFAULT_HEIGHT 12
#define COLUMNS_DEFAULT_WIDTH  96
#define COLUMNS_MARGIN 2
#define COLUMNS_RESIZE_THRESHOLD 4
#define COLUMNS_MIN_WIDTH  1

static SkinColor textColor("studio.list.text");// todo: have own color in skin.cpp
static SkinColor bgcolor("studio.list.background");// todo: have own color in skin.cpp
static SkinColor color_item_selected("studio.list.item.selected"); // RGB(0, 0, 128)
static SkinColor color_item_focused("studio.list.item.focused");// RGB(0, 128, 128)
static SkinColor color_headers("studio.list.column.background");//RGB(0, 152, 208)
static SkinColor columnTextColor("studio.list.column.text");

typedef struct {
  char *label;
  int column;
} listSubitemStruct;

class listItem {
friend ListWnd;
public:
  ListWnd *getList() const { return listwnd; }
protected:
  listItem() {
    data = 0;
    subitems = NULL;
  }
  ~listItem() {
    if (subitems != NULL) {
      for (int i=0;i<subitems->getNumItems();i++) {
        listSubitemStruct *subitem = subitems->enumItem(i);
        if (subitem->label)
          FREE(subitem->label);
      }
      subitems->freeAll();
      delete subitems;
    }
  }
  void setList(ListWnd *newlist) { listwnd=newlist; }

  RecycleString label;
  LPARAM data;
  PtrList<listSubitemStruct> *subitems;
  ListWnd *listwnd;
};

class CompareListItem {
public:
  static int compareItem(listItem *p1, listItem *p2);
};

class SelItemList {
public:
  SelItemList(ListWnd *parent);
  ~SelItemList();

  void setSelected(int pos, int selected);
  int isSelected(int pos);
  int getNumItems() { return list.getNumItems(); }

private:
  ListWnd *listwnd;
  PtrListQuickSortedByPtrVal<listItem> list;
};

int CompareListItem::compareItem(listItem *p1, listItem *p2) {
  return p1->getList()->sortCompareItem(p1, p2);
}

SelItemList::SelItemList(ListWnd *parent) {
  listwnd = parent;
}

SelItemList::~SelItemList() {
  list.removeAll();
}

void SelItemList::setSelected(int pos, int selected) {
  ASSERT(pos >= 0 && pos < listwnd->getNumItems());
  listItem *item = listwnd->itemList[pos];
  if (selected) {
    if (list.findItem(item) == NULL) {
      list.addItem(item);
      listwnd->onItemSelection(pos, TRUE);
      listwnd->invalidateItem(pos);
    }
  } else {
    int pos;
    if (list.findItem(item, &pos) != NULL) {
      list.removeByPos(pos);
      listwnd->onItemSelection(pos, FALSE);
      listwnd->invalidateItem(pos);
    }
  }
}

int SelItemList::isSelected(int pos) {
  listItem *item = listwnd->itemList[pos];
  ASSERT(item != NULL);
  return (list.findItem(item) != NULL);
}

ListWnd::ListWnd() {
  setAutoSort(FALSE);
  setOwnerDraw(FALSE);
  setSortDirection(0);
  setSortColumn(0);
  lastcolsort=-1;

  selItemList = new SelItemList(this);
	metrics_ok = FALSE;
  setFontSize(DEF_TEXT_SIZE);
  redraw = TRUE;
  columnsHeight = COLUMNS_DEFAULT_HEIGHT;
  lastItemFocused = NULL;
  lastItemFocusedPos = -1;
  lastAddedItem = NULL;
  selectionStart = -1;
  colresize = -1;
  resizing_col = FALSE;
  processbup = FALSE;
  bdown = FALSE;
  nodrag = FALSE;
  showColumnsHeaders = TRUE;
  rectselecting=0;
}

ListWnd::~ListWnd() {
  deleteAllItems();
  columnsList.deleteAll();
  delete selItemList;
  nodrag=FALSE;
}

int ListWnd::onInit() {

  LISTWND_PARENT::onInit();

  setBgBitmap("studio.list.background");
  setLineHeight(itemHeight);

  return 1;
}

int ListWnd::onPaint(Canvas *canvas) {

  PaintCanvas paintcanvas;
  PaintBltCanvas paintbcanvas;

  if (canvas == NULL) {
    if (needDoubleBuffer()) {
      if (!paintbcanvas.beginPaintNC(this)) return 0;
      canvas = &paintbcanvas;
    } else {
      if (!paintcanvas.beginPaint(this)) return 0;
      canvas = &paintcanvas;
    }
  }

  LISTWND_PARENT::onPaint(canvas);

  Region *clip = new Region();
  canvas->getClipRgn(clip); 
  Region *orig;
  orig = clip->clone();

  drawColumnHeaders(canvas);

  if (getNumColumns() > 0 && showColumnsHeaders) {
    RECT cr;
    getClientRect(&cr);
    cr.bottom = cr.top;
    cr.top -= getColumnsHeight();
    Region *colReg = new Region(&cr);
    clip->subtract(colReg);
    canvas->selectClipRgn(clip);
    delete colReg;
  }

  RECT r;
  getNonClientRect(&r);

  int y = -getScrollY()+Y_SHIFT+r.top;
  int x = -getScrollX()+X_SHIFT;

  canvas->setTextOpaque(FALSE);
  canvas->pushTextSize(getFontSize());

  firstItemVisible = -1;
  lastItemVisible = -1;

  //drawSubItems(canvas, x, &y, items, r.top, r.bottom, 0);
  drawItems(canvas);

  canvas->popTextSize();

  canvas->selectClipRgn(orig); // reset cliping region;

  delete clip;
  delete orig;

  return 1;
}

int ListWnd::onResize() {
  LISTWND_PARENT::onResize();

  recalcHeaders();
  return 1;
}

int ListWnd::getNumItems(void) {
  return itemList.getNumItems();
}

void ListWnd::drawItems(Canvas *canvas) {

  RECT r, c;
  getClientRect(&r);

  Region *clip = new Region();
  if (!canvas->getClipRgn(clip)) {
    clip->setRect(&r);
  }

  if (GetClipBox(canvas->getHDC(), &c) == NULLREGION) {
    getClientRect(&c);
  }

/*  char t[1024];
  wsprintf(t, "%d %d %d %d\n", c.left, c.top, c.right, c.bottom);
  OutputDebugString(t);*/

  float f,l;
  calcBounds();

  int first, last;

  RECT s=c;

  s.bottom = min(s.bottom, r.bottom);
  s.top = max(s.top, getLabelHeight()+(showColumnsHeaders ? getColumnsHeight() : 0));

  f = ((float)(s.top - getLabelHeight() - (showColumnsHeaders ? getColumnsHeight() : 0) - Y_SHIFT + getScrollY())) / (float)itemHeight;
  l = ((float)(s.bottom - getLabelHeight() - (showColumnsHeaders ? getColumnsHeight() : 0) - Y_SHIFT + getScrollY())) / (float)itemHeight;
  first = (int)f;
  first = max(0,first);
  last = (int)l;
  if ((float)((int)l) != l) {
    last++;
  }

  int g=0;

  for (int i=first;i<last && i<itemList.getNumItems();i++) {
    RECT ir;
    getItemRect(i, &ir);
    int a=ir.right;
    ir.right = r.right;
    if (!clip->intersects(&ir)) 
      continue;
    ir.right=a;
    g++;

    LPARAM itemdata = getItemData(i);
    int itemselected = getItemSelected(i);
    int itemfocused = getItemFocused(i);

    onPreItemDraw(canvas, i, &ir, itemdata, itemselected, itemfocused);

    if (!ownerDraw(canvas, i, &ir, itemdata, itemselected, itemfocused)) {
      canvas->setTextOpaque(FALSE);
      canvas->pushTextColor(getTextColor(itemdata));
      canvas->pushTextSize(getFontSize());
      int x = X_SHIFT - getScrollX();
      if (getItemSelected(i))
        canvas->fillRect(&ir, getSelBgColor(itemdata));
      if (getItemFocused(i))
        canvas->fillRect(&ir, getFocusColor(itemdata));
      for (int j=0;j<columnsList.getNumItems();j++) {
        ListColumn *col = columnsList[j];
        RECT cr=ir;
        cr.left = x+X_SHIFT;
        cr.right = cr.left + col->getWidth()-X_SHIFT*2;
        canvas->textOutEllipsed(cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top, getSubitemText(i, j));
        x += col->getWidth();
      }
      canvas->popTextSize();
      canvas->popTextColor();
    }

    onPostItemDraw(canvas, i, &ir, itemdata, itemselected, itemfocused);
  }
/*
  char t[1024];
  wsprintf(t, "%d items draw\n", g);
  OutputDebugString(t);
*/
  delete clip;
}

int ListWnd::getFirstItemSelected() {
  for (int i=0;i<itemList.getNumItems();i++)
    if (getItemSelected(i))
      return i;

  return -1;      
}

void ListWnd::calcBounds() {
  lastComplete = TRUE;
  firstComplete = TRUE;
  float f,l;
  RECT r;
  getClientRect(&r);

  f = ((float)(getScrollY()-Y_SHIFT)  / itemHeight);
  l = ((float)(getScrollY()-Y_SHIFT+(r.bottom-r.top)) / itemHeight);

  firstItemVisible = (int)f;
  lastItemVisible = (int)l-1;

  if ((float)((int)l) != l) {
    lastItemVisible++;
    lastComplete = FALSE;
  }
  if ((float)((int)f) != f && f >= 0) {
    firstComplete = FALSE;
  }
}

// Draws tiled background
void ListWnd::drawBackground(Canvas *canvas) {

  LISTWND_PARENT::drawBackground(canvas);

	if (showColumnsHeaders) {
	  drawColumnHeaders(canvas);
	}
}

void ListWnd::drawColumnHeaders(Canvas *c) {

  if (columnsList.getNumItems() == 0 || !showColumnsHeaders) return;

  RECT r;
  getNonClientRect(&r);
  r.bottom = r.top+columnsHeight;
  if (renderRatioActive())
    r.left -= (int)((double)getScrollX()*getRenderRatio());
  else
    r.left-=getScrollX();

  c->fillRect(&r, color_headers);
  int x = r.left + X_SHIFT/*+ 1*/;

  c->pushTextColor(columnTextColor);
  c->setTextOpaque(FALSE);
  c->pushTextSize(getColumnsHeight());

  for (int i=0;i<columnsList.getNumItems();i++) {
    ListColumn *col = columnsList[i];
    int width = col->getWidth();
    if (i > 0) {
      c->moveTo(x, r.top);
      c->lineTo(x, r.top+columnsHeight);
    }
    RECT ch;
    ch.left = x+COLUMNS_MARGIN-((i==0)?X_SHIFT:0);
    ch.top = r.top;
    ch.right = ch.left + col->getWidth()-1-COLUMNS_MARGIN*2+((i==0)?X_SHIFT:0);
    ch.bottom = ch.top + getColumnsHeight()-1;
    col->customDrawHeader(c, &ch);
    x+=width/*+1*/;
  }
  c->moveTo(x, r.top);
  c->lineTo(x, r.top+columnsHeight);

  c->popTextSize();
  c->popTextColor();

}

int ListWnd::getHeaderHeight() {
  return (showColumnsHeaders && columnsList.getNumItems() > 0) ? getColumnsHeight() : 0;
}

// Returns the current tree width in pixels
int ListWnd::getContentsWidth() {
  return getColumnsWidth()+X_SHIFT;
}

// Returns the current tree height in pixels
int ListWnd::getContentsHeight() {
  return itemList.getNumItems()*itemHeight+Y_SHIFT*2;
}

void ListWnd::setAutoSort(int dosort) {
  autosort = dosort;
  itemList.setAutoSort(dosort);
}

void ListWnd::setOwnerDraw(int doownerdraw) {
  ownerdraw = doownerdraw;
}

int ListWnd::setFontSize(int size) {
  LISTWND_PARENT::setFontSize(size);
  if (size >= 0) textsize = size;
  SysCanvas *c = new SysCanvas();
  c->pushTextSize(getFontSize());
  itemHeight = c->getTextHeight();
  c->popTextSize();
  delete c;
  redraw = TRUE;
  metrics_ok = FALSE;
  invalidate();
  return 1;
}

int ListWnd::getFontSize() {
  return textsize+api->metrics_getDelta();
}

void ListWnd::onSetVisible(int show) {
  LISTWND_PARENT::onSetVisible(show);
  if (show) invalidate();
}

int ListWnd::fullyVisible(int pos) {
  return (((lastComplete && pos <= lastItemVisible) || (!lastComplete && pos < lastItemVisible)) && ((firstComplete && pos >= firstItemVisible) || (!firstComplete && pos > firstItemVisible)));
}

void ListWnd::ensureItemVisible(int pos) {
  if (pos >= itemList.getNumItems()) pos = itemList.getNumItems()-1;
  if (pos < 0) pos = 0;
  if (!fullyVisible(pos)) {
    RECT c;
    int y=pos*itemHeight;;
    getClientRect(&c);
    if (y + (c.bottom - c.top) <= getContentsHeight())
      scrollToY(y);
    else {
      scrollToY(getContentsHeight()-(c.bottom-c.top));
    }
  }
}

void ListWnd::scrollToItem(int pos) {
  scrollToY(Y_SHIFT+pos*itemHeight);
}

int ListWnd::getNumColumns() {
  return columnsList.getNumItems();
}

ListColumn *ListWnd::getColumn(int n) {
  return columnsList.enumItem(n);
}

int ListWnd::getColumnWidth(int c) {
  ListColumn *col = columnsList[c];
  ASSERT(col != NULL);
  return col->getWidth();
}

int ListWnd::selectAll() {

  int i;

#if 1
  if (redraw) {
    RECT r;
    getClientRect(&r);
    int first = (getScrollY() - Y_SHIFT) / itemHeight;
    int last = min((r.bottom + getScrollY() - Y_SHIFT) / itemHeight + 1, itemList.getNumItems());

    for (i=first;i<last;i++) {
      if (!selItemList->isSelected(i)) {
        invalidateItem(i);
      }
    }
  }
#endif

  for (i=0;i<itemList.getNumItems();i++) {
    selItemList->setSelected(i, TRUE);
  }

  notifyParent(CHILD_LISTWND_SELCHANGED, 0, 0);
  return 1;
}

int ListWnd::deselectAll() {

  int lif = getItemFocused();
  if (lif != -1) 
    invalidateItem(lif);
  lastItemFocused = NULL;

#if 1
  if (redraw) {
    RECT r;
    getClientRect(&r);
    int first = (getScrollY() - Y_SHIFT) / itemHeight;
    int last = min((r.bottom + getScrollY() - Y_SHIFT) / itemHeight + 1, itemList.getNumItems());

    for (int i=0;i<itemList.getNumItems();i++) {
      listItem *item = itemList[i];
      ASSERT(item != NULL);
      if (selItemList->isSelected(i)) invalidateItem(i);
      selItemList->setSelected(i, FALSE);
    }
  }
#endif
//  for (int i = 0; i < itemList.getNumItems(); i++) {
//    selItemList->setSelected(i, FALSE);
//  }

  notifyParent(CHILD_LISTWND_SELCHANGED, 0, 0);
  return 1;
}

int ListWnd::invertSelection() {
  for (int i = 0; i < itemList.getNumItems(); i++) {
    toggleSelection(i, FALSE);
  }
  return 1;
}

int ListWnd::invalidateItem(int pos) {
  RECT r;
  if (!gethWnd()) return 0;
  int rv = getItemRect(pos, &r);
  if (rv)
    invalidateRect(&r);
  return rv;
}

int ListWnd::onLeftButtonDblClk(int x, int y) {
  // check for column dblclick
  int colhit;
  if ((colhit = hitTestColumns(Std::makePoint(x, y))) >= 0) {
    return onColumnDblClick(colhit, x, y);
  }

  if (itemList.getNumItems() == 0) return 0;
  POINT p={x,y};
  int i = hitTest(p);
  if (i > -1) {
    notifyParent(CHILD_LISTWND_DBLCLK, i, 0);
    onDoubleClick(i);
    return 1;
  }
  return 0;
}

int ListWnd::onRightButtonDown(int x, int y) {
  nodrag=TRUE;
  ListWnd::onLeftButtonDown(x,y); // don't call inherited!
  return 1;
}

int ListWnd::onRightButtonUp(int x, int y) {
  nodrag=FALSE;
  ListWnd::onLeftButtonUp(x,y); // don't call inherited!
  int i = hitTest(x,y);
  if (i >= 0)
    onRightClick(i);
  onContextMenu(x,y);
  
  return 1;
}

int ListWnd::onRightClick(int itemnum) {
  return 0;
}


int ListWnd::onLeftButtonDown(int x, int y) {
  if (colresize != -1) {
    resizing_col = TRUE;
    drawXorLine(colresizept.x);
  }

  processbup = FALSE;
  bdown = TRUE;
  bdownx = x;
  bdowny = y;

  if (!resizing_col) {

    POINT p={x,y};
    int i = hitTest(p);
    if (i >= 0) {
      if (Std::keyDown(VK_SHIFT)) {
        if (getItemSelected(i))
          processbup=TRUE;
        else
          setSelectionEnd(i);
        } else 
          if (Std::keyDown(VK_CONTROL)) {
            if (getItemSelected(i))
              processbup=TRUE;
            else
              toggleSelection(i);
          } else {
            if (getItemSelected(i))
              processbup = TRUE;
            else
              setSelectionStart(i);
          } 
    } else {
      deselectAll();
      /*rectselecting=1;
      selectStart.x = x;
      selectStart.y = y;
      selectLast.x = x;
      selectLast.y = y;
      drawRect(selectStart.x, selectStart.y, x, y);
      beginCapture();*/
    }
  }

  return 1;
}

int ListWnd::onLeftButtonUp(int x, int y) {

  bdown = FALSE;

  if (resizing_col) {
    resizing_col = FALSE;
    drawXorLine(colresizept.x);
    calcNewColWidth(colresize, colresizept.x);
    return 1;
  }

  // check for column label click
  int colhit;
  if ((colhit = hitTestColumnsLabel(Std::makePoint(x, y))) >= 0) {
    return onColumnLabelClick(colhit, x, y);
  }

  POINT p={x,y};
  int i = hitTest(p);

  if (rectselecting || (processbup && !resizing_col)) {
    if (i >= 0) {
      if (Std::keyDown(VK_SHIFT)) {
        if (getItemSelected(i))
          setSelectionStart(i);
        else
          setSelectionEnd(i);
      } else {
        if (Std::keyDown(VK_CONTROL)) {
          toggleSelection(i);
        } else {
          setSelectionStart(i);
        } 
      }
    } else {
/*      if (rectselecting) {
        drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
        endCapture();
        rectselecting=0;
        selectRect(selectStart.x, selectStart.y, x, y);
      } else*/
        deselectAll();
    }
  }

  if (i >= 0) onLeftClick(i);

  return 1;
}

int ListWnd::onMouseMove(int x, int y) {
  LISTWND_PARENT::onMouseMove(x,y);

  if (!rectselecting && bdown && !resizing_col && !nodrag && (ABS(x-bdownx) >= 4 || ABS(y-bdowny) >= 4)) {
    processbup = FALSE;
    bdown = FALSE;
    int i = hitTest(x, y);
    if (i != -1) {
      onBeginDrag(i);
      return 1;
    }
  }

/*  if (rectselecting) {
    drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
    selectLast.x = x;
    selectLast.y = y;
    drawRect(selectStart.x, selectStart.y, selectLast.x, selectLast.y);
    return 1;
  }*/

  POINT p={x,y};
  if (!resizing_col) {
    int c = hitTestColumns(p, &colresizeo);
    if (c != -1) {
      if (colresize != c) {
        SetCursor(LoadCursor(NULL, IDC_SIZEWE)); // NONPORTABLE
        if (!getCapture())
          beginCapture();
        colresize = c;
        colresizept = p;
      }
    } else {
      if (getCapture()) {
        SetCursor(LoadCursor(NULL, IDC_ARROW)); // NONPORTABLE
        endCapture();
        colresize = -1;
      }
    }
  } else {
    if (p.x + getScrollX() < colresizeo + COLUMNS_MIN_WIDTH) {
      p.x = colresizeo + COLUMNS_MIN_WIDTH - getScrollX();
    }
    drawXorLine(colresizept.x);
    colresizept = p;
    drawXorLine(colresizept.x);
  }
  return 1;
}

// NONPORTABLE / Todo: implement necessary stuff in canvas
void ListWnd::drawXorLine(int x) {
  HDC dc = GetDC(gethWnd());
	HBRUSH brush = CreateSolidBrush(0xFFFFFF);
	HPEN pen = CreatePen(PS_SOLID,0,0xFFFFFF);
	HBRUSH oldB = (HBRUSH)SelectObject(dc, brush);
	HPEN oldP = (HPEN)SelectObject(dc, pen);
	int mix = SetROP2(dc,R2_XORPEN);

	RECT r;
	getNonClientRect(&r);

  if (renderRatioActive()) {
    multRatio(&x);
    multRatio(&r);
  }

  MoveToEx(dc, x, r.top, NULL);
  LineTo(dc, x, r.bottom);

	SelectObject(dc, oldB);
	SelectObject(dc, oldP);
	SetROP2(dc, mix);
	DeleteObject(pen);
	DeleteObject(brush);
	ReleaseDC(gethWnd(), dc);
}

void ListWnd::calcNewColWidth(int c, int px) {
  RECT r;
  getNonClientRect(&r);
  px += getScrollX();
  int x = X_SHIFT;
  for (int i=0;i<columnsList.getNumItems();i++) {
    ListColumn *col = columnsList[i];
    if (col->getIndex() == c) {
      int w = px - x;
      col->setWidth(w);
      setSlidersPosition();
      return;
    }
    x += col->getWidth();  
  }
  return;
}

int ListWnd::hitTestColumns(POINT p, int *origin) {
  RECT r;
  if (!showColumnsHeaders) return -1;
  int best=-1, besto, bestd=9999;
  getNonClientRect(&r);
  p.x += getScrollX();
  if (p.y > r.top && p.y < r.top + columnsHeight) {
    int x = X_SHIFT;
    for (int i=0;i<columnsList.getNumItems();i++) {
      ListColumn *col = columnsList[i];
      x += col->getWidth();  
      if (p.x > x-COLUMNS_RESIZE_THRESHOLD && p.x < x+COLUMNS_RESIZE_THRESHOLD) {
        int d = ABS(p.x-x);
        if (d < bestd) {
          bestd = d;
          besto = x - col->getWidth();
          best = col->getIndex();
        }
      }
    }
  }

  if (best != -1)
    if (origin != NULL) *origin = besto;
  return best;
}

int ListWnd::hitTestColumnsLabel(POINT p) {
  RECT r;
  if (!showColumnsHeaders) return -1;
  getNonClientRect(&r);
  p.x += getScrollX();
  if (p.y > r.top && p.y < r.top + columnsHeight) {
    int x = X_SHIFT;
    for (int i=0;i<columnsList.getNumItems();i++) {
      ListColumn *col = columnsList[i];
      if (p.x >= x && p.x <= x+col->getWidth())
        return i;
      x += col->getWidth();  
    }
  }
  return -1;
}

void ListWnd::setSelectionStart(int pos) {
  deselectAll();
  if (!selItemList->isSelected(pos)) {
    selItemList->setSelected(pos, TRUE);
    lastItemFocused = itemList[pos];
    lastItemFocusedPos = pos;
    invalidateItem(pos);
    repaint();
  }

  selectionStart = pos;
  notifyParent(CHILD_LISTWND_SELCHANGED, 0, 0);
}

void ListWnd::setSelectionEnd(int pos) {

  if (itemList.getNumItems() == 0) return;
  if (selectionStart == -1) selectionStart = 0;

  deselectAll();

  int inc = (selectionStart > pos) ? -1 : 1;
  int i = selectionStart;

  while (1) {
    if (!selItemList->isSelected(i)) {
      selItemList->setSelected(i, TRUE);
      lastItemFocused = itemList[i];
      lastItemFocusedPos = i;
      invalidateItem(i);
    }
    if (i == pos) break;
    i += inc;
  }
  notifyParent(CHILD_LISTWND_SELCHANGED, 0, 0);
}

void ListWnd::setSelected(int pos, int selected) {
  selItemList->setSelected(pos, selected);
}

void ListWnd::toggleSelection(int pos, int setfocus) {
  if (!selItemList->isSelected(pos)) {
    selItemList->setSelected(pos, TRUE);
    if (setfocus) {
      if (selItemList->getNumItems() > 1) {
        for (int i=0;i<itemList.getNumItems();i++)
          if (selItemList->isSelected(i))
            invalidateItem(i);
      }
      lastItemFocused = itemList[pos];
      lastItemFocusedPos = pos;
    }
  } else {
    selItemList->setSelected(pos, FALSE);
    if (setfocus) {
      lastItemFocused = NULL;
      lastItemFocusedPos = -1;
    }
  }

  invalidateItem(pos);
  notifyParent(CHILD_LISTWND_SELCHANGED, 0, 0);
}

int ListWnd::onBeginDrag(int iItem) {
  // nothing by default
  lastItemFocused = NULL;
  lastItemFocusedPos = -1;
  invalidateItem(iItem);
  return 0;
}

void ListWnd::onSelectAll() {
}

void ListWnd::onDelete() {
  // do nothing by default
}

void ListWnd::onDoubleClick(int itemnum) {
  // do nothing by default
}

void ListWnd::onLeftClick(int itemnum) {
  // do nothing by default
}

void ListWnd::onSecondLeftClick(int itemnum) {
  // do nothing by default
}

int ListWnd::scrollAbsolute(int x) {
  scrollToX(x);
  return getScrollX();
}

int ListWnd::scrollRelative(int x) {
  scrollToX(getScrollX() + x);
  return getScrollX();
}

int ListWnd::getItemFocused() {
  if (lastItemFocused == NULL) return -1;
  if (itemList[lastItemFocusedPos] == lastItemFocused)
    return lastItemFocusedPos;
  else {
    lastItemFocusedPos = itemList.searchItem(lastItemFocused);
    return lastItemFocusedPos;
  }
}

int ListWnd::getItemRect(int pos, RECT *r) {
  if (pos < 0 || pos >= itemList.getNumItems()) return 0;
  RECT cr;
  getClientRect(&cr);
  r->left = -getScrollX() + X_SHIFT + cr.left;
  r->right = cr.left + getColumnsWidth();
  r->top = -getScrollY() + pos * itemHeight + Y_SHIFT + cr.top;
  r->bottom = r->top + itemHeight;

  // clip!
  if (r->top > cr.bottom || r->bottom < cr.top) return 0;

  return 1;
}

int ListWnd::locateData(LPARAM data) { // linear search
  for (int i=0;i<itemList.getNumItems();i++) {
    if (itemList[i]->data == data)
      return i;
  }
  return -1;
}

int ListWnd::getItemFocused(int pos) {
  if (pos >= itemList.getNumItems()) return 0;
  return getItemFocused() == pos;
}

int ListWnd::getItemSelected(int pos) {
  listItem *item = itemList[pos];
  return (item && selItemList->isSelected(pos));
}

int ListWnd::hitTest(POINT pos) {
  RECT r;
  getClientRect(&r);
  if (pos.y < r.top) return LW_HT_ABOVE;
  if (pos.y > r.bottom) return LW_HT_BELOW;

  if (pos.x > getColumnsWidth() - getScrollX()) return LW_HT_DONTKNOW;
  
  int i = (pos.y - r.top + getScrollY() - Y_SHIFT) / itemHeight;
  if (i >= itemList.getNumItems()) return LW_HT_DONTKNOW;
  return i; 
}

int ListWnd::hitTest(int x, int y) {
  POINT pt={x, y};
  return hitTest(pt);
}

void ListWnd::selectRect(int x1, int y1, int x2, int y2) {

}

void ListWnd::drawRect(int x1, int y1, int x2, int y2) {
  HDC dc = GetDC(gethWnd());
  RECT r={x1,y1,x2,y2};
  DrawFocusRect(dc, &r);
  ReleaseDC(gethWnd(), dc);
}

LPARAM ListWnd::getItemData(int pos) {
  if (pos >= itemList.getNumItems()) return 0;
  listItem *item = itemList[pos];
  if (item) return item->data;
  return NULL;
}

int ListWnd::getItemLabel(int pos, int subpos, char *txt, int textmax) {
  const char *t = getSubitemText(pos, subpos);
  if (t) {
    STRNCPY(txt, t, textmax);
    txt[textmax-1] = 0;
    return 1;
  }
  return 0;
}

void ListWnd::setItemLabel(int pos, char *text) {
  if (pos >= itemList.getNumItems()) return;
  listItem *item = itemList[pos];
  if (!item) return;
  item->label = text;
  invalidateItem(pos);
}

void ListWnd::resort() {
  itemList.sort(TRUE);
  invalidate();
}

int ListWnd::getSortDirection() {
  return sortdir;
}

int ListWnd::getSortColumn() {
  return sortcol;
}

void ListWnd::setSortDirection(int dir) {
  sortdir=dir;
}

void ListWnd::setSortColumn(int col) {
  sortcol=col;
}

int ListWnd::sortCompareItem(listItem *p1, listItem *p2) {
  RecycleString l1=p1->label;
  RecycleString l2=p2->label;

  if(sortcol!=0) {
    for (int i=0;i<p1->subitems->getNumItems();i++) {
      listSubitemStruct *subitem = p1->subitems->enumItem(i);
      if (subitem->column == sortcol) {
        l1=subitem->label;
        break;
      }
    }
    for (i=0;i<p2->subitems->getNumItems();i++) {
      listSubitemStruct *subitem = p2->subitems->enumItem(i);
      if (subitem->column == sortcol) {
        l2=subitem->label;
        break;
      }
    }
  }

  if(!columnsList.enumItem(sortcol)->getNumeric()) {
    if(!sortdir) return(STRICMP(l1.getValue(), l2.getValue()));
    else return(STRICMP(l2.getValue(), l1.getValue()));
  } else {
    int a=ATOI(l1.getValue()),b=ATOI(l2.getValue());
    if(!sortdir) return a-b;
    else return b-a;
  }
}


int ListWnd::findItemByParam(LPARAM param) {
  for (int i=0;i<itemList.getNumItems();i++) {
    if (itemList[i]->data == param)
      return i;
  }
  return -1;
}

void ListWnd::setItemParam(int pos, LPARAM param) {
  if (pos >= itemList.getNumItems()) return;
  itemList[pos]->data = param;
  invalidateItem(pos);
}

int ListWnd::deleteByPos(int pos) {
  listItem *item = itemList[pos];
  if (item == NULL) return 0;

  selItemList->setSelected(pos, FALSE);

  itemList.removeByPos(pos);

  onItemDelete(item->data);

  deleteListItem(item); item=NULL;

  if (redraw) {
    invalidate();
	setSlidersPosition();
  }
  return 1;
}

void ListWnd::deleteAllItems() {
  int sav = getRedraw();
  deselectAll();
  setRedraw(FALSE);
  while (itemList.getNumItems()) {
    deleteByPos(0);
  }
  setRedraw(sav);
  setSlidersPosition();
}

void ListWnd::setSubItem(int pos, int subpos, char *txt) {

  if (pos >= itemList.getNumItems()) return;
  listItem *item = itemList[pos];
  if (!item) return;
  if (!item->subitems)
    item->subitems = new PtrList<listSubitemStruct>;
  for (int i=0;i<item->subitems->getNumItems();i++) {
    listSubitemStruct *subitem = item->subitems->enumItem(i);
    if (subitem->column == subpos) {
      if (subitem->label) {
        FREE(subitem->label);
        subitem->label = NULL;
      }
      if (txt)
        subitem->label = STRDUP(txt);
      invalidateItem(pos);
      return;
    }
  }
  listSubitemStruct *subitem = (listSubitemStruct *) MALLOC(sizeof(listSubitemStruct));
  item->subitems->addItem(subitem);
  subitem->label = STRDUP(txt);
  subitem->column = subpos;
  invalidateItem(pos);
}

const char *ListWnd::getSubitemText(int pos, int subpos) {
  if (pos >= itemList.getNumItems()) return NULL;
  listItem *item = itemList[pos];
  if (!item) return NULL;
  if (subpos == 0) {
    return item->label;
  }
  if (!item->subitems) return NULL;
  for (int i=0;i<item->subitems->getNumItems();i++) {
    listSubitemStruct *subitem = item->subitems->enumItem(i);
    if (subitem->column == subpos) {
      return subitem->label;
    }
  }
  return NULL;
}

listItem *ListWnd::createListItem() {
  listItem *item = listItem_freelist.getRecord();
#ifdef FORTIFY
#undef new
#endif
  new(item) listItem();
#ifdef FORTIFY
#define new ZFortify_New
#endif
  item->setList(this);
  return item;
}

void ListWnd::deleteListItem(listItem *item) {
  if (item == NULL) return;
  item->~listItem();
  listItem_freelist.freeRecord(item);
}

ListColumn *ListWnd::enumListColumn(int pos) {
  return columnsList[pos];
}

int ListWnd::getColumnPosByName(const char *name) {
  for (int i = 0; i < columnsList.getNumItems(); i++) {
    const char *name2 = columnsList[i]->getName();
    if (name2 == NULL) continue;
    if (STREQL(name, name2)) return i;
  }
  return -1;
}

int ListWnd::delColumnByPos(int pos) {
  if (pos < 0 || pos >= columnsList.getNumItems()) return 0;
  delete columnsList[pos];
  columnsList.removeByPos(pos);
  recalcHeaders();
  return 1;
}

void ListWnd::recalcHeaders() {
  if (getNumColumns() <= 0) return;

  int wid = 0, ndynamic = 0;
  for (int i = 0; i < getNumColumns(); i++) {
    ListColumn *col = enumListColumn(i);
    if (!col->isDynamic()) wid += col->getWidth()+COLUMNS_MARGIN-1;
    else ndynamic++;
  }
  if (ndynamic == 0) return;

  RECT r = clientRect();
  int wwidth = r.right - r.left;

  int leftover = wwidth - wid;
  if (leftover <= 1) return;

  leftover--;
  leftover /= ndynamic;

  for (i = 0; i < getNumColumns(); i++) {
    ListColumn *col = enumListColumn(i);
    if (col->isDynamic()) col->setWidth(leftover);
  }

  invalidate();
}

int ListWnd::doAddItem(char *label, LPARAM lParam, int pos) {

  listItem *item = createListItem();
  if (label)
    item->label = label;
  else
    item->label = NULL;
  item->data = lParam;

  itemList.addItem(item, pos);
  lastAddedItem = item;

  if (redraw) {
    invalidate(); // todo: optimize to invalidate only if necessary
    setSlidersPosition();
  }

  if (gethWnd()) calcBounds();
  int p = (pos == POS_LAST) ? itemList.getNumItems()-1 : pos; 
  if (p <= selectionStart) selectionStart++;
  return p;
}

int ListWnd::addItem(char *label, LPARAM lParam) {
  return doAddItem(label, lParam, POS_LAST);
}

int ListWnd::insertItem(int pos, char *label, LPARAM lParam) {
  return doAddItem(label, lParam, pos);
}

int ListWnd::getLastAddedItemPos() {
  if (lastAddedItem == NULL) return -1;
  return itemList.searchItem(lastAddedItem);
}

int ListWnd::getColumnsHeight() {
  return columnsHeight;
}

int ListWnd::getColumnsWidth() {
  int i, x=0;
  for (i=0;i<columnsList.getNumItems();i++) x+= columnsList[i]->getWidth();
  return x+1;
}

int ListWnd::insertColumn(ListColumn *col, int pos) {
  ASSERT(col != NULL);
  ASSERT(pos >= -1);
  if (pos < 0) col->setIndex(columnsList.getNumItems());
  else col->setIndex(pos);
  col->setList(this);
  columnsList.addItem(col);
  if (pos >= 0) {
    columnsList.moveItem(columnsList.getNumItems()-1, pos);
  }
  if (redraw && isInited()) {
    invalidate();
    setSlidersPosition();
  }
  recalcHeaders();
  return columnsList.getNumItems();
}

int ListWnd::addColumn(const char *name, int width, int numeric) {
  ListColumn *col = new ListColumn();
  col->setWidth(width);
  col->setLabel(name);
  col->setNumeric(numeric);
  return insertColumn(col, -1);
}

void ListWnd::setRedraw(BOOL _redraw) {
  if (!redraw && _redraw) {
    invalidate();
    setSlidersPosition();
  }
  redraw = _redraw;
}

BOOL ListWnd::getRedraw() {
  return redraw;
}

int ListWnd::onChar(char c) {

  switch (c) {
    case 1: // CTRL-A
      selectAll();
      return 1;
  }

  char b = TOUPPER(c);
  if (b >= 'A' && b <= 'Z') {
    jumpToNext(b);
    return 1;
  }

  return LISTWND_PARENT::onChar(c);
}

int ListWnd::onKeyDown(int keyCode) {
  switch (keyCode) {
    case VK_DOWN:
      next();
      return 1;
    case VK_UP:
      previous();
      return 1;
    case VK_PRIOR:
      pageup();
      return 1;
    case VK_NEXT:
      pagedown();
      return 1;
    case VK_HOME:
      home();
      return 1;
    case VK_END:
      end();
      return 1;
    case VK_DELETE:
      onDelete();
      return 1;
    case VK_RETURN: {
      int i=getItemFocused();
      if(i!=-1)
        onDoubleClick(i);
      return 1;
    }
  }
  return LISTWND_PARENT::onKeyDown(keyCode);
}

void ListWnd::next() {
  int from=0;
  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  int to = from + 1;
  if (to < itemList.getNumItems() && to >= 0) {
    setSelectionStart(to);
    if (!fullyVisible(to)) {
      RECT c;
      getClientRect(&c);
      scrollToY((Y_SHIFT*2+(to+1)*itemHeight)-(c.bottom-c.top));
    }
  }
}

void ListWnd::previous() {
  int from=0;
  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  int to = from - 1;
  if (to < itemList.getNumItems() && to >= 0) {
    setSelectionStart(to);
    ensureItemVisible(to);
  }
}

void ListWnd::pagedown() {
  int from=-1,to;
  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  if(from==-1) to = 0;
  else to = from + getLinesPerPage();
  to=MIN(to,itemList.getNumItems()-1);
  if(to>=0) {
    setSelectionStart(to);
    if (!fullyVisible(to)) {
      RECT c;
      getClientRect(&c);
      scrollToY((Y_SHIFT*2+(to+1)*itemHeight)-(c.bottom-c.top));
    }
  }
}

void ListWnd::pageup() {
  int from=-1,to;
  if (selItemList->getNumItems() > 0)
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i;
        break;
      }
  if(from==-1) to = 0;
  else to = from - getLinesPerPage();
  to=MAX(to,0);
  to=MIN(to,itemList.getNumItems()-1);
  if(to>=0) {
    setSelectionStart(to);
    ensureItemVisible(to);
  }
}

void ListWnd::home() {
  if(!itemList.getNumItems()) return;
  setSelectionStart(0);
  ensureItemVisible(0);
}

void ListWnd::end() {
  if(!itemList.getNumItems()) return;
  int i=itemList.getNumItems()-1;
  setSelectionStart(i);
  if (!fullyVisible(i)) {
    RECT c;
    getClientRect(&c);
    scrollToY((Y_SHIFT*2+(i+1)*itemHeight)-(c.bottom-c.top));
  }
}

void ListWnd::jumpToNext(char c) {
  if (doJumpToNext(c, FALSE)) return;
  doJumpToNext(c, TRUE);
}

int ListWnd::doJumpToNext(char c, BOOL fromtop) {
  int from = 0;
  if (!fromtop && selItemList->getNumItems() > 0) {
    for (int i=0;i<itemList.getNumItems();i++)
      if (getItemSelected(i)) {
        from = i+1;
        break;
      }
  }
  for (int j=from;j<itemList.getNumItems();j++) {
    listItem *item = itemList[j];
    if (item->label != NULL) {
      char z = TOUPPER(*(item->label));
      if (z == c) {
        setSelectionStart(j);
        ensureItemVisible(j);
        return 1;
      }
    }
  }
  return 0;
}

void ListWnd::reset() {
  columnsList.deleteAll();
  deleteAllItems();
}

int ListWnd::onContextMenu (int x, int y) {
  return notifyParent(CHILD_LISTWND_POPUPMENU, x, y);
}

int ListWnd::onMouseWheelUp(int clicked, int lines) {
  if (!clicked)
    scrollToY(max(0, getScrollY()-itemHeight));
  else
    scrollToX(max(0, getScrollX()-itemHeight));
  return 1;
}

int ListWnd::onMouseWheelDown(int clicked, int lines) {
  if (!clicked)
    scrollToY(min(getMaxScrollY(), getScrollY()+itemHeight));
  else
    scrollToX(min(getMaxScrollX(), getScrollX()+itemHeight));
  return 1;
}

int ListWnd::onColumnLabelClick(int col, int x, int y)
{
  if(lastcolsort==col) {
    setSortDirection(1);
    lastcolsort=-1;
  } else {
    setSortDirection(0);
    lastcolsort=col;
  }
  setSortColumn(col);
  resort();
  return 1;
}

COLORREF ListWnd::getTextColor(LPARAM lParam) {
  return textColor;
}

COLORREF ListWnd::getSelBgColor(LPARAM LParam) {
  return color_item_selected;
}

COLORREF ListWnd::getBgColor() {
  return bgcolor;
}

COLORREF ListWnd::getFocusColor(LPARAM LParam) {
  return color_item_focused;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ListColumn
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ListColumn::ListColumn(const char *name) :
  Named(name)
{
  index = -1;
  width = COLUMNS_DEFAULT_WIDTH;
  list = NULL;
  dynamic = 0;
}

void ListColumn::setLabel(const char *newlabel) {
  setName(newlabel);
}

const char *ListColumn::getLabel() {
  return getName();
}

void ListColumn::setIndex(int newindex) {
  index = newindex;
}

int ListColumn::getIndex() {
  return index;
}

void ListColumn::setWidth(int _width) {
  width = _width;
  if (list && list->getRedraw()) {
    list->invalidate();
  }
}

int ListColumn::getWidth() {
  return width;
}

int ListColumn::customDrawHeader(Canvas *c, RECT *r) {
  int y = (r->bottom-r->top-c->getTextHeight()) / 2;
  c->textOutEllipsed(r->left, y, r->right-r->left, c->getTextHeight(), _(getName()));
  return 1;
}

void ListColumn::setDynamic(int isdynamic) {
  int prev = dynamic;
  dynamic = !!isdynamic;
  if (prev != dynamic && dynamic && list != NULL) list->recalcHeaders();
}

void ListColumn::setList(ListWnd *_list) {
  list = _list;
}

ListWnd *ListColumn::getList() {
  return list;
}

