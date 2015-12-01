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

#include "std.h"
#include "../studio/assert.h"

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
const int WS_EX_LAYERED=0x80000;
const int LWA_ALPHA=2;
#endif

#include "basewnd.h"
#include "usermsg.h"
#include "tipwnd.h"
#include "tlist.h"
#include "canvas.h"
#include "filename.h"
#include "region.h"
#include "notifmsg.h"
#include "../studio/api.h"
#include "../studio/metricscb.h"

#define TIP_TIMER_ID        1601
#define TIP_DESTROYTIMER_ID 1602
#define TIP_AWAY_ID         1603
#define TIP_AWAY_DELAY       100

#define TIP_TIMER_THRESHOLD  350
#define TIP_LENGTH          3000

#define VCHILD_TIMER_ID_MIN	2000
#define VCHILD_TIMER_ID_MAX	2100

class DragSet : public PtrList<void>, public Named {};

static void register_wndClass(HINSTANCE);

#define ROOTSTRING "RootWnd"

#define BASEWNDCLASSNAME "BaseWindow_" ROOTSTRING

#define WM_MOUSEWHEEL 0x20A

static RootWnd *stickyWnd;
static RECT sticky;

#ifdef WIN32
static HINSTANCE user32instance;
static void (__stdcall *setLayeredWindowAttributes)(HWND, int, int, int);
static int checked_for_alpha_proc;
#endif

/*RootWnd *RootWnd::rootwndFromPoint(POINT &point, int level) {
  RootWnd *wnd;
  wnd = api->rootWndFromPoint(&point);
  return RootWnd::rootwndFromRootWnd(wnd, level, &point);
}

RootWnd *RootWnd::rootwndFromRootWnd(RootWnd *wnd, int level, POINT *point) {

  for (;;) {
    if (wnd == NULL || level < 0) return NULL;
    if (point) {
      RECT r;
      wnd->getWindowRect(&r);
      if (!PtInRect(&r, *point)) return NULL; // PORT ME
    }
    if (level == 0) return wnd; 
    wnd = wnd->getRootWndParent();
    level--;
  }
  // should never get here
}*/

#include <shlobj.h>
#include <shellapi.h>

static HRESULT ResolveShortCut(HWND hwnd, LPCSTR pszShortcutFile, LPSTR pszPath)
{
  HRESULT hres;
  IShellLink* psl;
  char szGotPath[MAX_PATH];
  WIN32_FIND_DATA wfd;

  *pszPath = 0;   // assume failure

  hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (void **) &psl);
  if (SUCCEEDED(hres))
  {
    IPersistFile* ppf;

    hres = psl->QueryInterface(IID_IPersistFile, (void **) &ppf); // OLE 2!  Yay! --YO
    if (SUCCEEDED(hres))
    {
      WORD wsz[MAX_PATH];
      MultiByteToWideChar(CP_ACP, 0, pszShortcutFile, -1, wsz, MAX_PATH);

     hres = ppf->Load(wsz, STGM_READ);
     if (SUCCEEDED(hres))
     {
       hres = psl->Resolve(hwnd, SLR_ANY_MATCH);
       if (SUCCEEDED(hres))
       {
          strcpy(szGotPath, pszShortcutFile);
          hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd,
                 SLGP_SHORTPATH );
		  strcpy(pszPath,szGotPath);
        }
      }
      ppf->Release();
    }
    psl->Release();
  }
  return SUCCEEDED(hres);
}

BaseWnd::BaseWnd() {
  hinstance = NULL;
  hwnd = NULL;
  parentWnd = NULL;
  dragging = 0;
  prevtarg = NULL;
  inputCaptured = 0;
  btexture = NULL;
  ncb=FALSE;

  tip = NULL;
  ttip = NULL;
  tip_done = FALSE;
  tipshowtimer = FALSE;
  tipawaytimer = FALSE;
  tipdestroytimer = FALSE;
  start_hidden = 0;
  notifyWindow = NULL;
  lastClick[0] = 0;
  lastClick[1] = 0;
  lastClickP[0].x = 0;
  lastClickP[0].y = 0;
  lastClickP[1].x = 0;           
  lastClickP[1].y = 0;
  destroying = FALSE;

  curVirtualChildCaptured = NULL;
	virtualCanvas = NULL; virtualCanvasH = virtualCanvasW = 0;
	deferedInvalidRgn = NULL; 

	hasfocus = 0;
	curVirtualChildFocus = NULL;
  ratio = 1;
  rwidth=rheight=0;
  skin_id = -1;
}

BaseWnd::~BaseWnd() {
  if (hwnd) nreal--;
  if (!nreal) {
    FreeLibrary(gdi32instance);
    gdi32instance = NULL;
    FreeLibrary(user32instance);
    user32instance = NULL;
  }
  destroying=TRUE;
  curVirtualChildFocus = NULL;
  if (inputCaptured && GetCapture() == gethWnd()) ReleaseCapture();

  if (hwnd != NULL) {
#ifdef URLDROPS
    if (acceptExternalDrops()) RevokeDragDrop(hwnd, &m_target);
#endif
    DestroyWindow(gethWnd());
  }
  deleteFrameBuffer(virtualCanvas);
  virtualCanvas=NULL;

  resetDragSet();

  notifyParent(CHILD_DELETED);
  if (tip) FREE(tip);
  if (ttip) {
    delete ttip;
    killTimer(TIP_DESTROYTIMER_ID);
  }
  if (tipshowtimer)
    killTimer(TIP_TIMER_ID);
  if (tipawaytimer)
    killTimer(TIP_AWAY_ID);
  if (deferedInvalidRgn)
    delete deferedInvalidRgn;

  api->unregisterRootWnd(this);
  hwnd = NULL;
}

int BaseWnd::init(RootWnd *parWnd, int nochild) {
  if (parWnd == NULL) return 0;
  HWND phwnd = parWnd->gethWnd();
  ASSERT(phwnd != NULL);
  int ret = init(HINSTANCEfromHWND(phwnd), phwnd, nochild);
  if (ret) parentWnd = parWnd;	// set default parent wnd
  return ret;
}

int BaseWnd::init(HINSTANCE hInstance, HWND parent, int nochild) {
  RECT r;
  int w, h;

  ASSERTPR(gethWnd() == NULL, "don't you double init you gaybag");

  hinstance = hInstance;

  ASSERT(hinstance != NULL);

  register_wndClass(hinstance);

  if (parent != NULL) {
    GetClientRect(parent, &r);
  } else {
    r.left = 0;
    r.right = getPreferences(SUGGESTED_W);
    r.top = 0;
    r.bottom = getPreferences(SUGGESTED_H);
  }

  w = (r.right - r.left);
  h = (r.bottom - r.top);

  rwidth = w;
  rheight = h;
  rx = r.left;
  ry = r.top;

  // create the window
  RootWnd *rootwnd = (RootWnd *)this;
  int style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  int exstyle=0;
  if (parent == NULL) {
    exstyle |= WS_EX_TOOLWINDOW;
    style |= WS_POPUP;
  } else
    style |= WS_CHILD;

  if (nochild) style=WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

  if (acceptExternalDrops()) exstyle |= WS_EX_ACCEPTFILES;

  hwnd = CreateWindowEx(exstyle, BASEWNDCLASSNAME, NULL, style,
    r.left, r.top, w, h, parent, NULL, hinstance, (LPVOID)rootwnd);

  ASSERT(hwnd != NULL);
  if (hwnd == NULL) return 0;

  SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
    SWP_NOMOVE|SWP_NOSIZE|(start_hidden ? 0 : SWP_SHOWWINDOW)|SWP_DEFERERASE);//hop

  nreal++;

#ifdef URLDROPS
  if (acceptExternalDrops()) RegisterDragDrop(hwnd, &m_target);
#endif

  onInit();

  return 1;
}

int (WINAPI *BaseWnd::getRandomRgn)(HDC dc, HRGN rgn, int i)=NULL;
int BaseWnd::nreal=0;
int BaseWnd::grrfailed=0;
HINSTANCE BaseWnd::gdi32instance=NULL;

int BaseWnd::onInit() {
  api->registerRootWnd(this);
  return 0;
}

int BaseWnd::getCursorType(int x, int y) {
  return BASEWND_CURSOR_POINTER;
}

void BaseWnd::onSetName() {
  if (isInited())
    SetWindowText(gethWnd(), getNameSafe());
}

HWND BaseWnd::gethWnd() {
  return isVirtual() ? getParent()->gethWnd() : hwnd;
}

HINSTANCE BaseWnd::gethInstance() {
  return hinstance;
}

int BaseWnd::setTimer(int id, int ms) {
  ASSERTPR(id != 0, "id cannot be 0");
  ASSERTPR(gethWnd() != NULL, "Can't set a timer until window is inited");
  if (gethWnd() == NULL) return 0;
  return !SetTimer(gethWnd(), id, ms, NULL);
}

