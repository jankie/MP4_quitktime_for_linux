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

#ifndef __REGION_H
#define __REGION_H

#include "common.h"
#include "bitmap.h"

class BaseWnd;
class Canvas;

class COMEXP Region {
public:
	Region();
  Region(RECT *r);
  Region(int l, int t, int r, int b);
	Region(HRGN region);
	Region(SkinBitmap *bitmap, int xoffset=0, int yoffset=0, BOOL inverted=FALSE, int dothreshold=0, char threshold=0, int threversed=0);
  Region(SkinBitmap *bitmap, int xoffset, int yoffset, int _x, int _y, int _w, int _h, BOOL inverted=FALSE, int dothreshold=0, char threshold=0, int threversed=0);
  Region(Canvas *c);
	~Region();


  Region *clone();
  void disposeClone(Region *r);
  BOOL ptInRegion(POINT *pt);
  void offset(int x, int y);
  void getBox(RECT *r);
  void subtract(Region *reg);
  void subtract(RECT *r);
  void addRect(RECT *r);
  void add(Region *r);
  void and(Region *r);
  void setRect(RECT *r);
  void empty();
  int isEmpty();
  int equals(Region *r);
  int enclosed(Region *r);
  int enclosed(Region *r, Region *outside);
  int intersects(Region *r);
  int intersects(Region *r, Region *intersection);
  int intersects(RECT *r);
  int intersects(RECT *r, Region *intersection);
  void getRgnBox(RECT *r);
  int isRect();
  void scale(double s, BOOL round=0);
  void scale(double sx, double sy, BOOL round=0);
  void debug();

  // NONPORTABLE

  HRGN makeWindowRegion(); // gives you a handle to a clone of the HRGN object so you can insert it into a window's region with SetWindowRgn. ANY other use is prohibited
  HRGN getHRGN(); // avoid as much as you can, should be used only by WIN32-dependant classes

  // END NONPORTABLE

private:

  HRGN hrgn;
  HRGN alphaToRegionRect(SkinBitmap *bitmap, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted=FALSE, int dothreshold=0, unsigned char threshold=0, int thinverse=0);
  RECT overlay;
};


#endif
