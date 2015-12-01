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

//PORTABLE
#include "buttbar.h"
#include "notifmsg.h"
#include "buttwnd.h"
#include "std.h"

ButtBar::ButtBar() {
  split = 0;
  spacer = 1;
}

ButtBar::~ButtBar() {
  int i;
  for (i = 0; i < buttons.getNumItems(); i++) {
    ASSERT(buttons[i] != NULL);
    delete buttons[i];
  }
}

int ButtBar::onInit() {
  int i;

  BUTTBAR_PARENT::onInit();

  // create the buttons
  for (i = 0; i < buttons.getNumItems(); i++) {
    buttons[i]->init(this);
    buttons[i]->setParent(this);
  }

  // put them in place
  onResize();

  return 1;
}

int ButtBar::addChild(ButtonWnd *child) {
  buttons.addItem(child);
  if (isInited()) {
    child->init(this);
    child->setParent(this);
    onResize();
  }
  return 1;
}

int ButtBar::getWidth() {
  int w = 0;
  for (int i = 0; i < buttons.getNumItems(); i++) {
    w += buttons[i]->getWidth()+spacer;
  }
  return w;
}

int ButtBar::getHeight() {
  int h = 0;
  for (int i = 0; i < buttons.getNumItems(); i++) {
    h = MAX(h, buttons[i]->getHeight()+1);
  }
  return h;
}

int ButtBar::childNotify(RootWnd *child, int msg, int p1, int p2) {
  switch (msg) {
    case CHILD_NOTIFY_LEFTPUSH: {
      int ret;
      if (ret = onLeftPush(p1)) return ret;
      return notifyParent(msg, p1, p2);
    }
    break;
  }
  return BUTTBAR_PARENT::childNotify(child, msg, p1, p2);
}

int ButtBar::onResize() {
  int x;
  RECT r;

  getClientRect(&r);
  int height = r.bottom - r.top;
  if (split) {
    int w = (r.right - r.left) / buttons.getNumItems();
    int x = r.left;
    for (int i = 0; i < buttons.getNumItems(); i++) {
      if (i == buttons.getNumItems()-1) w = (r.right - r.left) - x;
      buttons[i]->resize(x, r.top, w, height);
      x += w;
    }
  } else {
    x = r.left;
    for (int i = 0; i < buttons.getNumItems(); i++) {
      int w = buttons[i]->getWidth()+spacer;
      buttons[i]->resize(x, r.top, w, height);
      x += w;
      if (x > r.right) break;
    }
  }

  return 1;
}
