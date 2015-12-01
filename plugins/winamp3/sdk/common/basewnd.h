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

//NONPORTABLE
#ifndef _BASEWND_H
#define _BASEWND_H

#define WIN32_LEAN_AND_MEAN
#include "std.h"
#include "common.h"
#include "rootwnd.h"
#include "drag.h"
#include "ptrlist.h"
#include "tlist.h"
#include "named.h"

class BaseWnd;
class TipWnd;
class BltCanvas;
class FilenamePS;
class Canvas;
class CanvasBase;
class Region;
class DragSet;
class VirtualWnd;

using namespace wasabi;

// it is safe to use anything higher than this for your own funky messages
#define BASEWND_NOTIFY_USER	NUM_NOTIFY_MESSAGES

#define BASEWND_CURSOR_USERSET			-1// window will do own setting
#define BASEWND_CURSOR_POINTER			1
#define BASEWND_CURSOR_NORTHSOUTH		2
#define BASEWND_CURSOR_EASTWEST			3
#define BASEWND_CURSOR_NORTHWEST_SOUTHEAST	4
#define BASEWND_CURSOR_NORTHEAST_SOUTHWEST	5
#define BASEWND_CURSOR_4WAY			6

// Our own defined window messages

#define WM_WA_CONTAINER_TOGGLED WM_USER+0x1338
#define WM_WA_COMPONENT_TOGGLED WM_USER+0x1339
#define WM_WA_RELOAD            WM_USER+0x133A
#define WM_WA_GETFBSIZE         WM_USER+0x133B
#define WM_WA_CONTEXTMENUNOTIFY WM_USER+0x133D
#define WM_WA_ACTIONNOTIFY      WM_USER+0x133E

#define DEFERREDCB_FLUSHPAINT   0x200

#define SYSRGN  4

typedef struct {
  RootWnd *origin;
  int param1;
  int param2;
} defered_callback;

// base class
#define BASEWND_PARENT RootWndI
class COMEXP NOVTABLE BaseWnd :
  public RootWndI,
  public DragInterface,
  public Named {

  friend VirtualWnd;

protected:
  // override constructor to init your data, but don't create anything yet
  BaseWnd();
public:
  virtual ~BaseWnd();

//INITIALIZATION
  // these actually create the window
  // try REALLY hard to not have to override these, and if you do,
  // override the second one
  virtual int init(RootWnd *parent, int nochild=FALSE);
  virtual int init(HINSTANCE hInstance, HWND parent, int nochild=FALSE);// for the root window
  virtual int isInited();	// are we post init() ? USE THIS INSTEAD OF gethWnd()==1

  // if at all possible put your init stuff in this one, and call up the
  // heirarchy BEFORE your code
  virtual int onInit();

  // use this to return the cursor type to show
  virtual int getCursorType(int x, int y);

// WINDOW SIZING/MOVEMENT/CONTROL
  virtual int getFontSize() { return 0; }
  virtual int setFontSize(int size) { return -1; }

  // if you override it, be sure to call up the heirarchy
  virtual void resize(int x, int y, int w, int h);	// resize yourself
  void resize(RECT *r);
  void resizeToRect(RECT *r) { resize(r); }//helper

  // called after resize happens, return TRUE if you override
  virtual int onResize();
  void resizeToClient(BaseWnd *wnd);	// resize a window to match you
  virtual int onPostedMove(); // whenever WM_WINDOWPOSCHANGED happens, use mainly to record positions when moved by the window tracker, avoid using for another prupose, not portable

  // move only, no resize
  virtual void move(int x, int y);

  // puts window on top of its siblings
  virtual void moveToFront();
  // puts window behind its siblings
  virtual void moveToBack();

  // fired when a sibbling invalidates a region. you can change the region before returning, use with care, can fuck up everything if not used well
  virtual int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx) { return 0; }

  // set window's visibility
  virtual void setVisible(int show);
  virtual void onSetVisible(int show);	// override this one

  // enable/disable window for input
  virtual void enable(int en);
  // grab the keyboard focus
  virtual void setFocus();

  virtual int pointInWnd(POINT *p);

  // repaint yourself
  virtual void invalidate();	// mark entire window for repainting
  virtual void invalidateRect(RECT *r);// mark specific rect for repainting
  virtual void invalidateRgn(Region *r);// mark specific rgn for repainting
  virtual void invalidateFrom(RootWnd *who);	// mark entire window for repainting
  virtual void invalidateRectFrom(RECT *r, RootWnd *who);// mark specific rect for repainting
  virtual void invalidateRgnFrom(Region *r, RootWnd *who);// mark specific rgn for repainting
  virtual void validate(); // unmark the entire window from repainting
  virtual void validateRect(RECT *r); // unmark specific rect from repainting
  virtual void validateRgn(Region *reg); // unmark specific region from repainting

  // no virtual on these please
  void deferedInvalidateRgn(Region *h);
  void deferedInvalidateRect(RECT *r);
  void deferedInvalidate();
  void deferedValidateRgn(Region *h); 
  void deferedValidateRect(RECT *r);
  void deferedValidate();
  Region *getDeferedInvalidRgn();
  int hasVirtualChildren();
  virtual int focusNextSibbling(int dochild);
  virtual int focusNextVirtualChild(BaseWnd *child);
  virtual int focusVirtualChild(RootWnd *child);

