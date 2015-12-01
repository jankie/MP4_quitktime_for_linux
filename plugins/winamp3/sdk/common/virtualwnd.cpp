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

#include "virtualwnd.h"
#include "region.h"
#include "../studio/api.h"
#include "../common/usermsg.h"

VirtualWnd::VirtualWnd() {
	virtualX = virtualY = virtualH = virtualW = 0;
	inited = 0;
	visible = 0;
  bypassvirtual = 0;
}

VirtualWnd::~VirtualWnd() {
  if (!bypassvirtual) {
    if (getParent())
      getParent()->removeVirtualChild(this);
  }
}

int VirtualWnd::init(RootWnd *parWnd, int nochild) {
  if (!bypassvirtual) 
    setParent(parWnd);
  return(VIRTUALWND_PARENT::init(parWnd,nochild));
}

int VirtualWnd::init(HINSTANCE hInstance, HWND parent, int nochild) {
  if (!bypassvirtual) {
    ASSERTPR(getParent()!=NULL, "Virtual window created without specifying BaseWnd parent");
    getParent()->addVirtualChild(this);
    if (getStartHidden())
      visible = 0;
    else
      visible = 1;
    inited = 1;
	  onInit();
	  return 1;
  } else
    return VIRTUALWND_PARENT::init(hInstance, parent, nochild);
}

HWND VirtualWnd::gethWnd() {
//	ASSERTPR(getParent() != NULL, "Virtual window used as base parent !");
  if (!bypassvirtual) {
	  if(!getParent()) return NULL;
	  return(getParent()->gethWnd());
  } else
   return VIRTUALWND_PARENT::gethWnd();
}

HINSTANCE VirtualWnd::gethInstance() {
  if (!bypassvirtual) {
    if(!getParent()) return NULL;
    return HINSTANCEfromHWND(getParent()->gethWnd());
  } else
   return VIRTUALWND_PARENT::gethInstance();
}

int VirtualWnd::setTimer(int id, int ms) {
  if (!bypassvirtual) {
    ASSERT(getRootParent() != NULL);
    return(getRootParent()->setVirtualChildTimer(this, id, ms));
  } else
   return VIRTUALWND_PARENT::setTimer(id, ms);
}

int VirtualWnd::killTimer(int id) {
  if (!bypassvirtual) {
    ASSERT(getRootParent() != NULL);
	  return(getRootParent()->killVirtualChildTimer(this, id));
  } else
   return VIRTUALWND_PARENT::killTimer(id);
}

void VirtualWnd::resize(RECT *r) {
  if (!bypassvirtual) {
    resize(r->left, r->top, r->right-r->left, r->bottom-r->top);
  } else
   VIRTUALWND_PARENT::resize(r);
}

void VirtualWnd::resize(int x, int y, int w, int h) {
  if (!bypassvirtual) {
    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

	  virtualX=x; virtualY=y;
	  if (w != AUTOWH) virtualW=w;
	  if (h != AUTOWH) virtualH=h;

    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

    setRSize(x, y, w, h);

	  onResize();
  } else
   VIRTUALWND_PARENT::resize(x, y, w, h);
}

//CUTvoid VirtualWnd::resize(RECT *r) {
//CUT  resize(r->left, r->top, r->right-r->left, r->bottom-r->top);
//CUT}

void VirtualWnd::move(int x, int y) {
  if (!bypassvirtual) {
    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }

	  virtualX=x; virtualY=y;

    if (isVisible()) {
      RECT r;
      getNonClientRect(&r);
      invalidateRect(&r);
    }
  } else
   VIRTUALWND_PARENT::move(x, y);
}

void VirtualWnd::invalidate() {
  if (!bypassvirtual) {
    if (!getRootParent()) return;
    RECT r(clientRect());
    getRootParent()->invalidateRectFrom(&r, this);
//	VIRTUALWND_PARENT::invalidate();	
  } else
   VIRTUALWND_PARENT::invalidate();
}

void VirtualWnd::invalidateRect(RECT *r) {
  if (!bypassvirtual) {
	  if(!getRootParent()) return;
	  getRootParent()->invalidateRectFrom(r, this);
  } else
   VIRTUALWND_PARENT::invalidateRect(r);
}

void VirtualWnd::invalidateRgn(Region *reg) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    getRootParent()->invalidateRgnFrom(reg, this);
  } else
   VIRTUALWND_PARENT::invalidateRgn(reg);
}

