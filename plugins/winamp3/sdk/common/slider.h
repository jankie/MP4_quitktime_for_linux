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
#ifndef _SLIDER_H
#define _SLIDER_H

#include "common.h"
#include "basewnd.h"
#include "canvas.h"
#include "bitmap.h"
#include "virtualwnd.h"

#define DEFAULT_THUMBWIDTH  16
#define DEFAULT_THUMBHEIGHT 16

#define SLIDERWND_PARENT VirtualWnd
class COMEXP SliderWnd : public SLIDERWND_PARENT {
protected:
  SliderWnd();
  virtual ~SliderWnd();

public:
  virtual int onPaint(Canvas *canvas);

  virtual int onLeftButtonDown(int x, int y);
  virtual int onMouseMove(int x, int y);	// only called when mouse captured
  virtual int onLeftButtonUp(int x, int y);

  virtual void onEnterArea();
  virtual void onLeaveArea();

  virtual void setAlpha(int alpha);
  virtual int getAlpha(void);

  virtual void setPosition(int newpos);
  int getSliderPosition();	// ranges from 0 .. 65536

  void setUseBaseTexture(int useit);
  void setBaseTexture(SkinBitmap *bmp, int x, int y);
	void setDrawOnBorders(int draw);

  void setBitmaps(const char *thumbbmp, const char *thumbdownbmp, const char *thumbhighbmp, const char *leftbmp, const char *middlebmp, const char *rightbmp);

  void setLeftBmp(const char *name);
  void setMiddleBmp(const char *name);
  void setRightBmp(const char *name);
  void setThumbBmp(const char *name);
  void setThumbDownBmp(const char *name);
  void setThumbHiliteBmp(const char *name);

  virtual int getHeight();
  virtual int getWidth();

  SkinBitmap *getLeftBitmap();
  SkinBitmap *getRightBitmap();
  SkinBitmap *getMiddleBitmap();
  SkinBitmap *getThumbBitmap();
  SkinBitmap *getThumbDownBitmap();
  SkinBitmap *getThumbHiliteBitmap();

  virtual void setEnable(int en);
  virtual int getEnable(void);

  virtual void setOrientation(int o); // 0=horizontal, 1=vertical
  virtual void setHotPosition(int h); // -1 to disable

protected:

  // override this to get position change notification
  virtual int onSetPosition();		// called constantly as mouse moves
  virtual int onSetFinalPosition();	// called once after move done
  virtual void onBgPaint(Canvas *c, RECT &r);
  virtual void onTextPaint(Canvas *c, RECT &r) {}

  int getSeekStatus();	// returns 1 if user is sliding tab

  int vertical;	// set to 1 for up-n-down instead

  int thumbWidth();
  int thumbHeight();

  virtual void freeResources(); 
  virtual void reloadResources();
 
private:
  int seeking;
  int enabled;
	int hilite;
  int pos;
  int thumbwidth;
  int captured;
  int xShift, yShift;
  SkinBitmap *base_texture;
  int use_base_texture;
	int drawOnBorders;
  int hotPosition;
  int origPos;

  SkinBitmap *left, *middle, *right;
  SkinBitmap *thumb, *thumbdown, *thumbhilite;
  String leftS, middleS, rightS, thumbS, thumbdownS, thumbhiliteS;

  int alpha;
};

#define SLIDER_FULL 65535

#endif