private: 
  virtual void physicalInvalidateRgn(Region *r); 
protected:
  virtual int ensureVirtualCanvasOk();
  virtual void setVirtualCanvas(Canvas *c);
  virtual void setRSize(int x, int y, int w, int h);

public:

  virtual void repaint();	// repaint right now!

  // override this to add decorations
  virtual void getClientRect(RECT *);
  RECT clientRect();	// helper
  virtual void getNonClientRect(RECT *);
  RECT nonClientRect();	// helper
  virtual void getWindowRect(RECT *);	// windows coords in screen system
  RECT windowRect();	// helper

  virtual void clientToScreen(int *x, int *y);		// convenience fn
  virtual void screenToClient(int *x, int *y);		// convenience fn
  virtual void clientToScreen(POINT *pt);		// convenience fn
  virtual void screenToClient(POINT *pt);		// convenience fn
  virtual void clientToScreen(RECT *r);		// convenience fn
  virtual void screenToClient(RECT *r);		// convenience fn

  void setIcon(HICON icon, int _small);
  virtual void setSkinId(int id);

  virtual int getPreferences(int what);
  virtual void setStartHidden(int wtf);

// from RootWnd
protected:
  virtual DragInterface *getDragInterface();
  virtual RootWnd *rootWndFromPoint(POINT *pt);
  virtual int getSkinId();
  virtual int onMetricChange(int metricid, int param1, int param2);
  virtual int rootwnd_onPaint(CanvasBase *canvas, Region *r);
  virtual int rootwnd_paintTree(CanvasBase *canvas, Region *r);

public:
#ifdef WIN32
  // override for custom processing (NONPORTABLE!)
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
// end from RootWnd

  // or override these, they're more portable
  virtual int onContextMenu(int x, int y);	// always return 1 if accept msg
  virtual int onChildContextMenu(int x, int y);	// always return 1 if accept msg

  void postDeferredCallback(int param1, int param2);
  virtual int onDeferredCallback(int param1, int param2);

  // called on WM_PAINT by onPaint(Canvas, Region *), if the canvas is null, create your own, if not, it will have update regions clipped for you
  virtual int onPaint(Canvas *canvas);
  void setTransparency(int amount);	// 0..255

private:
  virtual int onPaint(Canvas *canvas, Region *r);

public:  
  // click-drag. FYI, the drag-drop handling overrides these
  virtual int onLeftButtonDown(int x, int y); 
  virtual int onRightButtonDown(int x, int y) { return 0; }
  virtual int onMouseMove(int x, int y);	// only called when mouse captured
  virtual int onLeftButtonUp(int x, int y) { return 0; }
  virtual int onRightButtonUp(int x, int y) { return 0; }
  virtual int onMouseWheelUp(int click, int lines);
  virtual int onMouseWheelDown(int click, int lines);
  virtual int beginCapture();
  virtual int endCapture();
  virtual int getCapture();	// returns 1 if this window has mouse/keyb captured

  // these will not be called in the middle of a drag operation
  virtual int onLeftButtonDblClk(int x, int y);
  virtual int onRightButtonDblClk(int x, int y);

  virtual int onGetFocus();	// you have the keyboard focus
  virtual int onKillFocus();	// you lost the keyboard focus
  virtual int gotFocus();
  // return 1 if your derived class can indicate it has focus (i.e. LabelWnd)
  virtual int canShowFocus() { return 0; }
  int isActive();
  virtual int onActivate();
  virtual int onDeactivate();
  virtual int onEnable(int en);
  virtual int getEnabled();

  virtual int onChar(char c);
  virtual int onKeyDown(int keyCode) { return 0; }
  virtual int onKeyUp(int keyCode) { return 0; }

  virtual int onContextMenuNotify(int action, int param=0) { return 0; }
  virtual int onActionNotify(int action, int param=0) { return 0; }

  virtual int onEraseBkgnd(HDC dc);// override and return 0 to authorize bkg painting, 1 to avoid it (default to 1)
  virtual int onUserMessage(int msg, int w, int l, int *r);