void VirtualWnd::validate() {
  if (!bypassvirtual) {
	  if(!getRootParent()) return;
	  RECT r;
	  getClientRect(&r);
	  getRootParent()->validateRect(&r);
  } else
   VIRTUALWND_PARENT::validate();
}

void VirtualWnd::validateRect(RECT *r) {
  if (!bypassvirtual) {
	  if(!getRootParent()) return;
	  getRootParent()->validateRect(r);
  } else
   VIRTUALWND_PARENT::validateRect(r);
}

void VirtualWnd::validateRgn(Region *reg) {
  if (!bypassvirtual) {
    if(!getRootParent()) return;
    getRootParent()->validateRgn(reg);
  } else
   VIRTUALWND_PARENT::validateRgn(reg);
}

void VirtualWnd::getClientRect(RECT *rect) {
  if (!bypassvirtual) {
	  // CT:getClientRect behaves differently here for virtual windows
	  //    so we can use onPaint directly on the destination canvas
	  //    without using another temporary canvas.
	  rect->left=virtualX; rect->right=virtualX+virtualW;
	  rect->top=virtualY; rect->bottom=virtualY+virtualH;
  //		rect->left=0; rect->right=virtualW;
  //		rect->top=0; rect->bottom=virtualH;
  } else
   VIRTUALWND_PARENT::getClientRect(rect);
}

/*void VirtualWnd::getNonClientRect(RECT *rect) {
  getClientRect(rect);
}*/

void VirtualWnd::getWindowRect(RECT *rect) {
  if (!bypassvirtual) {
	  RECT a;
	  getParent()->getWindowRect(&a);
	  rect->left=a.left+virtualX; rect->right=rect->left+virtualW;
	  rect->top=a.top+virtualY; rect->bottom=rect->top+virtualH;
  } else
   VIRTUALWND_PARENT::getWindowRect(rect);
}

int VirtualWnd::beginCapture() {
  if (!bypassvirtual) {
	  getRootParent()->setVirtualChildCapture(this);
	  return 1;
  } else
   return VIRTUALWND_PARENT::beginCapture();
}

int VirtualWnd::endCapture() {
  if (!bypassvirtual) {
    ASSERT(getRootParent() != NULL);
	  getRootParent()->setVirtualChildCapture(NULL);
	  return 1;
  } else
   return VIRTUALWND_PARENT::endCapture();
}

int VirtualWnd::getCapture() {
  if (!bypassvirtual) {
    ASSERT(getRootParent() != NULL);
    return getRootParent()->getVirtualChildCapture() == this;
  } else
   return VIRTUALWND_PARENT::getCapture();
}

/*void VirtualWnd::setVirtualChildCapture(BaseWnd *child) {
  if (!bypassvirtual) {
	  getParent()->setVirtualChildCapture(child);
  } else
   VIRTUALWND_PARENT::setVirtualChildCapture(child);
}

int VirtualWnd::setVirtualChildTimer(BaseWnd *child, int id, int ms) {
  if (!bypassvirtual) {
	  return(getParent()->setVirtualChildTimer(child, id, ms));
  } else
   return VIRTUALWND_PARENT::setVirtualChildTimer(child, id, ms);
}

int VirtualWnd::killVirtualChildTimer(BaseWnd *child, int id) {
  if (!bypassvirtual) {
	  return(getParent()->killVirtualChildTimer(child, id));
  } else
   return VIRTUALWND_PARENT::killVirtualChildTimer(child, id);
}*/

void VirtualWnd::setVisible(int show) {
  if (!bypassvirtual) {
    if (visible == !!show) return;
    visible = !!show;
    invalidate();
    onSetVisible(show);
  } else
   VIRTUALWND_PARENT::setVisible(show);
}

int VirtualWnd::isVisible() {
  if (!bypassvirtual) {
    return (visible && getParent()->isVisible());
  } else
   return VIRTUALWND_PARENT::isVisible();
}

// eek
void VirtualWnd::repaint() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->repaint(); 
  } else
   VIRTUALWND_PARENT::repaint();
}

int VirtualWnd::isInited() {
  if (!bypassvirtual) {
    return inited;
  } else
   return VIRTUALWND_PARENT::isInited();
}

// those two will need to touch WindowTracker::allWnd order as well as parent's virtualChildren list
void VirtualWnd::moveToFront() {
  //ASSERTPR(0, "Implement me!");
}

void VirtualWnd::moveToBack() {
  //ASSERTPR(0, "Implement me!");
}

/*int VirtualWnd::focusNextSibbling(int dochild) {
  return 1;
}

int VirtualWnd::focusNextVirtualChild(BaseWnd *child) {
  return 1;
}*/

