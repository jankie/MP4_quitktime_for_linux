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


#include "clickwnd.h"

#include "../studio/api.h"

ClickWnd::ClickWnd() {
  handleRight = TRUE;
  button = -1;
  mousedown = 0;
  captured = 0;
  hilite = 0;
  down = 0;
}

void ClickWnd::setHandleRightClick(int tf) {
  handleRight=tf;
}

int ClickWnd::getHandleRightClick() {
  return handleRight;
}

int ClickWnd::onLeftButtonDown(int x, int y) {
  CLICKWND_PARENT::onLeftButtonDown(x, y);
  abortTip();
  if (mouseInRegion(x, y))
    return onButtonDown(WM_LBUTTONDOWN, x, y);
  else
    return 1;
}

int ClickWnd::onRightButtonDown(int x, int y) {
  CLICKWND_PARENT::onRightButtonDown(x, y);
  abortTip();
  if (!handleRight) return 1;
  if (mouseInRegion(x, y))
    return onButtonDown(WM_RBUTTONDOWN, x, y);
  else
    return 1;
}

int ClickWnd::onLeftButtonUp(int x, int y) {
  CLICKWND_PARENT::onLeftButtonUp(x, y);
//jf
//  if (mouseInRegion())
    return onButtonUp(WM_LBUTTONUP, x, y);
//  else
//    return 1;
}

int ClickWnd::onRightButtonUp(int x, int y) {
  CLICKWND_PARENT::onRightButtonUp(x, y);
  //jf
  //if (mouseInRegion()) 
  if (!handleRight) {
    onRightPush(x, y);
    return 1;
  }
    return onButtonUp(WM_RBUTTONUP, x, y);
//  else
//    return 1;
}

int ClickWnd::onMouseMove(int x, int y) {
  POINT pos, rpos={x,y};
  int mouseover;

  CLICKWND_PARENT::onMouseMove(x, y);

  pos=rpos;
  clientToScreen(&pos);

  mouseover = (api->rootWndFromPoint(&pos) == static_cast<RootWnd *>(this) && mouseInRegion(x, y));

  if (!mouseover && (!mousedown || !Std::keyDown(button?MK_RBUTTON:MK_LBUTTON)))
  {
    onLeaveArea();
    endCapture();
    down = 0;
    mousedown = 0;
    captured = 0;
    hilite = 0;
    invalidate();
    return 1;
  }

  if (!getCapture() && mouseover) {	// capture to see when leave
    onEnterArea();
    beginCapture();
    captured = 1;
  }

  int lastdown = down;
  int lasthilite = hilite;
  hilite = mouseover;
  down = userDown() || (mouseover && mousedown);
  if (down != lastdown || hilite != lasthilite) invalidate();

  return 1;
}

int ClickWnd::onButtonDown(int which, int x, int y) {
  
  if (!wantClicks()) return 1;

  if (!getCapture()) {
    beginCapture();
    captured = 1;
  }
  mousedown = 1;
  down = 1;
  button = -1;
  if (which == WM_LBUTTONDOWN) button = 0;
  else if (which == WM_RBUTTONDOWN) button = 1;
  invalidate();

  return 1;
}

int ClickWnd::onButtonUp(int which, int x, int y) {

    // make sure same button
  if (button == 0 && which == WM_RBUTTONUP) return 1;
  if (button == 1 && which == WM_LBUTTONUP) return 1;

  if (!down) {
    onLeaveArea();
    endCapture();
    captured = 0;
    hilite = 0;
    mousedown = 0;
    return 1;
  }

    POINT pos={x,y};
    clientToScreen(&pos);

    int mouseover = (api->rootWndFromPoint(&pos) == (RootWnd *)this && mouseInRegion(x, y));
    if (!mouseover && captured) {
      onLeaveArea();
      endCapture();
      captured = 0;
      hilite = 0;
    }

    // it was down, process the event
    int a = down;
    down = 0;
    mousedown = 0;
    invalidate();

    if (a) {
      if (button == 0) onLeftPush(x, y);
      else if (button == 1) onRightPush(x, y);
    } else {		// was off button
      captured = 0;
      hilite = 0;
    }

    return 1;
}

void ClickWnd::onSetVisible(int show) {
  CLICKWND_PARENT::onSetVisible(show);
  if (!show && getCapture()) {
    endCapture();
    onLeaveArea();
  }
}
