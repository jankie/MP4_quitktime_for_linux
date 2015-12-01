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

#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#include "virtualwnd.h"
#include "region.h"
#include "usermsg.h"
#include "autobitmap.h"

#define SCROLLBAR_PARENT VirtualWnd

#define SCROLLBAR_FULL 65535

#define POS_NONE    0
#define POS_LEFT    1
#define POS_BUTTON  2
#define POS_RIGHT   3

#define PAGE_NONE   0
#define PAGE_DOWN   1
#define PAGE_UP     2

#define DEFAULT_HEIGHT 16

class COMEXP ScrollBar : public SCROLLBAR_PARENT {
public:
  ScrollBar();
  virtual ~ScrollBar();

  virtual int onMouseMove (int x, int y);
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onRightButtonDown(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  virtual int onMouseWheelUp(int clicked, int lines);
  virtual int onMouseWheelDown(int clicked, int lines);
  virtual int onPaint(Canvas *canvas);
  virtual int onResize();
  virtual int onInit();
  virtual void timerCallback(int id);

  virtual int wantDoubleClicks() { return 0; };

  virtual int onSetPosition(BOOL smooth=FALSE);
  virtual int onSetFinalPosition();

  void setBitmaps(char *left, char *lpressed, char *lhilite,
                  char *right, char *rpressed, char *rhilite, 
                  char *button, char *bpressed, char *bhilite);
  void setPosition(int pos);
  int getPosition();
  int getHeight();
  void setHeight(int newheight);
  void setNPages(int n);
  void gotoPage(int n);
  void setUpDownValue(int newupdown);
  void setVertical(BOOL isvertical);
  int getWidth();

  virtual void freeResources();
  virtual void reloadResources();

private:
  void deleteResources();
  int getMousePosition();
  void calcOverlapping();
  void calcXPosition();
  void calcPosition();
  void handlePageUpDown();
  int checkPageUpDown();
  void handleUpDown();
  int checkUpDown();
  int pageUp();
  int pageDown();
  int upDown(int which);
  void setPrivatePosition(int pos, BOOL signal=TRUE, BOOL smooth=FALSE);
  void loadBmps();

  AutoSkinBitmap bmpleft, bmplpressed, bmplhilite,
             bmpright, bmprpressed, bmprhilite,
             bmpbutton, bmpbpressed, bmpbhilite;

  Region *leftrgn, *rightrgn, *buttonrgn;
  int position;

  int moving;
  int lefting;
  int righting;
  int clicked;

  int buttonx;

  int curmouseposition;
  int clickmouseposition;
  int height;

  int shiftleft, shiftright;
  POINT clickpos;
  int clickbuttonx;
  int pageing;
  int firstdelay;
  int timer;
  int npages;
  int pageway;
  int updown;
  int timer2;
  int insetpos;

  int vertical;
  int lastx, lasty;
};


#endif
