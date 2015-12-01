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
#ifndef _EDITWND_H
#define _EDITWND_H

#include "basewnd.h"
#include "./usermsg.h"
#include "common.h"

#define EDITWND_PARENT BaseWnd
class COMEXP EditWnd : public BaseWnd {
public:
  EditWnd(char *buffer=NULL, int buflen=0);
  virtual ~EditWnd();

  virtual int onInit();
  virtual int onPaint(Canvas *canvas);
  virtual int onResize();
  virtual LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  void setBuffer(char *buffer, int len);
  void getBuffer(char *outbuf, int len);
  void setModal(int modal);//if modal, deletes self on enter
  void setBorder(int border);
  int getTextLength();
  void setAutoEnter(int a);
  
  // the wndproc for the edit box
  LRESULT editWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  HWND getEditWnd();
  virtual int handleRatio() { return 0; }
  virtual int getAutoSelect() { return autoselect; }
  void setAutoSelect(int a) { autoselect = a; };

protected:
  virtual void timerCallback(int id);

  // call down on these if you override them
  virtual void onEditUpdate();
  virtual void onIdleEditUpdate();
  virtual int onEnter();	// user hit enter.. return 1 to close window
  virtual int onAbort();	// user hit escape.. return 1 to close window
  virtual int onLoseFocus();	// different from onKillFocus() from BaseWnd!

private:
  HWND editWnd;
  WNDPROC prevWndProc;
  char *outbuf;
  int maxlen;
  int retcode;
  int idletimelen;
  int modal;
  int bordered;
  int autoenter;
  int beforefirstresize;
  int autoselect;
};

#define EDITWND_RETURN_NOTHING	0	// user didn't do nothing
#define EDITWND_RETURN_OK	1	// user hit return
#define EDITWND_RETURN_CANCEL	2	// user hit escape or something

#endif