void BaseWnd::timerCallback(int id) { 
  switch (id) {
    case TIP_TIMER_ID:

      tipshowtimer=FALSE;
      tip_done = TRUE;
      killTimer(TIP_TIMER_ID);
  
      POINT p;
      Std::getMousePos(&p.x, &p.y);
      if (api->rootWndFromPoint(&p) == (RootWnd *)this) {
        ttip = new TipWnd; 
        ttip->setTip(tip);
        ttip->spawnTip(HINSTANCEfromHWND(gethWnd()), this);
        setTimer(TIP_DESTROYTIMER_ID, TIP_LENGTH);
        tipdestroytimer=TRUE;
      }
      setTimer(TIP_AWAY_ID, TIP_AWAY_DELAY);
      tipawaytimer=TRUE;
      break;
    case TIP_DESTROYTIMER_ID:
      killTimer(TIP_DESTROYTIMER_ID);
      killTimer(TIP_AWAY_ID);
      tipawaytimer=FALSE;
      tipdestroytimer=FALSE;
      if (ttip) {
        delete ttip;
        ttip = NULL;
      }
      break;
    case TIP_AWAY_ID:
      onTipMouseMove();
      break;
  }
}

int BaseWnd::killTimer(int id) {
  return !KillTimer(gethWnd(), id);
}

int BaseWnd::isInited() {
  return gethWnd() != NULL;
}

int BaseWnd::isDestroying() {
  return destroying;
}

int BaseWnd::wantDoubleClicks() {
  return TRUE;
}

int BaseWnd::wantSiblingInvalidations() {
  return FALSE;
}

void BaseWnd::setRSize(int x, int y, int w, int h) {
  rwidth = w;
  rheight = h;
  rx = x;
  ry = y;
}

void BaseWnd::resize(int x, int y, int w, int h) {

  RECT oldsize, newsize={x,y,w,h};
  if (hwnd != NULL) 
    BaseWnd::getNonClientRect(&oldsize);

  setRSize(x, y, w, h);
  
  int enlarge=0;
  if (handleRatio() && renderRatioActive()) {
    enlarge=1;
    multRatio(&w, &h);
    if (getParent() != NULL) {
      multRatio(&x, &y);
    }
  }

	if (gethWnd() != NULL) {

	SetWindowPos(gethWnd(), NULL, x, y, w, h,
	SWP_NOZORDER |
	SWP_NOACTIVATE |
	(ncb ? SWP_NOCOPYBITS : 0) | 
	( (x == -1 && y == -1) ? SWP_NOMOVE : 0) |
	( (w == -1 && h == -1) ? SWP_NOSIZE : 0) |
	0);

	onResize();

	if (ncb)
    invalidate();
  else {
    RECT r;
    if (hwnd != NULL) {
      if (newsize.left == oldsize.left && newsize.top == oldsize.top) {
        if (newsize.right > oldsize.right) { // growing in width
          r.left = oldsize.right;
          r.right = newsize.right;
          r.top = newsize.top;
          r.bottom = newsize.bottom;
          invalidateRect(&r);
          if (newsize.bottom > oldsize.bottom) { // growing in width & height
            r.left = oldsize.left;
            r.right = newsize.right;
            r.top = oldsize.bottom;
            r.bottom = newsize.bottom;
            invalidateRect(&r);
          }
        } else if (newsize.bottom > oldsize.bottom) {
          if (newsize.bottom > oldsize.bottom) { // growing in height
            r.left = oldsize.left;
            r.right = newsize.right;
            r.top = oldsize.bottom;
            r.bottom = newsize.bottom;
            invalidateRect(&r);
          }
        }
      }
    }
  }
 }
}


int BaseWnd::onResize() {
  return FALSE;
}

void BaseWnd::resizeToClient(BaseWnd *wnd) {
  if (wnd != NULL)
    wnd->resize(&clientRect());
}

int BaseWnd::onPostedMove() {
  return FALSE;
}

void BaseWnd::resize(RECT *r) {
  resize(r->left, r->top, r->right-r->left, r->bottom-r->top);
}

