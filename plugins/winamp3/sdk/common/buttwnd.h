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

#ifndef _BUTTWND_H
#define _BUTTWND_H

#include "common.h"
#include "canvas.h"
#include "autobitmap.h"
#include "clickwnd.h"
#include "skinclr.h"

class Region;

#define DEFAULT_BUTTON_TEXT_SIZE 14
typedef enum {
  BUTTONJUSTIFY_LEFT,
  BUTTONJUSTIFY_CENTER
} ButtonJustify;

#define BUTTONWND_PARENT ClickWnd
class COMEXP ButtonWnd : public BUTTONWND_PARENT {
public:
  ButtonWnd();
  virtual ~ButtonWnd();

  virtual int onPaint(Canvas *canvas);

  // methods to modify button looks
  int setBitmaps(const char *normal, const char *pushed=NULL, const char *hilited=NULL, const char *activated=NULL);
  int setBitmaps(HINSTANCE hInst, int normal, int pushed, int hilited, int activated);
  int setRightBitmap(char *bitmap);
  int setBitmapCenter(int centerit);
  void setUseBaseTexture(int useit);
  void setBaseTexture(SkinBitmap *bmp, int x, int y, int tile=0);
  int setButtonText(const char *text, int size=DEFAULT_BUTTON_TEXT_SIZE);
  void setTextJustification(ButtonJustify jus);
  // id will be param1 in child notify msgs
  void setButtonId(int id) { buttonid = id; }
  int getButtonId() const { return buttonid; }
  void setWantFocus(int want) { iwantfocus = !!want; }
  virtual int wantFocus() const { return iwantfocus; }
  void setRectRgn(int r);
  virtual void onLeaveArea();

  int getWidth();	// our preferred width and height (from bitmaps)
  int getHeight();

  virtual int onGetFocus();
  virtual int onKillFocus();
  virtual int onChar(char c);

  virtual void setAlpha(int a);
  virtual int getAlpha(void) const;

  void enableButton(int enabled);	// can be pushed

  virtual void onLeftPush(int x, int y);
  virtual void onRightPush(int x, int y);
  virtual void onLeftDoubleClick(int x, int y);
  virtual void onRightDoubleClick(int x, int y);

  virtual int onResize();

  virtual int onMouseMove(int x, int y);	// need to catch region changes

  virtual int onEnable(int is);
  virtual int getEnabled() const;
  virtual int getPreferences(int what);

  virtual int userDown() { return userdown; }
  virtual int wantClicks() { return getEnabled(); }

  void setCheckBitmap(char *checkbm);
  void setChecked(int c) { checked=c; }; // <0=nocheck, 0=none, >0=checked
  int  getChecked() const { return checked; }
  void setHilite(int h);
  int getHilite();
  void setPushed(int p); // used by menus to simulate pushing
  int getPushed() const; // used by menus to simulate pushing
  void setAutoDim(int ad) { autodim=!!ad; } // nonzero makes it dim if there's no hilite bitmap
  int getAutoDim() const { return autodim; } // nonzero makes it dim if there's no hilite bitmap
  void setActivatedButton(int a);
  int getActivatedButton() const;
  void setBorders(int b) { borders=!!b; }
  int getBorders() const { return borders; }
  void setFolderStyle(int f) { folderstyle = !!f; }
  void setInactiveAlpha(int a) { inactivealpha=a; }
  void setActiveAlpha(int a) { activealpha=a; }

  void setColors(const char *text="studio.button.text", const char *hilite="studio.button.hiliteText", const char *dimmed="studio.button.dimmedText");
  virtual void freeResources();
  virtual void reloadResources();

  virtual int onActivateButton(int active);

  virtual int onActivate();
  virtual int onDeactivate();

protected:
  virtual BOOL mouseInRegion(int x, int y);	// from ClickWnd

private:
  AutoSkinBitmap normalbmp, pushedbmp, hilitebmp, checkbmp, rightbmp, activatedbmp;
  SkinBitmap *base_texture;
  Region *normalrgn, *pushedrgn, *hirgn, *currgn, *activatedrgn;
//CUT  char *label;
  int textsize, buttonid;
  ButtonJustify textjustify;
//  char *color_text,*color_hilite,*color_dimmed;
  SkinColor color_text, color_hilite, color_dimmed;

	String normalBmpStr, pushedBmpStr, hilitedBmpStr, activatedBmpStr;

  int folderstyle:1;
  int autodim:1;
  int userhilite:1;
  int userdown:1;
  int activated:1;
  int enabled:1;
  int borders:1;
  int hilited;
  int iwantfocus:1;
  int center_bitmap:1;
  int use_base_texture:1;

  int checked;
  int xShift, yShift, tile_base_texture;
  int rectrgn;

  int alpha, inactivealpha, activealpha;
};

#endif
