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

#ifndef __TREEWND_H
#define __TREEWND_H

// BU: lots of changes
// - all items must be deletable, and will be deleted on destructor
// - root items list not allocated w/ new
// - items set sorting within their PtrListSorted instead of manually calling it
// - setting an item to auto-sort does *not* make subitems autosort too

#include "scbkgwnd.h"
#include "ptrlist.h"
#include "editwnd.h"
#include "common.h"
#include "skinclr.h"

#define TREEWND_PARENT ScrlBkgWnd

#define STATUS_EXPANDED  0
#define STATUS_COLLAPSED 1

#define HITTEST_BEFORE   0
#define HITTEST_IN       1
#define HITTEST_AFTER    2

#define LINK_RIGHT       1
#define LINK_TOP         2
#define LINK_BOTTOM      4

#define TAB_NO           FALSE
#define TAB_YES          TRUE
#define TAB_AUTO         2

#define WM_SETITEMDEFERRED WM_USER+6546

#define DC_SETITEM	10
#define DC_DELITEM	20
#define DC_EXPAND	30
#define DC_COLLAPSE	40

// Forward references

class TreeItemList;
class TreeItem;
class TreeWnd;

class FontSize;

// classes & structs

class COMEXP TreeItem {
friend TreeWnd;
public:
  TreeItem();
  virtual ~TreeItem();

  virtual SkinBitmap *getIcon();
  virtual void setIcon(SkinBitmap *newicon);

  // FUCKO: see if these are all implemented
  virtual void onTreeAdd() {}
  virtual void onTreeRemove() {}
  virtual void onSelect() {}
  virtual void onDeselect() {}
  virtual int onLeftDoubleClick() { return 0; }
  virtual int onRightDoubleClick() { return 0; }
  virtual int onContextMenu(int x, int y);
  virtual int onChar(UINT key) { return 0; }	// return 1 if you eat the key

  virtual int onBeginLabelEdit();
  virtual int onEndLabelEdit(const char *newlabel);

  virtual void setLabel(const char *label);
  virtual const char *getLabel();

  // override to draw by yourself. Return the width of what you've drawn
  virtual int customDraw(Canvas *canvas, const POINT &pt, int defaultTxtHeight, int indentation, const RECT &clientRect); 

  // return 0 to refuse being dragged
  // else return 1 and install the droptype and dropitem
  virtual int onBeginDrag(char *suggestedTitle) { return 0; }

  virtual int dragOver(RootWnd *sourceWnd) { return 0; }
  virtual int dragLeave(RootWnd *sourceWnd) { return 0; }
  virtual int dragDrop(RootWnd *sourceWnd) { return 0; }

  virtual int dragComplete(int success) { return 0; }

  void ensureVisible();

  TreeItem *getNthChild(int nth); // enumerates children (zero based)
  TreeItem *getChild();
  TreeItem *getChildSibling(TreeItem *item);
  TreeItem *getSibling();
  TreeItem *getParent();

  void editLabel();
	
  int getNumChildren();
  BOOL hasSubItems();

  void setSorted(int issorted);
  void setChildTab(int haschildtab);
  BOOL isSorted();

  BOOL isCollapsed();
  BOOL isExpanded();

  void invalidate();
  BOOL isSelected();
  BOOL isHilited();
  void setHilited(BOOL ishilited);

  int collapse();
  int expand();

  int getCurRect(RECT *r);

  void setCurrent(BOOL tf);

  TreeWnd *getTree() const;

protected:

  BOOL isHilitedDrop();
  void setHilitedDrop(BOOL ishilitedDrop);

  void linkTo(TreeItem *linkto);
//  void childDeleted(TreeItem *child);
  void setTree(TreeWnd *newtree);
  void addSubItem(TreeItem *item);
  void setCurRect(int x1, int y1, int x2, int y2, int z);
  int getIndent();

  BOOL needTab();
  void sortItems(); // sorts the children of this item
  void setEdition(BOOL isedited);
  BOOL getEdition();

private:
  void setSelected(BOOL isselected, BOOL expandCollapse=FALSE, BOOL editifselected=FALSE);
  // this really calls delete on the subitems
  void deleteSubitems();

  int removeSubitem(TreeItem *item);

  int getItemWidth(int txtHeight, int indentation);

  String label;
  class TreeItem *parent;
  TreeItemList *subitems;	// children
  RECT curRect;
  int childTab;
  TreeWnd *tree;
  int expandStatus;
  SkinBitmap *icon;
  int _z;

  BOOL selected:1;
  BOOL hilitedDrop:1;
  BOOL hilited:1;
  BOOL being_edited:1;
};

class CompareTreeItem {
public:
  static int compareItem(TreeItem *p1, TreeItem *p2);
};

class TreeItemList : public PtrListQuickSorted<TreeItem, CompareTreeItem> { };

class COMEXP TreeWnd : public TREEWND_PARENT {

friend TreeItem;

public:
	
  TreeWnd();
  virtual ~TreeWnd();

  virtual int onInit();
  virtual int onPaint(Canvas *canvas);
  virtual int childNotify(RootWnd *child, int msg, int param1=0, int param2=0);

  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  virtual int onMouseMove(int x, int y);

  virtual int onLeftButtonDblClk(int x, int y);
  virtual int onRightButtonDblClk(int x, int y);

  virtual int onMouseWheelUp(int clicked, int lines);
  virtual int onMouseWheelDown(int clicked, int lines);
  
  virtual void timerCallback(int c);

  virtual int onContextMenu(int x, int y);
  virtual int onDeferredCallback(int param1, int param2);