void BaseWnd::move(int x, int y) {
	if (!ncb)
      SetWindowPos(gethWnd(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_DEFERERASE); 
	else
      SetWindowPos(gethWnd(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_DEFERERASE);
}

void BaseWnd::moveToFront() {
  SetWindowPos(gethWnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE|SWP_DEFERERASE);
}

void BaseWnd::moveToBack() {
  SetWindowPos(gethWnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE|SWP_DEFERERASE);
}

void BaseWnd::setVisible(int show) {
  if (isVisible() && show) return;
  invalidate();
  if (hwnd != NULL) ShowWindow(gethWnd(), show ? SW_SHOWNA : SW_HIDE);
  onSetVisible(show);
}

void BaseWnd::onSetVisible(int show) { }

void BaseWnd::enable(int en) {
  EnableWindow(gethWnd(), en);
  onEnable(en);
}

int BaseWnd::getEnabled() {
  return IsWindowEnabled(hwnd);
}

int BaseWnd::onEnable(int en) {
  return 0;
}

void BaseWnd::setFocus() {
  SetFocus(gethWnd());
}

Region *BaseWnd::getDeferedInvalidRgn() {
  return deferedInvalidRgn;
}

void BaseWnd::deferedInvalidate() {
  if (!hasVirtualChildren() || !isVisible()) return; 
  RECT r;
  getNonClientRect(&r);
  deferedInvalidateRect(&r);
}

void BaseWnd::deferedInvalidateRect(RECT *r) {
  if (!hasVirtualChildren()) return; 
  Region h(r);
  deferedInvalidateRgn(&h);
}

void BaseWnd::deferedInvalidateRgn(Region *h) {
  if (!hasVirtualChildren()) return; 
  if (!deferedInvalidRgn) {
    deferedInvalidRgn = new Region();
  }

  deferedInvalidRgn->add(h);
}

void BaseWnd::deferedValidate() {
  if (!hasVirtualChildren() || !isVisible()) return; 
  RECT r;
  getNonClientRect(&r);
  deferedValidateRect(&r);
}

void BaseWnd::deferedValidateRect(RECT *r) {
  if (!hasVirtualChildren()) return; 
  Region *h = new Region(r);
  deferedValidateRgn(h);
  delete r;
}

void BaseWnd::deferedValidateRgn(Region *h) {
  if (!hasVirtualChildren()) return; 
  if (!deferedInvalidRgn) return;

  deferedInvalidRgn->subtract(h);
}

int BaseWnd::hasVirtualChildren() {
  return 1; //virtualChildren.getNumItems() > 0;
}

void BaseWnd::invalidate() {
  invalidateFrom(this);
}

void BaseWnd::invalidateFrom(RootWnd *who) {
  if (hasVirtualChildren()) deferedInvalidate();
  if (hwnd != NULL && isVisible()) InvalidateRect(gethWnd(), NULL, FALSE);
}

void BaseWnd::invalidateRectFrom(RECT *r, RootWnd *who) {
  if (hasVirtualChildren()) deferedInvalidateRect(r);
  Region *rg = new Region(r);
  invalidateRgnFrom(rg, who);
  delete rg;
}

void BaseWnd::invalidateRgn(Region *r) {
  invalidateRgnFrom(r, this);
}

void BaseWnd::invalidateRect(RECT *r) {
  invalidateRectFrom(r, this);
}

void BaseWnd::invalidateRgnFrom(Region *r, RootWnd *who) {
  static int inhere=0;
  if (hwnd != NULL && isVisible()) {
    if (hasVirtualChildren()/* && !inhere*/) {
      inhere=1;
      Region *_r = r->clone();
      int j = virtualChildren.searchItem(who);
      for (int i=0;i<virtualChildren.getNumItems();i++) {
        RootWnd *w = virtualChildren[i];
        if (w != who && w->wantSiblingInvalidations())
          w->onSiblingInvalidateRgn(_r, who, j, i);
      }

      inhere=0;
	  
      deferedInvalidateRgn(_r);
      physicalInvalidateRgn(_r);
      r->disposeClone(_r);
    } else {
      deferedInvalidateRgn(r);
      physicalInvalidateRgn(r);
    }
  }
}

void BaseWnd::physicalInvalidateRgn(Region *r) {
  if (hwnd != NULL && isVisible()) {
    if (ABS(getRenderRatio()-1.0) > 0.01) {
      Region *clone = r->clone();
      clone->scale(getRenderRatio(),TRUE);
      InvalidateRgn(gethWnd(), clone->getHRGN(), FALSE);
      r->disposeClone(clone);
    } else
      InvalidateRgn(gethWnd(), r->getHRGN(), FALSE);
  }
}

void BaseWnd::validate() {
  if (hwnd != NULL) ValidateRect(gethWnd(), NULL);
}

void BaseWnd::validateRect(RECT *r) {
  if (hwnd != NULL) {
    if (ABS(getRenderRatio()-1.0) > 0.01) {
      RECT r2 = *r;
      r2.left = (int)((double)r2.left * getRenderRatio());
      r2.top = (int)((double)r2.top * getRenderRatio());
      r2.right = (int)((double)r2.right * getRenderRatio());
      r2.bottom = (int)((double)r2.bottom * getRenderRatio());
      ValidateRect(gethWnd(), &r2);
    } else
    ValidateRect(gethWnd(), r);
  }
}

void BaseWnd::validateRgn(Region *reg) {
  if (hwnd != NULL) {
    if (ABS(getRenderRatio()-1.0) > 0.01) {
      Region *clone = reg->clone();
      clone->scale(getRenderRatio());
      ValidateRgn(gethWnd(), clone->getHRGN());
      reg->disposeClone(clone);
    } else
      ValidateRgn(gethWnd(), reg->getHRGN());
  }
}

void BaseWnd::repaint() {
/*	if (hasVirtualChildren())	{
	  Region *h = new Region();
	  int s = GetUpdateRgn(gethWnd(), h->getHRGN(), FALSE);
	  if (s != NULLREGION && s != ERROR) {
  		virtualDrawRgn(h);
	  }
	  delete h;
	}*/
  if (hwnd != NULL) UpdateWindow(gethWnd());
}

void BaseWnd::getClientRect(RECT *rect) {
/*  rect->left = rx;
  rect->right = rx + rwidth;
  rect->top = ry;
  rect->bottom = ry + rheight;*/
  ASSERT(hwnd != NULL);
  if (getRenderRatio() == 1.0)
    GetClientRect(gethWnd(), rect);
  else {
    GetClientRect(gethWnd(), rect);
    rect->right = rect->left + rwidth;
    rect->bottom = rect->left + rheight;
  }
}

RECT BaseWnd::clientRect() {
  RECT ret;
  getClientRect(&ret);
  return ret;
}

void BaseWnd::getNonClientRect(RECT *rect) {
//  ASSERT(hwnd != NULL);
  if (!hwnd) getClientRect(rect);
  else {
    if (getRenderRatio() == 1.0)
      GetClientRect(gethWnd(), rect);
    else {
      GetClientRect(gethWnd(), rect);
      rect->right = rect->left + rwidth;
      rect->bottom = rect->left + rheight;
    }
  }
/*  rect->left = rx;
  rect->right = rx + rwidth;
  rect->top = ry;
  rect->bottom = ry + rheight;*/
}

RECT BaseWnd::nonClientRect() {
  RECT ret;
  getNonClientRect(&ret);
  return ret;
}

void BaseWnd::getWindowRect(RECT *rect) {
#ifdef WIN32
  ASSERT(hwnd != NULL);
  GetWindowRect(gethWnd(), rect);
#else
#error port me
#endif
}

RECT BaseWnd::windowRect() {
  RECT ret;
  getWindowRect(&ret);
  return ret;
}


void BaseWnd::clientToScreen(int *x, int *y) {
  POINT p;
  p.x = x ? *x : 0;
  p.y = y ? *y : 0;
  if (ABS(getRenderRatio()-1.0) > 0.01) {
    p.x = (int)((double)p.x * getRenderRatio());
    p.y = (int)((double)p.y * getRenderRatio());
  }
  ClientToScreen(gethWnd(), &p);
  if (x) *x = p.x;
  if (y) *y = p.y;
}

void BaseWnd::clientToScreen(RECT *r) {
  clientToScreen((int*)&r->left, (int*)&r->top);
  clientToScreen((int*)&r->right, (int*)&r->bottom);
}

void BaseWnd::clientToScreen(POINT *p) {
  clientToScreen((int *)&p->x, (int *)&p->y);
}

void BaseWnd::screenToClient(int *x, int *y) {
  POINT p;
  p.x = x ? *x : 0;
  p.y = y ? *y : 0;
  ScreenToClient(gethWnd(), &p);
  if (ABS(getRenderRatio()-1.0) > 0.01) {
    p.x = (int)((double)p.x / getRenderRatio());
    p.y = (int)((double)p.y / getRenderRatio());
  }
  if (x) *x = p.x;
  if (y) *y = p.y;
}

void BaseWnd::screenToClient(RECT *r) {
  screenToClient((int*)&r->left, (int*)&r->top);
  screenToClient((int*)&r->right, (int*)&r->bottom);
}

void BaseWnd::screenToClient(POINT *p) {
  screenToClient((int *)&p->x, (int *)&p->y);
}


void BaseWnd::setParent(RootWnd *newparent) {
  ASSERTPR(newparent != NULL, "quit being a weeny");
  ASSERTPR(parentWnd == NULL || newparent == parentWnd, "can't reset parent");
  parentWnd = newparent;
  if (isInited()) {
    HWND w1 = gethWnd();
    HWND w2 = newparent->gethWnd();
    if (w1 != w2)
      SetParent(w1, w2);
  }
}

int BaseWnd::reparent(RootWnd *newparent) {
  RootWnd *old = getParent();
  if (!old && newparent) {
    ::SetParent(gethWnd(), newparent->gethWnd());
    SetWindowLong(gethWnd() , GWL_STYLE, GetWindowLong(gethWnd(), GWL_STYLE) & ~WS_POPUP);
    SetWindowLong(gethWnd() , GWL_STYLE, GetWindowLong(gethWnd(), GWL_STYLE) | WS_CHILD);
  } else if (old && !newparent) {
    SetWindowLong(gethWnd() , GWL_STYLE, GetWindowLong(gethWnd(), GWL_STYLE) & ~WS_CHILD);
    SetWindowLong(gethWnd() , GWL_STYLE, GetWindowLong(gethWnd(), GWL_STYLE) | WS_POPUP);
    ::SetParent(gethWnd(), NULL);
  } else {
    ::SetParent(gethWnd(), newparent ? newparent->gethWnd() : NULL);
  }
  parentWnd = newparent;
  onSetParent(newparent);
  return 1;
}

RootWnd *BaseWnd::getParent() {
  return parentWnd;
}

RootWnd *BaseWnd::getRootParent() {
  return this;
}

int BaseWnd::notifyParent(int msg, int param1, int param2) {
  RootWnd *notifywnd = getNotifyWindow();
  if (getParent() == NULL && notifywnd == NULL) return 0;
  if (notifywnd == NULL) notifywnd = getParent();
  ASSERT(notifywnd != NULL);
  return notifywnd->childNotify(this, msg, param1, param2);
}

DragInterface *BaseWnd::getDragInterface() {
  return this;
}

RootWnd *BaseWnd::rootWndFromPoint(POINT *pt) {
  // pt is in client coordinates
  int x = (int)((double)pt->x / getRenderRatio());
  int y = (int)((double)pt->y / getRenderRatio());

  RootWnd *ret = getVirtualChild(x, y);
  if (ret == NULL) ret = this;
  return ret;
}

int BaseWnd::getSkinId() {
  return skin_id;
}

int BaseWnd::onMetricChange(int metricid, int param1, int param2) {
  switch (metricid) {
    case Metric::TEXTDELTA:
      return setFontSize(-1);
  }
  return 0;
}

int BaseWnd::rootwnd_onPaint(CanvasBase *canvas, Region *r) {
  BaseCloneCanvas c(canvas);
  return onPaint(&c, r);
}

int BaseWnd::rootwnd_paintTree(CanvasBase *canvas, Region *r) {
  BaseCloneCanvas c(canvas);
  return paintTree(&c, r);
}

void BaseWnd::setSkinId(int id) {
  skin_id = id;
}


int BaseWnd::getPreferences(int what) {
  switch (what) {
    case SUGGESTED_W: return 320;
    case SUGGESTED_H: return 200;
    case SUGGESTED_X: return 0;
    case SUGGESTED_Y: return 0;
  }
  return 0;
}

void BaseWnd::setStartHidden(int wtf) {
  start_hidden = wtf;
}

LRESULT BaseWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

  if (!isDestroying()) switch (uMsg) {
    case WM_CREATE:
      hwnd = hWnd;
    break;

    case WM_CLOSE:
      return 0;

    case WM_PAINT: {
      ASSERT(hwnd != NULL);
      if (!isVisible() || IsIconic(gethWnd())) break;
      RECT r;
      if (GetUpdateRect(hWnd, &r, FALSE)) {
//        if (!hasVirtualChildren() && onPaint(NULL)) { return 0; }
	      if (/*hasVirtualChildren() && */virtualOnPaint()) { return 0; }
      }
    }
    break;

    case WM_NCPAINT: return 0;
    case WM_SYNCPAINT: return 0;

    case WM_SETCURSOR:
     if (checkModal()) return TRUE;
     if (hWnd == (HWND)wParam) {
        int ct = BASEWND_CURSOR_POINTER;
        int _x, _y;
        Std::getMousePos(&_x, &_y);
        screenToClient(&_x, &_y);
        if (!handleVirtualChildMsg(WM_SETCURSOR,_x,_y, &ct)) {
          ct = getCursorType(_x, _y);
        }
        char *wincursor;
        switch (ct) {
          case BASEWND_CURSOR_USERSET: /* do nothing */ break;
          default:
          case BASEWND_CURSOR_POINTER:
            wincursor = IDC_ARROW;
          break;
          case BASEWND_CURSOR_NORTHSOUTH:
            wincursor = IDC_SIZENS;
          break;
          case BASEWND_CURSOR_EASTWEST:
            wincursor = IDC_SIZEWE;
          break;
          case BASEWND_CURSOR_NORTHWEST_SOUTHEAST:
            wincursor = IDC_SIZENWSE;
          break;
          case BASEWND_CURSOR_NORTHEAST_SOUTHWEST:
            wincursor = IDC_SIZENESW;
          break;
          case BASEWND_CURSOR_4WAY:
            wincursor = IDC_SIZEALL;
          break;
        }
        SetCursor(LoadCursor(NULL, wincursor));
      }
    return TRUE;

    case WM_TIMER:
      if (wParam >= VCHILD_TIMER_ID_MIN && wParam <= VCHILD_TIMER_ID_MAX && (((int)wParam-VCHILD_TIMER_ID_MIN) < virtualChildTimers.getNumItems())) {
        virtualChildTimer *t = &virtualChildTimers[wParam-VCHILD_TIMER_ID_MIN];
        RootWnd *child = t->child;
        child->timerCallback(t->id);
        return 0;
      }
      timerCallback(wParam);
    return 0;

    case WM_SETFOCUS:
      onGetFocus();
    break;

    case WM_KILLFOCUS:
      if(api) api->forwardOnKillFocus(); // resets the keyboard active keys buffer
      onKillFocus();
    break;

    case UMSG_DEFERRED_CALLBACK: {
      RootWnd *origin = ((defered_callback *)lParam)->origin;
      int p1 = ((defered_callback *)lParam)->param1;
      int p2 = ((defered_callback *)lParam)->param2;
      delete (defered_callback *)lParam;
      origin->onDeferredCallback(p1, p2);
      break;
    }

    // dragging and dropping

    case WM_LBUTTONDOWN: {
      if (checkModal()) return 0;
      abortTip();
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
			if (!getCapture() && hasVirtualChildren())
			  if (handleVirtualChildMsg(WM_LBUTTONDOWN,xPos,yPos)) 
			    return 0;
      if (!dragging) {
        int r = onLeftButtonDown(xPos, yPos);
        if (checkDoubleClick(uMsg, xPos, yPos)) if (onLeftButtonDblClk(xPos, yPos)) return 0;
        return r;
      }
		}
    break;

    case WM_RBUTTONDOWN: {
      if (checkModal()) return 0;
      abortTip();
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
			if (!getCapture() && hasVirtualChildren())
			  if (handleVirtualChildMsg(WM_RBUTTONDOWN,xPos,yPos)) 
			    return 0;
      if (!dragging) {
        int r = onRightButtonDown(xPos, yPos);
        if (checkDoubleClick(uMsg, xPos, yPos)) if (onRightButtonDblClk(xPos, yPos)) return 0;
        return r;
      }
		}
    break;

    case WM_MOUSEMOVE: {
      if (checkModal()) return 0;
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (dragging) {
        POINT pt={xPos, yPos};
        clientToScreen(&pt);
        RootWnd *targ;
        int candrop = 0;
        // find the window the mouse is over

        targ = NULL;
        if (stickyWnd) {
          RECT wr;
          GetWindowRect(stickyWnd->gethWnd(), &wr);
          if (pt.x >= wr.left-sticky.left &&
              pt.x <= wr.right + sticky.right &&
              pt.y >= wr.top - sticky.top &&
              pt.y <= wr.bottom + sticky.bottom) targ = stickyWnd;
          else stickyWnd = NULL;
        }

        if (targ == NULL && api) targ = api->rootWndFromPoint(&pt); // FG> not to self, check

        DI prevtargdi(prevtarg);
        DI targdi(targ);

        if (prevtarg != targ) {	// window switch
          if (prevtarg != NULL) prevtargdi.dragLeave(this);
          if (targ != NULL) targdi.dragEnter(this);
        }
        if (targ != NULL)
          candrop = targdi.dragOver(pt.x, pt.y, this);
        if (targ == NULL || !candrop)
          SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_NO)));
        else
          SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_APPSTARTING)));
        prevtarg = targ;
      } else {
        tipbeenchecked=FALSE;
        if (!getCapture() && hasVirtualChildren()) {
          if (handleVirtualChildMsg(WM_MOUSEMOVE, xPos, yPos))
            return 0;
        }
        if (getCapture()) {
          if (onMouseMove(xPos, yPos)) 
            return 0;
        }
        if (!tipbeenchecked) onTipMouseMove();
        return 0;
      }
    }
    break;

    case WM_LBUTTONUP: {
      if (checkModal()) return 0;
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      abortTip();
      if (!dragging && !getCapture() && hasVirtualChildren()) {
        if (handleVirtualChildMsg(WM_LBUTTONUP,xPos,yPos)) 
          return 0;
      }
      if (dragging) {
        clientToScreen(&xPos, &yPos);
        int res = 0;
        if (prevtarg != NULL) {
          res = DI(prevtarg).dragDrop(this, xPos, yPos);
        }

        // inform source what happened
        dragComplete(res);

        resetDragSet();
        prevtarg = NULL;
        stickyWnd = NULL;
        suggestedTitle = NULL;
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        ReleaseCapture();
        dragging = 0;
      } else {
        if (onLeftButtonUp(xPos, yPos)) return 0;
      }
    }
    break;

    case WM_RBUTTONUP: {
      if (checkModal()) return 0;
      abortTip();
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      xPos = (int)((float)xPos / getRenderRatio());
      yPos = (int)((float)yPos / getRenderRatio());
      if (!getCapture() && hasVirtualChildren()) {
        if (handleVirtualChildMsg(WM_RBUTTONUP,xPos,yPos))
          return 0;
      }
      if (!dragging) if (onRightButtonUp(xPos, yPos)) return 0;
    }
    break;

    case WM_CONTEXTMENU: {
      if (checkModal()) return 0;
      ASSERT(hWnd != NULL);
      int xPos = (signed short)LOWORD(lParam);
      int yPos = (signed short)HIWORD(lParam);        
      if (hWnd == gethWnd()) {
        if (onContextMenu(xPos, yPos)) return 0;
      } else if (GetParent(hWnd) == gethWnd()) {
        if (onChildContextMenu(xPos, yPos)) return 0;
      }
    }
    break;

    case WM_ERASEBKGND:
      return (onEraseBkgnd((HDC)wParam));

    case WM_MOUSEWHEEL: {
      abortTip();
      
      int l,a,t=LOWORD(wParam);;
      l=(short)HIWORD(wParam)/120;
      a=(short)HIWORD(wParam);
      if (!l)
        if (a > 0) l=1;
      else if (a < 0)l=0;
      a=l>=0?l:-l;
      if (GetAsyncKeyState(VK_MBUTTON)&0x8000) {
        if (l>=0) l=0; // Fast Forward 5s
        else l=1; // Rewind 5s
      } else {
        if (l>=0) l=2; // Volume up
        else l=3; // Volume down
        a*=2;
      }

      if (l & 1)
        return onMouseWheelDown(!(BOOL)(l&2), a);
      else
        return onMouseWheelUp(!(BOOL)(l&2), a);
    }

    case WM_WA_RELOAD: {
      if (wParam == 0)
          freeResources();
        else
          reloadResources();
      return 0;
      }

    case WM_WA_GETFBSIZE: {
      SIZE *s = (SIZE *)wParam;
      s->cx = rwidth;
      s->cy = rheight;
      return 0;
      }
      
    case WM_WA_CONTEXTMENUNOTIFY: {
      int action=(int)wParam;
      int param=(int)lParam;
      return onContextMenuNotify(action, param);
    }
      
    case WM_WA_ACTIONNOTIFY: {
      int action=(int)wParam;
      int param=(int)lParam;
      return onActionNotify(action, param);
    }

    case WM_USER+8976: // wheel in tip, delete tip
      abortTip();
      return 0;

    case WM_CHAR:
      if (curVirtualChildFocus == NULL) {
        if (onChar((char)((TCHAR) wParam))) return 0;
      } else {
        if (curVirtualChildFocus->onChar((char)((TCHAR) wParam))) return 0;
      }
      if (api && api->forwardOnChar((TCHAR) wParam)) return 0;
      break;

    case WM_KEYDOWN:
      if (curVirtualChildFocus == NULL) {
        if (onKeyDown((int) wParam)) return 0;
      } else {
        if (curVirtualChildFocus->onKeyDown((int)wParam)) return 0;
      }
      if (api && api->forwardOnKeyDown((int) wParam)) return 0;
      break;

    case WM_KEYUP:
      if (curVirtualChildFocus == NULL) {
        if (onKeyUp((int) wParam)) return 0;
      } else {
        if (curVirtualChildFocus->onKeyUp((int)wParam)) return 0;
      }
      if (api && api->forwardOnKeyUp((int) wParam)) return 0;
      break;

    case WM_MOUSEACTIVATE: {
      if (checkModal())
        return MA_NOACTIVATE;
      SetFocus(gethWnd());
      return MA_ACTIVATE;
      }
      
    case WM_ACTIVATE: {
      int fActive = LOWORD(wParam);
      if (fActive == WA_ACTIVE || fActive == WA_CLICKACTIVE)
        onActivate();
      else
        onDeactivate();
    }
    return 0;

    case WM_WINDOWPOSCHANGED:
      onPostedMove();
      return 0;

    case WM_DROPFILES: {
      api->pushModalWnd();
      onExternalDropBegin();
      HDROP h = (HDROP)wParam;
      POINT dp;
      DragQueryPoint(h, &dp);
      clientToScreen(&dp);
      // build a file list
      char buf[MAX_PATH];
      PtrList<FilenamePS> keep;
      
      SetCursor(LoadCursor(NULL, IDC_WAIT));
      int nfiles = DragQueryFile(h, 0xffffffff, buf, sizeof(buf));
      // convert them all to PlayItem *'s
      for (int i = 0; i < nfiles; i++) {
        DragQueryFile(h, i, buf, sizeof(buf));
        addDroppedFile(buf, &keep);	// recursive
      }
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      dragging = 1;
      if (dragEnter(this)) {
        if (dragOver(dp.x, dp.y, this)) dragDrop(this, dp.x, dp.y);
      } else {
        dragLeave(this);
      }
      dragging = 0;

      // remove data
      keep.deleteAll();
      resetDragSet();

      onExternalDropEnd();
      api->popModalWnd();
    }
    return 0;	// dropfiles
  }

  if (uMsg >= WM_USER) {
    int ret;
    if (onUserMessage(uMsg, wParam, lParam, &ret))
      return ret;
    return 0;
  }

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int BaseWnd::onUserMessage(int msg, int w, int l, int *r) {
  return 0;
}