//CHILD->PARENT MESSAGES
  // feel free to override for your own messages, but be sure to call up the
  // chain for unhandled msgs
  // children should not call this directly if they don't have to; use
  // notifyParent on yourself instead
  // message ids should be put in notifmsg.h to avoid conflicts
  virtual int childNotify(RootWnd *child, int msg,
                          int param1=0, int param2=0);

  // don't try to override these
  void setParent(RootWnd *newparent);
  RootWnd *getParent();
  virtual RootWnd *getRootParent(); 
  
  // avoid overriding this one if you can
  virtual int reparent(RootWnd *newparent);

  // override this one
  virtual void onSetParent(RootWnd *newparent) {}

  // override this to do your own region checking
  virtual BOOL mouseInRegion(int x, int y) { return 1; }

  // call this to notify your parent via its childNotify method
  virtual int notifyParent(int msg, int param1=0, int param2=0);

  // from class Named
  virtual void onSetName();

  virtual const char *getTip();
  virtual void setTip(const char *tooltip);
  virtual int getStartHidden();
  virtual void abortTip();
  virtual int isVisible();

	// Virtual windows functions
  virtual Canvas *createFrameBuffer(int w, int h);
  virtual void prepareFrameBuffer(Canvas *canvas, int w, int h);
  virtual void deleteFrameBuffer(Canvas *canvas);

  virtual void addVirtualChild(RootWnd *child);
  virtual void removeVirtualChild(RootWnd *child);
  virtual int isVirtual();
  static RootWnd *checkGhost(RootWnd *r, short x, short y);
  virtual RootWnd *getVirtualChild(int x, int y);
  virtual RootWnd *getVirtualChild(int _enum);
  virtual int getNumVirtuals();
  virtual int handleVirtualChildMsg(UINT uMsg,int x, int y, void *p=NULL);
  virtual void setVirtualChildCapture(RootWnd *child);
  virtual RootWnd *getVirtualChildCapture();
  virtual int setVirtualChildTimer(RootWnd *child, int id, int ms);
  virtual int killVirtualChildTimer(RootWnd *child, int id);

  virtual int cascadeRepaintFrom(RootWnd *who);
  virtual int cascadeRepaintRgnFrom(Region *reg, RootWnd *who);
  virtual int cascadeRepaintRectFrom(RECT *r, RootWnd *who);
  virtual int cascadeRepaint();
  virtual int cascadeRepaintRgn(Region *reg);
  virtual int cascadeRepaintRect(RECT *r);
  virtual void flushPaint();

  virtual void freeResources();
  virtual void reloadResources();

protected:
  virtual void do_flushPaint();
  virtual int paintTree(Canvas *canvas, Region *r);
  virtual int virtualBeforePaint() { return 0; }
  virtual int virtualAfterPaint() { return 0; }
  virtual int virtualOnPaint();
  virtual void virtualCanvasCommit(Canvas *canvas, RECT *r, double ratio);

public:
	
#ifdef WIN32
  virtual HWND gethWnd();
  //NONPORTABLE--avoid prolonged use
  virtual HINSTANCE gethInstance();//FUCKO CUT, use HINSTANCEfromHWND
#endif

public:
	
  BOOL getNoCopyBits(void);
  void setNoCopyBits(BOOL ncb);

protected:
//TIMERS
  // portable helper timer fns
  virtual int setTimer(int id, int ms);
  virtual void timerCallback(int id);
  virtual int killTimer(int id);
  virtual int checkDoubleClick(int button, int x, int y);

//MISC
public:
  virtual int isDestroying();	// in the middle of dying

//DRAGGING AND DROPPING -- (derived from DragInterface)

  // derived windows should call this if they detect a drag beginning
  // call once per datum per type of data being exposed. order is maintained
  int addDragItem(char *droptype, void *item);
  // returns TRUE if drop was accepted
  int handleDrag();
  int resetDragSet();	// you don't need to call this

  // (called on dest) when dragged item enters the winder
  virtual int dragEnter(RootWnd *sourceWnd);
  // (called on dest) during the winder
  // FG> x/y are in screen corrdinates because target is a rootwnd
  virtual int dragOver(int x, int y, RootWnd *sourceWnd) { return 0; }
  // (called on src)
  virtual int dragSetSticky(RootWnd *wnd, int left, int right, int up, int down);
  // (called on dest) when dragged item leaves the winder
  virtual int dragLeave(RootWnd *sourceWnd) { return 0; }

  // when it finally is dropped:
 
  // called on destination window
  // FG> x/y are in screen corrdinates because target is a rootwnd
  virtual int dragDrop(RootWnd *sourceWnd, int x, int y) { return 0; }
  // called on source window
  virtual int dragComplete(int success) { return 0; }
  // in that order
  // must be called right before handleDrag();		(sender)
  void setSuggestedDropTitle(const char *title);

  // must be called from dragDrop();			(receiver)
  virtual const char *dragGetSuggestedDropTitle(void);
  virtual int dragCheckData(const char *type, int *nitems=NULL);
  virtual void *dragGetData(int slot, int itemnum);
  virtual int dragCheckOption(int option) { return 0; }

  // return TRUE if you support any of the datatypes this window is exposing
  virtual int checkDragTypes(RootWnd *sourceWnd) { return 0; }

