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

#include "editwnd.h"
#include "canvas.h"
#include "notifmsg.h"

#include "../studio/assert.h"
#include "std.h"

#define ID_EDITCHILD 12

enum { IDLETIMER=8 };
#define IDLETIME 500

static LRESULT CALLBACK static_editWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  EditWnd *editwnd = (EditWnd *)GetWindowLong(hWnd, GWL_USERDATA);
  if (editwnd == NULL) return DefWindowProc(hWnd, uMsg, wParam, lParam);
  return editwnd->editWndProc(hWnd, uMsg, wParam, lParam);
}

EditWnd::EditWnd(char *buffer, int buflen) {
  beforefirstresize=1;
  editWnd = NULL;
  prevWndProc = NULL;
  setBuffer(buffer, buflen);
  maxlen = 0;
  retcode = EDITWND_RETURN_NOTHING;
  modal = 0;
  autoenter = 0;
  autoselect = 0;
//  bordered = 0;
}

EditWnd::~EditWnd() {
  killTimer(IDLETIMER);
  if (editWnd != NULL) {
    SetWindowLong(editWnd, GWL_USERDATA, (LONG)0);
    SetWindowLong(editWnd, GWL_WNDPROC, (LONG)prevWndProc);
    DestroyWindow(editWnd);
  }
  notifyParent(CHILD_RETURN_CODE, retcode);
}

int EditWnd::onInit() {
  EDITWND_PARENT::onInit();

  RECT r;
  getClientRect(&r);

  editWnd = CreateWindow("EDIT", NULL,
    WS_CHILD | ES_AUTOHSCROLL,
    r.left, r.top, r.right - r.left, r.bottom - r.top,
    gethWnd(), (HMENU)ID_EDITCHILD,
    gethInstance(), NULL);
  ASSERT(editWnd != NULL);

  if (outbuf != NULL) //FG
    setBuffer(outbuf, maxlen);

  // stash a pointer to us
  SetWindowLong(editWnd, GWL_USERDATA, (LONG)this);
  // subclass the edit control
  prevWndProc = (WNDPROC)SetWindowLong(editWnd, GWL_WNDPROC, (LONG)static_editWndProc);

  SendMessage(editWnd, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), FALSE);
  return 1;
}

int EditWnd::onPaint(Canvas *canvas) {
//  if (!bordered) return EDITWND_PARENT::onPaint(canvas);

  PaintCanvas paintcanvas;
  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  EDITWND_PARENT::onPaint(canvas);

  RECT r;
  getClientRect(&r);
  canvas->drawRect(&r, TRUE, RGB(0, 0, 0));	//SKIN
  
  return 1;
}

int EditWnd::onResize() {
  EDITWND_PARENT::onResize();
  RECT r;
  getClientRect(&r);
  if (1/*bordered*/) {
    r.top++;
    r.bottom--;
    r.left++;
    r.right--;
  }
  MoveWindow(editWnd, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE);

  if (beforefirstresize) { 
    ShowWindow(editWnd, SW_NORMAL); beforefirstresize=0;
    if (modal) {
      SetFocus(editWnd);
      if (getAutoSelect())
        SendMessage(editWnd, EM_SETSEL, 0, -1);
    }
  }

  return TRUE;
}

LRESULT EditWnd::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
#if 0
    case WM_CTLCOLOREDIT: {
      HDC hdc = (HDC)wParam;
      SetTextColor(hdc, RGB(0, 255, 0));		//SKIN
    }
    return (LRESULT)GetStockObject(WHITE_BRUSH);	//SKIN
#endif

    case WM_COMMAND: {
      switch (HIWORD(wParam)) {
        case EN_CHANGE: {
          if (maxlen > 0) {
            *outbuf = 0;
            int l = SendMessage(editWnd, WM_GETTEXT, (WPARAM)maxlen, (LPARAM)outbuf);
            if (l <= 0) break;
            outbuf[l] = 0;
            onEditUpdate();
          }
        }
        break;
        case EN_KILLFOCUS:
          onLoseFocus();
        break;
      }
    }
    break;
  }
  return EDITWND_PARENT::wndProc(hWnd, uMsg, wParam, lParam);
}

void EditWnd::setBuffer(char *buffer, int len) {
  if (buffer == NULL || len <= 0) return;
  ASSERT(len > 0);
  ASSERT(len < 0x7ffe);
  ASSERT(STRLEN(buffer) < len);

  if (editWnd != NULL) {
    SendMessage(editWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)buffer);
    SendMessage(editWnd, EM_LIMITTEXT, (WPARAM)len, (LPARAM)0);
    if (getAutoSelect()) {
      SetFocus(editWnd);
      SendMessage(editWnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
    }
  }

  outbuf = buffer;
  maxlen = len;
}

void EditWnd::getBuffer(char *buf, int _len) {
  if (_len > maxlen) _len = maxlen;
  MEMCPY(buf, outbuf, _len);
}

void EditWnd::setModal(int _modal) {
  modal = _modal;
}

void setBorder(int border) {
//  bordered = border;
}

LRESULT EditWnd::editWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch (uMsg) {
    case WM_CHAR:
      if (wParam == VK_RETURN) {
        if (onEnter()) return 0;
        return 0;
      } else if (wParam == VK_ESCAPE) {
        if (onAbort()) return 0;
      } 
    break;
  }
  return CallWindowProc(prevWndProc, hWnd, uMsg, wParam, lParam);
}

void EditWnd::timerCallback(int id) {
  switch (id) {
    case IDLETIMER:
      killTimer(IDLETIMER);
      onIdleEditUpdate();
    break;
    default:
      EDITWND_PARENT::timerCallback(id);
  }
}

void EditWnd::onEditUpdate() {
  killTimer(IDLETIMER);
  setTimer(IDLETIMER, IDLETIME);
  notifyParent(UMSG_EDIT_UPDATE);
}

void EditWnd::onIdleEditUpdate() { }

int EditWnd::onEnter() {
  notifyParent(CHILD_EDIT_DATA_MODIFIED);
  notifyParent(CHILD_EDIT_ENTER_PRESSED);
  if (modal) {
    retcode = EDITWND_RETURN_OK;
    delete this;
    return 1;
  }
  return 0;
}

int EditWnd::onAbort() {
  if (modal) {
    retcode = EDITWND_RETURN_CANCEL;
    notifyParent(CHILD_EDIT_CANCEL_PRESSED);
    delete this;
    return 1;
  }
  return 0;
}

int EditWnd::onLoseFocus() {	// fake an onEnter()
  if (autoenter)
    PostMessage(editWnd, WM_CHAR, VK_RETURN, 0);
  return 0;
}

void EditWnd::setAutoEnter(int a) {
  autoenter = a;
}

int EditWnd::getTextLength() { // TOTALLY NONPORTABLE AND TOTALLY DIRTY

  HFONT font = (HFONT)SendMessage(editWnd, WM_GETFONT, 0, 0);
  
  HDC sdc = GetDC(NULL);
  HDC dc = CreateCompatibleDC(sdc);
  ReleaseDC(NULL, sdc);

  HFONT oldfont = (HFONT)SelectObject(dc, font);
  
  SIZE s;
  GetTextExtentPoint32(dc, outbuf, STRLEN(outbuf), &s);
  SelectObject(dc, oldfont);
  DeleteDC(dc);
  return s.cx + SendMessage(editWnd, EM_GETMARGINS, 0, 0)*2+2;
}

HWND EditWnd::getEditWnd() {
  return editWnd;
}