BaseWnd::checkDoubleClick(int b, int x, int y) {

  DWORD now = Std::getTickCount();

  switch (b) {
    case WM_LBUTTONDOWN:
      if (lastClick[0] > now - Std::getDoubleClickDelay()) {
        lastClick[0] = 0;
        if (abs(lastClickP[0].x - x) > Std::getDoubleClickX() || abs(lastClickP[0].y - y) > Std::getDoubleClickY()) return 0;
        return 1;
      }
      lastClick[0] = now;
      lastClickP[0].x = x;
      lastClickP[0].y = y;
      break;

    case WM_RBUTTONDOWN:
      if (lastClick[1] > now - Std::getDoubleClickDelay()) {
        lastClick[1] = 0;
        if (abs(lastClickP[1].x - x) > Std::getDoubleClickX() || abs(lastClickP[1].y - y) > Std::getDoubleClickY()) return 0;
        return 1;
      }
      lastClick[1] = now;
      lastClickP[1].x = x;
      lastClickP[1].y = y;
      break;
    }

  return 0;
}

BaseWnd::onMouseWheelUp(int click, int lines) {
  return 0;
}

BaseWnd::onMouseWheelDown(int click, int lines) {
  return 0;
}

int BaseWnd::onContextMenu(int x, int y) {
  return 0;
}