  virtual int onChar(char c);
  virtual int onKeyDown(int keycode);
  virtual void jumpToNext(char c);
  void ensureItemVisible(TreeItem *item);

  // don't need to override this: just calls thru to the treeitem
  virtual int onBeginDrag(TreeItem *treeitem);

  virtual int dragEnter(RootWnd *sourceWnd);
  virtual int dragOver(int x, int y, RootWnd *sourceWnd);
  virtual int dragLeave(RootWnd *sourceWnd);
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y);

  virtual int dragComplete(int success);

  int wantFocus() { return 1; }
  virtual void freeResources();
  virtual void reloadResources();

  virtual int compareItem(TreeItem *p1, TreeItem *p2);

protected:
  // these will be called if the pointer is not over a treeitem
  virtual int defaultDragOver(int x, int y, RootWnd *sourceWnd) { return 0; }
  virtual int defaultDragDrop(RootWnd *sourceWnd, int x, int y) { return 0; }

  // called with item that received a drop
  virtual void onItemRecvDrop(TreeItem *item) {}

  virtual void onLabelChange(TreeItem *item) {}

  virtual void onItemSelected(TreeItem *item) {}
  virtual void onItemDeselected(TreeItem *item) {}

  virtual int onKillFocus();

public:

  virtual int getContentsWidth();
  virtual int getContentsHeight();

  void setRedraw(BOOL r);

  TreeItem *addTreeItem(TreeItem *item, TreeItem *par, int sorted=TRUE, int haschildtab=FALSE);

  // just removes a TreeItem from the tree, doesn't delete it... this is for
  // ~TreeItem to call only
  int removeTreeItem(TreeItem *item);

  void moveTreeItem(TreeItem *item, TreeItem *newparent);

  void deleteAllItems();

  int expandItem(TreeItem *item);
  void expandItemDeferred(TreeItem *item);
  int collapseItem(TreeItem *item);
  void collapseItemDeferred(TreeItem *item);

  void selectItem(TreeItem *item);	// selects.
  void selectItemDeferred(TreeItem *item);// selects. posted.
  void delItemDeferred(TreeItem *item);
  void hiliteItem(TreeItem *item);
  void unhiliteItem(TreeItem *item);
  void setHilitedColor(const char *colorname);
  COLORREF getHilitedColor();

  TreeItem *getCurItem();

  TreeItem *hitTest(POINT pos);
  TreeItem *hitTest(int x, int y);

  virtual void setTip(const char *tip);
  virtual const char *getTip();

  void editItemLabel(TreeItem *item);
  void cancelEditLabel(int destroyit=0);
  void setAutoEdit(int ae);
  int getAutoEdit();
  // use a NULL item to search all items. returns first item found
  TreeItem *getByLabel(TreeItem *item, char *name);

  int getItemRect(TreeItem *item, RECT *r);

  int ownerDraw();

  int getNumRootItems();
  TreeItem *enumRootItem(int which);

  void setSorted(BOOL dosort);
  BOOL getSorted();

  void sortTreeItems();

  TreeItem *getSibling(TreeItem *item);

  TreeItem *getItemFromPoint(POINT *pt);

  void setAutoCollapse(BOOL doautocollapse);

  virtual int setFontSize(int newsize);
  int getFontSize();

  int getNumVisibleChildItems(TreeItem *c);
  int getNumVisibleItems();
  TreeItem *enumVisibleItems(int n);
  TreeItem *enumVisibleChildItems(TreeItem *c, int n);

protected:
  void hiliteDropItem(TreeItem *item);  
  void unhiliteDropItem(TreeItem *item);
  void invalidateMetrics();

private:
  TreeItemList items;	// root-level stuff

  TreeItem *curSelected;

	BltCanvas *dCanvas;

  void drawItems(Canvas *c);
  void setCurItem(TreeItem *item, BOOL expandCollapse=TRUE, BOOL editifselected=FALSE);
  void countSubItems(PtrList<TreeItem> &drawlist, TreeItemList *list, int indent, int *c, int *m, int z);
  void getMetrics(int *numItemsShow, int *maxWidth);
  void ensureMetricsValid();
  int getLinkLine(TreeItem *item, int level);
  void endEditLabel(char *newlabel);
  void editUpdate();
  int jumpToNextSubItems(TreeItemList *list, char c);

  int itemHeight;

  SkinBitmap *tabClosed, *tabOpen;
  SkinBitmap *linkTopRight, *linkTopBottom, *linkTopRightBottom;
  SkinBitmap *linkTabTopRight, *linkTabTopBottom, *linkTabTopRightBottom;
  
  TreeItem *firstItemVisible;
  TreeItem *lastItemVisible;

  TreeItem *mousedown_item, *prevbdownitem;
  POINT mousedown_anchor;
  BOOL mousedown_dragdone;
  TreeItem *hitItem,		// the dest item
           *draggedItem;	// the source item

  int inHitTest;

  BOOL metrics_ok;
  int maxWidth;
  int maxHeight;

  String defaultTip;

  const char *getLiveTip();
  void setLiveTip(const char *tip);
  TreeItem *tipitem;

  BOOL redraw;

  PtrList<TreeItem> drawList;
  TreeItem *edited;

  EditWnd *editwnd;
  char editbuffer[256];

  int deleteItems;
  BOOL firstFound;

  TreeItem *currentItem;
  String hilitedColorName;
  SkinColor hilitedColor;
  int autoedit;
  int autocollapse;
  int textsize;
};

template<class T> class TreeItemParam : public TreeItem {
public:
  TreeItemParam(T _param) { param = _param; }

  T getParam() { return param; }
  operator T() { return getParam(); }

private:
  T param;
};

#endif
