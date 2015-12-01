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

#ifndef _LISTWND_H
#define _LISTWND_H

#include "scbkgwnd.h"
#include "common.h"

#include "string.h"
#include "freelist.h"

#define POS_LAST -1

#define LISTWND_PARENT ScrlBkgWnd

#define LW_HT_DONTKNOW	(-1)
#define LW_HT_ABOVE	(-10)
#define LW_HT_BELOW	(-20)

class listItem;
class ListWnd;
class CompareListItem;

class COMEXP ListColumn : public Named {
friend ListWnd;
public:
  ListColumn(const char *name=NULL);

  int getWidth();
  void setWidth(int newwidth);
  const char *getLabel();
  void setLabel(const char *newlabel);
  virtual int customDrawHeader(Canvas *c, RECT *cr);
  int getNumeric() { return numeric; }

  void setDynamic(int isdynamic);
  int isDynamic() { return dynamic; }

protected:  
  void setIndex(int i);
  int getIndex();
  void setList(ListWnd *list);
  ListWnd *getList();
  void setNumeric(int n) { numeric=n; }

private:
  int width;
  int index;
  int numeric;
  int dynamic;
  ListWnd *list;
};

class SelItemList;

class COMEXP ListWnd : public ScrlBkgWnd {
friend ListColumn;
friend SelItemList;
public:
  ListWnd();
  virtual ~ListWnd();