int BaseWnd::onChildContextMenu(int x, int y) {
  return 0;
}

void BaseWnd::postDeferredCallback(int param1, int param2) {
  defered_callback *c = new defered_callback;
  c->origin = this;
  c->param1 = param1;
  c->param2 = param2;
  PostMessage(hwnd, UMSG_DEFERRED_CALLBACK, (WPARAM)NULL, (LPARAM)c);
}

int BaseWnd::onDeferredCallback(int param1, int param2) {
  switch (param1) {
    case DEFERREDCB_FLUSHPAINT:
      do_flushPaint();
      break;
  }
  return 0;
}

int BaseWnd::onPaint(Canvas *canvas) {
#if 0
  // example:
  PaintCanvas c;
  if (!c.beginPaint(this)) return 0;
  (do some painting)
  c will self-destruct on return
#endif
  return 0;
}

int BaseWnd::onPaint(Canvas *canvas, Region *h) {

  if (!canvas) return onPaint(canvas);

  int sdc = SaveDC(canvas->getHDC());

  canvas->selectClipRgn(h);

	int rs = onPaint(canvas);

  RestoreDC(canvas->getHDC(), sdc);

	return rs;
}

void BaseWnd::setTransparency(int amount) {
#ifdef WIN32
  if (isVirtual()) return;

  if (user32instance == NULL) user32instance = LoadLibrary("USER32.DLL");
  if (user32instance == NULL) return;

  if (setLayeredWindowAttributes == NULL) {
    if (checked_for_alpha_proc) return;
    setLayeredWindowAttributes=(void (__stdcall *)(HWND,int,int,int))GetProcAddress(user32instance ,"SetLayeredWindowAttributes");
    checked_for_alpha_proc = 1;
    if (setLayeredWindowAttributes == NULL) return;
  }

  if (amount < 0) amount = 0;
  else if (amount > 255) amount = 255;

  DWORD dwLong = GetWindowLong(hwnd, GWL_EXSTYLE);
  if (amount==255) {
    if (dwLong & WS_EX_LAYERED)
      SetWindowLong(hwnd, GWL_EXSTYLE, dwLong & ~WS_EX_LAYERED);
  } else {
    if (!(dwLong & WS_EX_LAYERED))
      SetWindowLong(hwnd, GWL_EXSTYLE, dwLong | WS_EX_LAYERED)
;
    setLayeredWindowAttributes(hwnd, RGB(0,0,0), amount, LWA_ALPHA);
  }
#endif
}

int BaseWnd::beginCapture() {
  if (!getCapture())
  {
    curVirtualChildCaptured = NULL;
    oldCapture = SetCapture(gethWnd());
    if (oldCapture) {
      OutputDebugString("Stolen capture detected, this may be ok, but try to avoid it if possible. Saving old capture\n");
    }
    inputCaptured = 1;
  }
  return 1;
}

int BaseWnd::onMouseMove(int x, int y) { 
  onTipMouseMove();
  return 0;
}

void BaseWnd::onTipMouseMove() {
  POINT p;

  if (dragging) return;
  return;

  tipbeenchecked = TRUE;

  Std::getMousePos(&p.x, &p.y);

  if (api->rootWndFromPoint(&p) != (RootWnd *)this) { // leaving area
    tip_done = FALSE;
    if (tipawaytimer)
      killTimer(TIP_AWAY_ID);
    tipawaytimer = FALSE;
    if (tipshowtimer) 
      killTimer(TIP_TIMER_ID);
    tipshowtimer = FALSE;
    if (ttip) {
      delete ttip;
      ttip = NULL;
    }
  } else { // moving in area
    if (!tipshowtimer && !tip_done && tip != NULL && *tip) { //entering area & need tip
      setTimer(TIP_TIMER_ID, TIP_TIMER_THRESHOLD);
      tipshowtimer=TRUE;
    } else if (tipshowtimer) {
      killTimer(TIP_TIMER_ID);
      setTimer(TIP_TIMER_ID, TIP_TIMER_THRESHOLD);
    }
  }
}

int BaseWnd::endCapture() {
  if (!inputCaptured) return 0;
  ReleaseCapture();
  if (oldCapture) { 
    OutputDebugString("Restoring old capture\n");
    SetCapture(oldCapture); 
    oldCapture = NULL; 
  }
  inputCaptured = 0;
  return 1;
}

int BaseWnd::getCapture() {
  if (inputCaptured && GetCapture() == gethWnd() && curVirtualChildCaptured == NULL) return 1;
  return 0;
}

int BaseWnd::onLeftButtonDblClk(int x, int y) {
  return 0;
}

int BaseWnd::onRightButtonDblClk(int x, int y) {
  return 0;
}

int BaseWnd::onGetFocus() {	// return TRUE if you override this
  if (curVirtualChildFocus == NULL && hasVirtualChildren()) {
    if (!wantFocus())
      focusNextVirtualChild(NULL); // focus first virtual child that accepts focus
  }
  hasfocus = 1;
  notifyParent(CHILD_GOTFOCUS);
  return 1;
}

int BaseWnd::onKillFocus() {	// return TRUE if you override this
  hasfocus = 0;
  if (curVirtualChildFocus)
    curVirtualChildFocus->onKillFocus();
  else
    notifyParent(CHILD_KILLFOCUS);
  curVirtualChildFocus = NULL;
  return 1;
}

int BaseWnd::childNotify(RootWnd *child, int msg, int p1, int p2) {
  return 0;
}

int BaseWnd::addDragItem(char *droptype, void *item) {
  DragSet *set;
  int pos = dragCheckData(droptype);
  if (pos == -1) {
    set = new DragSet();
    set->setName(droptype);
    dragsets.addItem(set);
    pos = dragsets.getNumItems() - 1;
  } else set = dragsets[pos];
  set->addItem(item);
  return pos;
}

int BaseWnd::handleDrag() {
  abortTip();
  if (dragsets.getNumItems() == 0) return 0;

  SetCapture(hwnd);
  SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_APPSTARTING)));

  dragging = 1;
  stickyWnd = NULL;

  return 1;
}

int BaseWnd::resetDragSet() {
  dragsets.deleteAll();
  return 1;
}

int BaseWnd::dragEnter(RootWnd *sourceWnd) {
  RootWnd *rw = getParent(); //RootWnd::rootwndFromRootWnd(getParent()); //FG> note to self, check!
  if (rw) return DI(rw).dragEnter(sourceWnd);
  return 0;
}

