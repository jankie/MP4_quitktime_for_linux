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

#include "contwnd.h"
#include "canvas.h"

#include "../studio/api.h"

ContWnd::~ContWnd() {
  children.deleteAll();
}

int ContWnd::addChild(BaseWnd *child, int x, int y, int w, int h, int invalidate_on_resize) {
  ChildSlot *cs = new ChildSlot(child, Std::makeRect(x, y, x+w, y+h), invalidate_on_resize);
  children.addItem(cs);
  return 1;
}

ContWnd::onInit() {

  CONTWND_PARENT::onInit();

  RECT r;
  getClientRect(&r);

  for (int i=0; i < children.getNumItems(); i++) {
    ChildSlot *cslot = children[i];
    BaseWnd *child = cslot->child;
    if (child->init(this) != 0) {
      child->setParent(this);
//      if (cslot.max)
//        cslot.child->resize(r.left, r.top, r.right, r.bottom);
//      else
        child->resize(&cslot->pos);
    }
  }
  return 1;
}

int ContWnd::onPaint(Canvas *canvas) {
  PaintCanvas pc;
  if (canvas == NULL) {
    pc.beginPaint(this);
    canvas = &pc;
  }

  CONTWND_PARENT::onPaint(canvas);

  RECT r;
  getClientRect(&r); // canvas will have a clipping region set by basewnd if needed

  renderBaseTexture(canvas, r);

  return 1;
}

int ContWnd::onResize() {
  RECT r;
  getClientRect(&r);
  for (int i=0;i<children.getNumItems();i++) {
    ChildSlot *cslot = children[i];
    RECT pos;
    if (cslot->child->handleRatio()) {
      pos.left = r.left + cslot->pos.left;
      pos.top = r.top + cslot->pos.top;
      pos.right = pos.left + (cslot->pos.right - cslot->pos.left);
      pos.bottom = pos.top + (cslot->pos.bottom - cslot->pos.top);
    } else {
      pos.left = (int)((double)r.left + cslot->pos.left * getRenderRatio());
      pos.top = (int)((double)r.top + cslot->pos.top * getRenderRatio());
      pos.right = (int)((double)pos.left + (cslot->pos.right - cslot->pos.left) * getRenderRatio());
      pos.bottom = (int)((double)pos.top + (cslot->pos.bottom - cslot->pos.top) * getRenderRatio());
    }
    cslot->child->resize(&pos);
    if (cslot->invalidate_on_resize) cslot->child->invalidate();
  }
  invalidate();
  return 1;
}