  virtual int onInit();
  virtual int onPaint(Canvas *canvas);
  virtual int onResize();
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onRightButtonDown(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  virtual int onMouseMove(int x, int y);
  virtual int onLeftButtonDblClk(int x, int y);
  virtual int onChar(char c);
  virtual int onKeyDown(int keyCode);
  virtual int onContextMenu (int x, int y);
  virtual int onMouseWheelUp(int click, int lines);
  virtual int onMouseWheelDown(int click, int lines);

  void onSetVisible(int show);

  void setAutoSort(BOOL dosort);
  void setOwnerDraw(BOOL doownerdraw);

  void next();
  void previous();
  void pagedown();
  void pageup();
  void home();
  void end();
  
  void setItemCount(int c);
  void reset();
  int addColumn(const char *name, int width, int numeric=0);	// adds to end
  ListColumn *getColumn(int n);
  int getNumColumns();
  int getColumnWidth(int col);
  void setRedraw(BOOL redraw);
  BOOL getRedraw();
  virtual int addItem(char *label, LPARAM lParam);
  virtual int insertItem(int pos, char *label, LPARAM lParam);
  virtual int getLastAddedItemPos();
  virtual void setSubItem(int pos, int subpos, char *txt);
  virtual void deleteAllItems();
  virtual int deleteByPos(int pos);
  int getNumItems(void);
  virtual int getItemLabel(int pos, int subpos, char *text, int textmax);
  virtual void setItemLabel(int pos, char *text);
  virtual LPARAM getItemData(int pos);
  virtual int getItemRect(int pos, RECT *r);
  virtual int getItemSelected(int pos);	// returns 1 if selected
  virtual int getItemFocused(int pos);	// returns 1 if focused
  virtual int getItemFocused();         // returns focused item
  void ensureItemVisible(int pos);
  virtual int scrollAbsolute(int x);
  virtual int scrollRelative(int x);
  virtual const char *getSubitemText(int pos, int subpos);
  virtual int getFirstItemSelected();

  virtual int selectAll();	// force all items selected
  virtual int deselectAll(); // force all items to be deselected
  virtual int invertSelection();	// invert all selections

  virtual int hitTest(POINT pos);
  virtual int hitTest(int x, int y);
  virtual int invalidateItem(int pos);
  virtual int locateData(LPARAM data);

  // -1 if we've never been drawn yet
  int getFirstItemVisible() const { return firstItemVisible; }
  int getLastItemVisible() const { return lastItemVisible; }

  virtual int setFontSize(int size);
  virtual int getFontSize();
  virtual void jumpToNext(char c);
  int wantFocus() { return 1; }

  void scrollToItem(int pos);

  virtual void resort();
  int getSortDirection();
  int getSortColumn();
  void setSortColumn(int col);
  void setSortDirection(int dir);
  int findItemByParam(LPARAM param);
  void setItemParam(int pos, LPARAM param);
  int getItemCount() { return getNumItems(); }

  void setSelectionStart(int pos);
  void setSelectionEnd(int pos);
  void setSelected(int pos, int selected);
  void toggleSelection(int pos, int setfocus=TRUE);

  virtual int getHeaderHeight();

  // this sort function just provides string/numeric comparison
  // if you need more types, just override and provide your own
  virtual int sortCompareItem(listItem *p1, listItem *p2);
  
protected:
  // return 1 if you override this
  virtual int ownerDraw(Canvas *canvas, int pos, RECT *r, LPARAM lParam, int selected, int focused) { return 0; };

  virtual void onPreItemDraw(Canvas *canvas, int pos, RECT *r, LPARAM lParam, int selected, int focused) { }
  virtual void onPostItemDraw(Canvas *canvas, int pos, RECT *r, LPARAM lParam, int selected, int focused) { };
  virtual COLORREF getTextColor(LPARAM lParam);
  virtual COLORREF getSelBgColor(LPARAM lParam);
  virtual COLORREF getBgColor();
  virtual COLORREF getFocusColor(LPARAM lParam);

public:
  int insertColumn(ListColumn *col, int pos=-1);// -1 is add to end

protected:
  int getColumnsHeight();
  int getColumnsWidth();
  int getContentsWidth();
  int getContentsHeight();
  virtual void drawBackground(Canvas *canvas);
  void drawColumnHeaders(Canvas *c);
  void drawItems(Canvas *canvas);
  void updateScrollX();
  void updateScrollY();
  int doJumpToNext(char c, BOOL fromTop);
  int fullyVisible(int pos);

  virtual int onBeginDrag(int iItem);

  virtual void onSelectAll();	// hit Control-A
  virtual void onDelete();		// hit 'delete'
  virtual void onItemDelete(LPARAM lparam) {}
  virtual void onDoubleClick(int itemnum);	// double-click on an item
  // this is called with the selected item#
  virtual void onLeftClick(int itemnum);	// left-click
  // the second time you click on an already-focused item
  virtual void onSecondLeftClick(int itemnum);
  // return 1 if you want more callbacks with the rest of the items
  virtual int onRightClick(int itemnum);	// right-click on item

  // override this to be notified of item selections & deselections
  virtual void onItemSelection(int itemnum, int selected) {}

  virtual int onColumnDblClick(int col, int x, int y) { return 0; }
  virtual int onColumnLabelClick(int col, int x, int y);

  void selectRect(int x1, int y1, int x2, int y2);
  void drawRect(int x1, int y1, int x2, int y2);

  // interface to Freelist
  listItem *createListItem();
  void deleteListItem(listItem *item);

  ListColumn *enumListColumn(int pos);
  int getColumnPosByName(const char *name);
  int delColumnByPos(int pos);

protected:
  BOOL showColumnsHeaders;

  void recalcHeaders();

private:
  int doAddItem(char *label, LPARAM lParam, int pos);
  int hitTestColumns(POINT p, int *origin=NULL);
  int hitTestColumnsLabel(POINT p);
  void drawXorLine(int x);
  void calcNewColWidth(int col, int x);
  void calcBounds();

  int autosort, ownerdraw;
  int textsize;
	int itemHeight;
	BOOL metrics_ok;
	BOOL redraw;
	int columnsHeight;

	PtrList<ListColumn> columnsList;
	PtrListQuickSorted<listItem,CompareListItem> itemList;

	int firstItemVisible;
	int lastItemVisible;

	listItem *lastItemFocused;
	int lastItemFocusedPos;

	listItem *lastAddedItem;
  SelItemList *selItemList;

	int selectionStart;
	int colresize;
	POINT colresizept;
	BOOL resizing_col;
	int colresizeo;

	BOOL processbup;
	BOOL bdown;
	BOOL nodrag;
  int bdownx, bdowny;
	BOOL firstComplete, lastComplete;

	int rectselecting;
	POINT selectStart;
	POINT selectLast;

  int sortdir, sortcol, lastcolsort;
	
  Freelist<listItem> listItem_freelist;
};

#endif