int BaseWnd::dragSetSticky(RootWnd *wnd, int left, int right, int up, int down){
  ASSERT(dragging);
  stickyWnd = wnd;

  if (left < 0) left = 0;
  if (right < 0) right = 0;
  if (up < 0) up = 0;
  if (down < 0) down = 0;

  Std::setRect(&sticky, left, up, right, down);

  return 1;
}

void BaseWnd::setSuggestedDropTitle(const char *title) {
  ASSERT(title != NULL);
  suggestedTitle = title;
}

const char *BaseWnd::dragGetSuggestedDropTitle() {
  return suggestedTitle;	// can be NULL
}

int BaseWnd::dragCheckData(const char *type, int *nitems) {
  for (int i = 0; i < dragsets.getNumItems(); i++) {
    if (STRCASEEQL(type, dragsets[i]->getName())) {
      if (nitems != NULL) *nitems = dragsets[i]->getNumItems();
      return i;
    }
  }
  return -1;
}

void *BaseWnd::dragGetData(int slot, int itemnum) {
  if (slot < 0 || slot >= dragsets.getNumItems()) return 0;
  if (itemnum < 0 || itemnum >= dragsets[slot]->getNumItems()) return 0;
  return dragsets[slot]->enumItem(itemnum);
}

void BaseWnd::addDroppedFile(const char *filename, PtrList<FilenamePS> *plist) {
  const char *slash = filename + STRLEN(filename) - 1;
  for (; slash > filename; slash--) if (*slash == '/' || *slash == '\\') break;
  if (STREQL(slash+1, ".") || STREQL(slash+1, "..")) return;

  char buf[WA_MAX_PATH];
  STRCPY(buf, filename);
  // try to resolve shortcuts
  char *ext = buf + STRLEN(buf) - 1;
  for (; ext > buf; ext--) if (*ext == '.' || *ext == '\\' || *ext == '/') break;
  if (!STRICMP(ext, ".lnk")) {
    char buf2[MAX_PATH];
    if (ResolveShortCut(gethWnd(), buf, buf2)) STRCPY(buf, buf2);
  }

  int isdir = 0;

  // handle root dir specially?
  
  WIN32_FIND_DATA data;
  HANDLE r = FindFirstFile(buf, &data);
  if (!r) return;
  FindClose(r);
  if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) isdir = 1;

  if (isdir) {
    onExternalDropDirScan(buf);
    // enumerate that dir
    char search[WA_MAX_PATH];
    wsprintf(search, "%s\\*.*", buf);
    HANDLE files = FindFirstFile(search, &data);
    if (files == INVALID_HANDLE_VALUE) return;	// nothin' in it
    for (;;) {
      char obuf[WA_MAX_PATH];
      wsprintf(obuf, "%s\\%s", buf, data.cFileName);
      addDroppedFile(obuf, plist);
      if (!FindNextFile(files, &data)) {
        FindClose(files);
        return;
      }
    }
    // should never get here
  } else {
    addDragItem(DD_FILENAME, plist->addItem(new FilenamePS(StringPrintf("file:%s", buf))));
  }
}

static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  // fetch out the RootWnd *
  if (uMsg == WM_CREATE) {
    CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
    ASSERT(cs->lpCreateParams != NULL);
    // stash pointer to self
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)cs->lpCreateParams);
  }

  // pass the messages into the BaseWnd
  RootWnd *rootwnd = (RootWnd*)GetWindowLong(hWnd, GWL_USERDATA);
  if (rootwnd == NULL) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  } else {
    return rootwnd->wndProc(hWnd, uMsg, wParam, lParam);
  }
}

static void register_wndClass(HINSTANCE hInstance) {

  WNDCLASS wc;
  if (GetClassInfo(hInstance, BASEWNDCLASSNAME, &wc)) return;

  // regiszter pane class
  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC)wndProc;
  wc.cbClsExtra		= 0;
  wc.cbWndExtra		= 0;
  wc.hInstance		= hInstance;
  wc.hIcon		= NULL;
  wc.hCursor		= NULL;
  wc.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
  wc.lpszMenuName	= NULL;
  wc.lpszClassName	= BASEWNDCLASSNAME;

  int r = RegisterClass(&wc);
  if (r == 0) {
    int res = GetLastError();
    if (res != ERROR_CLASS_ALREADY_EXISTS) {
      char florp[WA_MAX_PATH];
      wsprintf(florp, "Failed to register class, err %d", res);
      ASSERTPR(0, florp);
    }
  }
}

BOOL BaseWnd::getNoCopyBits(void) {
  return ncb;
}

void BaseWnd::setNoCopyBits(BOOL newncb) {
  ncb = newncb;
}

int BaseWnd::onEraseBkgnd(HDC dc) {
/*  RECT r;
  DCCanvas c;
  c.cloneDC(dc, this);
  GetUpdateRect(gethWnd(), &r, FALSE);
  api->skin_renderBaseTexture(getBaseTextureWindow(), &c, r);*/
  return 1; 
}

void BaseWnd::setIcon(HICON icon, int _small) {
  SendMessage(gethWnd(), WM_SETICON, _small ? ICON_SMALL : ICON_BIG, (int)icon);
}

const char *BaseWnd::getTip() {
  return tip;
}

void BaseWnd::setTip(const char *tooltip) {
  if (tip) FREE(tip);
  if (tooltip)
    tip = STRDUP(tooltip);
  else
    tip = NULL;
  abortTip();
}

int BaseWnd::getStartHidden() {
  return start_hidden;
}

void BaseWnd::abortTip() {
  if (tipshowtimer)
    killTimer(TIP_TIMER_ID);
  tipshowtimer=FALSE;
  if (tipawaytimer)
    killTimer(TIP_AWAY_ID);
  tipawaytimer=FALSE;
  if (tipdestroytimer)
    killTimer(TIP_DESTROYTIMER_ID);
  tipdestroytimer=FALSE;
  if (ttip) {
    delete ttip;
    ttip = NULL;
  }
  tip_done=FALSE;
  RECT r;
  if (gethWnd() && GetUpdateRect(gethWnd(), &r, FALSE) != 0) // FG> avoids xoring over disapearing tip
    repaint();
}

int BaseWnd::isVisible() {
  return ::IsWindowVisible(gethWnd());
}

void BaseWnd::addVirtualChild(RootWnd *child) {
	virtualChildren.addItem(child);
}

void BaseWnd::removeVirtualChild(RootWnd *child) {
  if (curVirtualChildCaptured == child) {
    setVirtualChildCapture(NULL);
  }
  if (curVirtualChildFocus == child)
    curVirtualChildFocus = NULL;
  virtualChildren.removeItem(child);
}

int BaseWnd::isVirtual() { 
  return 0; 
}

inline int isInRect(RECT *r,int x,int y) {
	if(x>=r->left&&x<=r->right&&y>=r->top&&y<=r->bottom) return 1;
	return 0;
}

int BaseWnd::ensureVirtualCanvasOk() {
	RECT ncr;

  if (isVirtual() && getParent()) return 1;

	int size_w = rwidth;
	int size_h = rheight;
                    
	if(!isVisible()) return 0;
	if(!size_w||!size_h) return 0;

  if(!virtualCanvas || virtualCanvasH!=size_h || virtualCanvasW!=size_w) {
    if(virtualCanvas) {
      deleteFrameBuffer(virtualCanvas);
      virtualCanvas=NULL;
    }
    virtualCanvas = createFrameBuffer(size_w, size_h);
    prepareFrameBuffer(virtualCanvas, size_w, size_h);
		virtualCanvas->setBaseWnd(this);
		virtualCanvasH=size_h; virtualCanvasW=size_w;
		BaseWnd::getNonClientRect(&ncr);
		deferedInvalidateRect(&ncr);
	}
  return 1;
}

Canvas *BaseWnd::createFrameBuffer(int w, int h) {
	return new BltCanvas(w,h);      
}

void BaseWnd::prepareFrameBuffer(Canvas *canvas, int w, int h) {
  virtualBeforePaint();
  RECT r={0,0,w, h};
  canvas->selectClipRgn(NULL);
  canvas->fillRect(&r, 0);
  virtualAfterPaint();
}

void BaseWnd::deleteFrameBuffer(Canvas *canvas) {
  delete canvas;
}

// paints the client content, followed by the virtual child tree. recursive
int BaseWnd::paintTree(Canvas *canvas, Region *r) {

  onPaint(canvas, r);

  if (isVirtual() && !hasVirtualChildren()) return 0;

  deferedValidateRgn(r);

  Region *client_update = new Region();
  for (int i=0;i<virtualChildren.getNumItems();i++) {
    if (!virtualChildren[i]->isVisible()) continue;
    RECT rChild;
    virtualChildren[i]->getNonClientRect(&rChild);
    if ((rChild.right != rChild.left) && (rChild.bottom != rChild.top))
    if (r->intersects(&rChild, client_update)) {
      virtualChildren[i]->paintTree(canvas, client_update);
    }
  }
  delete client_update;
  return 1;
}

void BaseWnd::setVirtualCanvas(Canvas *c) {
  virtualCanvas = c;
}

