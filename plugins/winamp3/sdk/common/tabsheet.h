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

#ifndef _TABSHEET_H
#define _TABSHEET_H

#include "common.h"
#include "basewnd.h"
#include "buttwnd.h"
#include "ptrlist.h"
#include "tlist.h"

class SkinBitmap;

class COMEXP TabButton : public ButtonWnd {
public:
  TabButton();

  virtual void onLeftPush(int x, int y);

  void linkWnd(BaseWnd *control);

private:
  BaseWnd *linked;
};

#define TABSHEET_PARENT BaseWnd
class COMEXP TabSheet : public TABSHEET_PARENT {
public:
  TabSheet();
  virtual ~TabSheet();

  virtual int onInit();
  virtual void getClientRect(RECT *);
  virtual int onPaint(Canvas *canvas);
  virtual int onResize();
  virtual int childNotify(RootWnd *which, int msg, int param1, int param2);

  void setTabRowHeight(int newheight);
  void setTabRowMargin(int pixels);
  int addChild(BaseWnd *newchild);
  int addChild(BaseWnd *newchild, const char *tip);
  int addChild(BaseWnd *newchild, HINSTANCE hInst, int normal, int pushed, int hilite);
  int addChild(BaseWnd *newchild, HINSTANCE hInst, int normal, int pushed, int hilite, const char *tip);
  int setButtonVisible(int r, BOOL state);
  virtual void activateChild(BaseWnd *activechild);

  BaseWnd *enumChild(int child);
  int getNumChild();
  TabButton *enumButton(int button);
  int getNumButton();

  void setBackgroundBmp(const char *name); //FG
  SkinBitmap *getBackgroundBitmap(); //FG

protected:
  // you can set these in your constructor, they will be deleted for you
  ButtonWnd *leftscroll, *rightscroll;
  SkinBitmap *background;

private:
  int tabrowheight, tabrowwidth, tabrowmargin;
  PtrList<BaseWnd> children;
  PtrList<TabButton> tabs;
  TList<HINSTANCE> instances;
  TList<int> normal;
  TList<int> pushed;
  TList<int> hilite;
  TList<BOOL> showed;
  PtrList<char> tips;

  int tilex, tilew;

  BaseWnd *active;
};

#endif