int VirtualWnd::cascadeRepaint() {
  if (!bypassvirtual) {
    if (getRootParent()) {
      RECT r;
      VirtualWnd::getNonClientRect(&r);
      getRootParent()->cascadeRepaintRectFrom(&r, this);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaint();
}

int VirtualWnd::cascadeRepaintRect(RECT *r) {
  if (!bypassvirtual) {
    if (getRootParent()) {
      getRootParent()->cascadeRepaintRectFrom(r, this);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaintRect(r);
}

int VirtualWnd::cascadeRepaintRgn(Region *r) {
  if (!bypassvirtual) {
    if (getRootParent()) {
      getRootParent()->cascadeRepaintRgnFrom(r, this);
    }
    return 1;
  } else
   return VIRTUALWND_PARENT::cascadeRepaintRgn(r);
}

/*RootWnd *VirtualWnd::getWindowBehindMyself(int x, int y) {
  RECT r;
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    int n = getParent()->getNumVirtuals();

    RootWnd *c = NULL;

    for (int i=n-1;i>=0;i++) {
      c = getParent()->getVirtualChild(i);
      if (c == this) break;
    }

    i--;
    if (i < 0) return getParent();

    for (;i>=0; i--) {
      c = getParent()->getVirtualChild(i);
      c->getNonClientRect(&r);
      if (x>=r.left&&x<=r.right&&y>=r.top&&y<=r.bottom)
        return c;
    }
    return getParent();
  } else
   return NULL;
}*/

RootWnd *VirtualWnd::rootWndFromPoint(POINT *pt) {
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    return getParent()->rootWndFromPoint(pt);
  } else
   return VIRTUALWND_PARENT::rootWndFromPoint(pt);
}

double VirtualWnd::getRenderRatio() {
  if (!bypassvirtual) {
    if (!getParent()) return 1.0;
    return getParent()->getRenderRatio();
  } else
   return VIRTUALWND_PARENT::getRenderRatio();
}

void VirtualWnd::bringToFront() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualToFront(this);
  }/* else
   VIRTUALWND_PARENT::bringToFront();*/
}

void VirtualWnd::bringToBack() {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualToBack(this);
  } /*else
   VIRTUALWND_PARENT::bringToBack();*/
}

void VirtualWnd::bringAbove(BaseWnd *o) {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualAbove(this, o);
  }/* else
   VIRTUALWND_PARENT::bringAbove();*/
}

void VirtualWnd::bringBelow(BaseWnd *o) {
  if (!bypassvirtual) {
    if(!getParent()) return;
    getParent()->bringVirtualBelow(this, o);
  }/* else
   VIRTUALWND_PARENT::bringBelow();*/
}

void VirtualWnd::postDeferredCallback(int param1, int param2) {
  ASSERT(getRootParent() && getRootParent()->gethWnd());
  if (!bypassvirtual) {
    defered_callback *c = new defered_callback;
    c->origin = this;
    c->param1 = param1;
    c->param2 = param2;
    PostMessage(getRootParent()->gethWnd(), UMSG_DEFERRED_CALLBACK, (WPARAM)NULL, (LPARAM)c);
  } else
   VIRTUALWND_PARENT::postDeferredCallback(param1, param2);
}

int VirtualWnd::reparent(RootWnd *newparent) {
  if (!bypassvirtual) {
    if (getParent()) 
      getParent()->removeVirtualChild(this);
    parentWnd = NULL;
    newparent->addVirtualChild(this);
    onSetParent(newparent);
    newparent->invalidate();
    return 1;
  } else {
   return VIRTUALWND_PARENT::reparent(newparent);
  }
}

int VirtualWnd::setVirtual(int i) {

  ASSERT(!isInited()); // cut

  if (isInited()) return 0; 
  bypassvirtual = !i;
  return 1;
}

RootWnd *VirtualWnd::getRootParent() {
  if (!bypassvirtual) {
    if (!getParent()) return NULL;
    RootWnd *t = this;
    while (t->isVirtual()) {
      if (!t->getParent()) return NULL;
      t = t->getParent();
    }
    return t;
  } else {
    return VIRTUALWND_PARENT::getRootParent();
  }
}


/* todo:  setCursor 
          setFocus
          
   + real childs going invisible should deferedInvalidate their rect on their parent window if it has a virtualCanvas
*/          
          


// No need for screenToClient/clientToScreen overrides since the virtual's origin is the same as it's parent