int BaseWnd::pointInWnd(POINT *p) {
  RECT r;
  if (!isVisible()) return 0;
  getWindowRect(&r);
  if (!Std::pointInRect(&r, *p))
    return 0;
  for(int i=0;i<virtualChildren.getNumItems();i++) {
    if (!virtualChildren[i]->isVisible()) continue;
    RECT rChild;
    virtualChildren[i]->getWindowRect(&rChild);
    if (Std::pointInRect(&rChild, *p))
      return 0;
  }
  //NONPORTABLE
  HWND child = GetWindow(gethWnd(), GW_CHILD);
  while (child != NULL) {
    if (IsWindowVisible(child)) {
      RECT r2;
      GetWindowRect(child, &r2);
      if (Std::pointInRect(&r2, *p))
        return 0;
    }
    child = GetWindow(child, GW_HWNDNEXT);
  }
  return 1;
}

int BaseWnd::virtualOnPaint() {

  if (gdi32instance == NULL && !grrfailed) {
    gdi32instance=LoadLibrary("GDI32.dll");
    if (gdi32instance != NULL) {
      getRandomRgn = (int (WINAPI *)(HDC,HRGN,int)) GetProcAddress(gdi32instance,"GetRandomRgn");
      if (getRandomRgn == NULL) {
        grrfailed=1;
        FreeLibrary(gdi32instance);
        gdi32instance=NULL;
      }
    } else {
      grrfailed=1;
    }
  }

  if (!ensureVirtualCanvasOk()) return 0;

  virtualBeforePaint();

  Region *r = new Region();

  GetUpdateRgn(gethWnd(), r->getHRGN(), FALSE);

  PaintCanvas paintcanvas;
  if (!paintcanvas.beginPaint(this)) { virtualAfterPaint(); return 0; }

  // DO NOT DELETE - This looks like it does nothing, but it actually makes the GDI call us again with WM_PAINT if some window
  // moves over this one between BeginPaint and EndPaint. We still use GetUpdateRgn so we don't have to check for
  // the version of Windows. See doc. If this function is not available (should be here in 95/98/NT/2K, but we never know)
  // then we use the rcPaint rect... less precise, but still works.
/*  if (getRandomRgn) {
    Region *zap = new Region();
    getRandomRgn(paintcanvas.getHDC(), zap->getHRGN(), SYSRGN);
    delete zap;
  } else { 
    RECT z;
    paintcanvas.getRcPaint(&z);
    r->setRect(&z);
  }*/

  // -------------


  RECT rcPaint2;
  r->getBox(&rcPaint2);

//  if (!r->isEmpty() && ratio != 1.0)
//  	r->scale(1.0/ratio);

/*// for debug
  HDC dc = GetDC(gethWnd());
  InvertRgn(dc, r->getHRGN());
  InvertRgn(dc, r->getHRGN());
  ReleaseDC(gethWnd(), dc);*/


  RECT rcPaint;
  r->getBox(&rcPaint);

  double ra = getRenderRatio();

  if (deferedInvalidRgn) {
    Region *nr=NULL;
    if (ABS(ra-1.0) > 0.01) {
      nr=r->clone();
      nr->scale(1.0/ra);
    }
    Region *i = new Region();
    if (deferedInvalidRgn->intersects(nr?nr:r, i)) { // some deferednvalidated regions needs to be repainted
      paintTree(virtualCanvas, i);
  	}
    delete i;
    if (nr) r->disposeClone(nr);
  }

  int prec=0;


  // if fractionnal part of ratio is not 0, we'll have to stretchblit from an integer pixel to another integer pixel so as to avoid
  // coordinates stretching bugs. The easiest way to do that is just to blit the framebuffer and rely on the GDI's clipping region to
  // optimize this blit for us. Feel free to optimize ;)

  virtualAfterPaint();

  virtualCanvasCommit(&paintcanvas, &rcPaint, ra);

/*
  //DEBUG
  BaseWnd::getClientRect(&rcPaint);

  HDC dc = GetDC(NULL);
  BitBlt(dc, 0, 0, rcPaint.right-rcPaint.left, rcPaint.bottom-rcPaint.top, virtualCanvas->getHDC(), 0, 0, SRCCOPY);
  ReleaseDC(NULL, dc);
  // END DEBUG
*/

  delete r;
  return 1;
}

RootWnd *BaseWnd::getVirtualChild(int _enum) {
  return virtualChildren[_enum];
}

int BaseWnd::getNumVirtuals() {
  return virtualChildren.getNumItems();
}

RootWnd *BaseWnd::getVirtualChild(int x,int y) {
  int _x=x, _y=y;
  clientToScreen(&_x, &_y);
  for(int i=virtualChildren.getNumItems()-1;i>-1;i--) {
    RECT r;
    RootWnd *child = virtualChildren[i];
    child->getNonClientRect(&r);
    if (child->isVisible() && x>=r.left&&x<=r.right&&y>=r.top&&y<=r.bottom && !child->isClickThrough() && child->mouseInRegion(x, y)) {
      if (child->getVirtualChild(0)) 
        return child->getVirtualChild(x, y);
      return child;
    }
  }
  return mouseInRegion(x, y) ? this : NULL;
}

int BaseWnd::handleVirtualChildMsg(UINT uMsg, int x, int y, void *p) {
  RootWnd *child=NULL;

  if(curVirtualChildCaptured) 
    child=curVirtualChildCaptured;
  else 
    child=getVirtualChild(x, y);

  if (child) {
    switch(uMsg) {
      case WM_LBUTTONDOWN:
        if (child != curVirtualChildFocus)
          focusVirtualChild(child);
        child->onLeftButtonDown(x, y);
        if (child->checkDoubleClick(uMsg, x, y))
          child->onLeftButtonDblClk(x, y);
      return 1;
      case WM_RBUTTONDOWN:
        if (child != curVirtualChildFocus)
          focusVirtualChild(child);
        child->onRightButtonDown(x, y);
        if (child->checkDoubleClick(uMsg, x, y))
          child->onRightButtonDblClk(x, y);
      return 1;
      case WM_LBUTTONUP:
        child->onLeftButtonUp(x, y);
      return 1;
      case WM_RBUTTONUP:
        child->onRightButtonUp(x, y);
      return 1;
      case WM_MOUSEMOVE:{
        if (curVirtualChildCaptured == child || (curVirtualChildCaptured == NULL)) {
          child->onMouseMove(x, y);
          return 1;
        }
        return 0;
      }
      case WM_SETCURSOR:
        if (!p) return 0;
        *(int *)p = child->getCursorType(x, y);
        return 1;
    }
  }
  return 0;
}

int BaseWnd::onLeftButtonDown(int x, int y) {
 setFocus();
 return 1;
}

void BaseWnd::setVirtualChildCapture(RootWnd *child) {
  if (child) {
    if (!inputCaptured) {
      beginCapture();
    }
  } else {
    endCapture();
  }
  curVirtualChildCaptured = child;
}

RootWnd *BaseWnd::getVirtualChildCapture() {
  if (inputCaptured && GetCapture() == gethWnd())
    return curVirtualChildCaptured;
  else
    if (inputCaptured) inputCaptured=0;
  return NULL;
}

int BaseWnd::setVirtualChildTimer(RootWnd *child, int id, int ms) {
  virtualChildTimer t;
  t.id=id;
  t.child=child;
  int nb;
  if ((nb=virtualChildTimers.getItemPos(t))==-1) {
    virtualChildTimers.addItem(t);
    return(setTimer(VCHILD_TIMER_ID_MIN+virtualChildTimers.getNumItems()-1,ms));
  } else {
    return(setTimer(VCHILD_TIMER_ID_MIN+nb,ms));
  }
}

int BaseWnd::killVirtualChildTimer(RootWnd *child, int id) {
  virtualChildTimer t;
  t.id=id;
  t.child=child;
  int nb=virtualChildTimers.getItemPos(t);
  if(nb==-1) return 0;
  return(killTimer(VCHILD_TIMER_ID_MIN+nb));
}

RootWnd *BaseWnd::getBaseTextureWindow() {
  // return our base texture window if we have it
  if (btexture)
    return btexture;
  // return our parent's if they have it
  if (getParent())
    return getParent()->getBaseTextureWindow();
  else
    return NULL;
}

void BaseWnd::renderBaseTexture(CanvasBase *c, RECT &r) {
  api->skin_renderBaseTexture(getBaseTextureWindow(), c, r, this);
}

void BaseWnd::setBaseTextureWindow(RootWnd *w) {
  btexture = w;
}

void BaseWnd::setNotifyWindow(RootWnd *newnotify) {
  notifyWindow = newnotify;
}

RootWnd *BaseWnd::getNotifyWindow() {
  return destroying ? NULL : notifyWindow;
}

int BaseWnd::gotFocus() {
  return hasfocus;
}

int BaseWnd::isActive() {
  HWND h = hwnd;
  if (h == NULL) h = getParent()->gethWnd();
  if (h == NULL) return 0;
  return (GetActiveWindow() == h);
}

int BaseWnd::onChar(char c) {
/*  switch (c) {
    case 9: // TAB
        focusNextSibbling(1);
      return 1;
  }*/
  return 0;
}

int BaseWnd::focusNextSibbling(int dochild) {
  return 1;
}

int BaseWnd::focusNextVirtualChild(BaseWnd *child) {
  return 1;
}