// external drops 
  // override this and return 1 to receive drops from the OS
  virtual int acceptExternalDrops() { return 0; }

  virtual void onExternalDropBegin() {}
  virtual void onExternalDropDirScan(const char *dirname) {}
  virtual void onExternalDropEnd() {}
private:
  void addDroppedFile(const char *filename, PtrList<FilenamePS> *plist); // recursive

public:
//FG> alternate notify window
  virtual void setNotifyWindow(RootWnd *newnotify);
  virtual RootWnd *getNotifyWindow();

  virtual double getRenderRatio();
  virtual void setRenderRatio(double r);
  virtual int handleRatio() { return 1; }
  int renderRatioActive();
  void multRatio(int *x, int *y=NULL);
  void multRatio(RECT *r);
  void divRatio(int *x, int *y=NULL);
  void divRatio(RECT *r);
  virtual int isClickThrough() { return 0; }

// DERIVED WINDOW BEHAVIORAL PREFERENCES
protected:
  virtual int wantDoubleClicks();
  // return 1 to get onMouseOver even if mouse isn't captured
  virtual int wantSiblingInvalidations();

  virtual int wantFocus();

protected: // tip function

  void onTipMouseMove();
  void renderBaseTexture(CanvasBase *c, RECT &r);

public:
  virtual RootWnd *getBaseTextureWindow();
  void setBaseTextureWindow(RootWnd *w);
  virtual int isMouseOver(int x, int y);

  virtual void bringVirtualToFront(RootWnd *w);
  virtual void bringVirtualToBack(RootWnd *w);
  virtual void bringVirtualAbove(RootWnd *w, RootWnd *b);
  virtual void bringVirtualBelow(RootWnd *w, RootWnd *b);
  void changeChildZorder(RootWnd *w, int newpos);

private:
  // ATTENTION: note the capitalization on these -- so as not to mix up with
  // wndProc()'s hWnd
  HINSTANCE hinstance;
  HWND hwnd;

  RootWnd *parentWnd;

  int inputCaptured;

  void onTab();

protected:

  int checkModal();

  int dragging;

	Canvas *virtualCanvas;
  static int (WINAPI *getRandomRgn)(HDC dc, HRGN rgn, int i);

private:

  PtrList<DragSet> dragsets;
  RootWnd *prevtarg;
  String suggestedTitle;

  //FG
  int start_hidden;
  char *tip;
  TipWnd *ttip;
  BOOL tip_done;
  BOOL tipawaytimer;
  BOOL tipshowtimer;
  BOOL tipbeenchecked;
  BOOL tipdestroytimer;

  //FG
  BOOL ncb;
  DWORD lastClick[2];
  POINT lastClickP[2];

  //FG
  RootWnd *btexture;

  //FG
  RootWnd *notifyWindow;
  BOOL destroying;

	//CT:Virtual children
	PtrList<RootWnd> virtualChildren;
	int virtualCanvasHandled;
	int virtualCanvasH,virtualCanvasW;
	int rwidth, rheight, rx, ry;
	RootWnd *curVirtualChildCaptured;
	typedef struct {
		int id;
		RootWnd *child;
	} virtualChildTimer;
	wasabi::TList<virtualChildTimer> virtualChildTimers;

	//FG>
	Region *deferedInvalidRgn;
	static int nreal;
	static HINSTANCE gdi32instance;
  static int grrfailed;

  HWND oldCapture;
  
	int hasfocus;
	BaseWnd *curVirtualChildFocus;

	double ratio;
  int skin_id;
};

#ifdef WIN32
__inline HINSTANCE HINSTANCEfromHWND(HWND wnd) {
  if (wnd == NULL) return NULL;
  return (HINSTANCE)GetWindowLong(wnd, GWL_HINSTANCE);
}
#endif

#endif