int BaseWnd::focusVirtualChild(RootWnd *child) {
  return 1;
}

int BaseWnd::wantFocus() {
  return 0;
}

// Return 1 if there is a modal window that is not this
int BaseWnd::checkModal() {
  RootWnd *w = api->getModalWnd();
  if (w && w != (RootWnd*)this) {
    return 1;
  }
  return 0;
}

int BaseWnd::cascadeRepaintFrom(RootWnd *who) {
  RECT r;
  BaseWnd::getNonClientRect(&r);
  return BaseWnd::cascadeRepaintRect(&r);
}

int BaseWnd::cascadeRepaint() {
  return cascadeRepaintFrom(this);
}

int BaseWnd::cascadeRepaintRgn(Region *r) {
  return cascadeRepaintRgnFrom(r, this);
}

int BaseWnd::cascadeRepaintRect(RECT *r) {
  return cascadeRepaintRectFrom(r, this);
}

int BaseWnd::cascadeRepaintRectFrom(RECT *r, RootWnd *who) {
  Region *reg = new Region(r);
  int rt = cascadeRepaintRgnFrom(reg, who);
  delete reg;
  return rt; 
}


int BaseWnd::cascadeRepaintRgnFrom(Region *_rg, RootWnd *who) {
  if (!ensureVirtualCanvasOk()) return 0;

  Region *rg = _rg->clone();

  int j = virtualChildren.searchItem(who);
  for (int i=0;i<virtualChildren.getNumItems();i++) {
    RootWnd *w = virtualChildren[i];
    if (w != who && w->wantSiblingInvalidations()) 
      w->onSiblingInvalidateRgn(rg, who, j, i);
  }

  virtualBeforePaint();

  deferedInvalidateRgn(rg);
  paintTree(virtualCanvas, rg);

  DCCanvas *paintcanvas;
  HDC dc = GetDC(gethWnd());
  paintcanvas = new DCCanvas();
  paintcanvas->cloneDC(dc, this);

  virtualAfterPaint();

  double ra = getRenderRatio();

  RECT rcPaint;
  rg->getBox(&rcPaint);

  RECT rc;
  getClientRect(&rc); //JF> this gets it in virtual (non-scaled) coordinates,
                      // so we need to do these comparisons before scaling.
  rcPaint.bottom = MIN((int)rc.bottom, (int)rcPaint.bottom);
  rcPaint.right = MIN((int)rc.right, (int)rcPaint.right);
 
  if (ABS(ra-1.0) > 0.01) // probably faster than scaling the clone
  {
    rcPaint.left=(int)((rcPaint.left-1)*ra);
    rcPaint.top=(int)((rcPaint.top-1)*ra);
    rcPaint.right=(int)(rcPaint.right*ra + 0.999999);
    rcPaint.bottom=(int)(rcPaint.bottom*ra + 0.999999);
  }
  rcPaint.left = MAX(0, (int)rcPaint.left);
  rcPaint.top = MAX(0, (int)rcPaint.top);


  virtualCanvasCommit(paintcanvas, &rcPaint, ra);

	ReleaseDC(gethWnd(), dc);
	delete paintcanvas;
  _rg->disposeClone(rg);

	return 1;
}

void BaseWnd::virtualCanvasCommit(Canvas *paintcanvas, RECT *r, double ra) {
  if (ABS(ra-1.0) <= 0.01)
  	virtualCanvas->blit(r->left,r->top,paintcanvas,r->left,r->top,r->right-r->left,r->bottom-r->top);
	else
  {
    RECT tr=*r;

    double invra=65536.0/ra;
    int lp=tr.left;
    int tp=tr.top;
    int w=tr.right-tr.left;
    int h=tr.bottom-tr.top;

    int sx=(int)((double)lp * invra);
    int sy=(int)((double)tp * invra);
    int sw=(int)((double)w * invra);
    int sh=(int)((double)h * invra);
   
  	virtualCanvas->stretchblit(
      sx,sy,sw,sh,
      paintcanvas, lp, tp, w, h);
  }
}

void BaseWnd::flushPaint() {
  postDeferredCallback(DEFERREDCB_FLUSHPAINT, 0);
}

void BaseWnd::do_flushPaint() {
  if (!deferedInvalidRgn || deferedInvalidRgn->isEmpty()) return;
  Region *r = deferedInvalidRgn->clone();
  cascadeRepaintRgn(r);
  deferedInvalidRgn->disposeClone(r);
}

int BaseWnd::isMouseOver(int x, int y) {
  POINT pt={x, y};
  clientToScreen(&pt);

  return (api->rootWndFromPoint(&pt) == this);
}

void BaseWnd::freeResources() {
}

void BaseWnd::reloadResources() {
  invalidate(); 
}

double BaseWnd::getRenderRatio() {
  if (!handleRatio()) return 1.0;
  return getParent() ? getParent()->getRenderRatio() : ratio;
}

void BaseWnd::setRenderRatio(double r) {
  // "snap" to 1.0
  if (ABS(r - 1.0) <= 0.02f) r = 1.0;
  if (isInited() && r != ratio && !isVirtual() && !getParent()) {
    // must scale size & region accordingly
    RECT rc;
    BaseWnd::getWindowRect(&rc);
    rc.right = rc.left + rwidth;
    rc.bottom = rc.top + rheight;
    ratio = r;
    resize(&rc);

/*    Region *reg = new Region;
    GetWindowRgn(gethWnd(), reg->getHRGN());
    if (!reg->isEmpty()) {
      reg->scale(getRenderRatio());
      SetWindowRgn(gethWnd(), reg->getHRGN(), TRUE);
    }
    delete reg;*/
    invalidate();
  }
}

int BaseWnd::renderRatioActive() {
  return ABS(getRenderRatio() - 1.0) > 0.01f;
}

void BaseWnd::multRatio(int *x, int *y) {
  double rr = getRenderRatio();
  *x = (int)((double)(*x) * rr);
  if (y) *y = (int)((double)(*y) * rr);
}

void BaseWnd::multRatio(RECT *r) {
  double rr = getRenderRatio();
  r->left = (int)((double)(r->left) * rr);
  r->right = (int)((double)(r->right) * rr);
  r->top = (int)((double)(r->top) * rr);
  r->bottom = (int)((double)(r->bottom) * rr);
}

void BaseWnd::divRatio(int *x, int *y) {
  double rr = getRenderRatio();
  *x = (int)((double)(*x) / rr);
  if (y) *y = (int)((double)(*y) / rr);
}

void BaseWnd::divRatio(RECT *r) {
  double rr = getRenderRatio();
  r->left = (int)((double)(r->left) / rr);
  r->right = (int)((double)(r->right) / rr);
  r->top = (int)((double)(r->top) / rr);
  r->bottom = (int)((double)(r->bottom) / rr);
}

void BaseWnd::bringVirtualToFront(RootWnd *w) {
  changeChildZorder(w, 0);
}

void BaseWnd::bringVirtualToBack(RootWnd *w) {
  changeChildZorder(w, virtualChildren.getNumItems());
}

void BaseWnd::bringVirtualAbove(RootWnd *w, RootWnd *b) {
  ASSERT(b->isVirtual());
  int p = virtualChildren.searchItem(b);
  if (p == -1) return;
  changeChildZorder(w, p);
}

void BaseWnd::bringVirtualBelow(RootWnd *w, RootWnd *b) {
  ASSERT(b->isVirtual());
  int p = virtualChildren.searchItem(b);
  if (p == -1) return;
  changeChildZorder(w, p+1);
}

void BaseWnd::changeChildZorder(RootWnd *w, int newpos) {
  int p = newpos;
  p = MAX(p, (int)0);
  p = MIN(p, virtualChildren.getNumItems());
  RECT cr;
  w->getClientRect(&cr);

  PtrList<RootWnd> l;
  for (int i=0;i<virtualChildren.getNumItems();i++)
    if (virtualChildren[i] != w)
      l.addItem(virtualChildren[i]);

  p = virtualChildren.getNumItems() - newpos -1;
  virtualChildren.removeAll();

  int done = 0;

  for (i=0;i<l.getNumItems();i++)
    if (i==p && !done) {
      virtualChildren.addItem(w);
      i--;
      done++;
    } else {
      RECT dr;
      RECT intersection;
      l.enumItem(i)->getClientRect(&dr);
      if (IntersectRect(&intersection, &dr, &cr))
        l[i]->invalidateRect(&intersection);
      virtualChildren.addItem(l.enumItem(i));
    }
  if (i==p && !done) 
    virtualChildren.addItem(w);
  w->invalidate();
}

int BaseWnd::onActivate() {
  if(hasVirtualChildren()) {
    int l=getNumVirtuals();
    for(int i=0;i<l;i++) {
      RootWnd *r=getVirtualChild(i);
      r->onActivate();
    }
  }
  return 0;
}

int BaseWnd::onDeactivate() {
  if(hasVirtualChildren()) {
    int l=getNumVirtuals();
    for(int i=0;i<l;i++) {
      RootWnd *r=getVirtualChild(i);
      r->onDeactivate();
    }
  }
  return 0;
}