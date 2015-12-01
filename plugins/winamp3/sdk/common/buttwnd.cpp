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

// bitmap-style buttons

#include "canvas.h"
#include "buttwnd.h"
#include "std.h"
#include "region.h"
#include "notifmsg.h"

#include "../studio/api.h"
#include "../studio/paintsets.h"

#define DEFAULT_BUTTON_HEIGHT 20

ButtonWnd::ButtonWnd() {
  currgn = NULL;
  hirgn = NULL;
  normalrgn = NULL;
  pushedrgn = NULL;
	activatedrgn = NULL;
  base_texture = NULL;
  xShift=0;
  yShift=0;
  textsize = DEFAULT_BUTTON_TEXT_SIZE;
  buttonid = 0;
  textjustify = BUTTONJUSTIFY_CENTER;
  activated = 0;
  folderstyle = 0;
  userhilite = 0;
  userdown = 0;
  use_base_texture = 0;
  center_bitmap = 0;
  enabled = 1;
  checked=0;
  autodim=0;
  borders = 1;
  iwantfocus = 1;
  color_text = "studio.button.text";
  color_hilite = "studio.button.hiliteText";
  color_dimmed = "studio.button.dimmedText";         
  checkbmp = "studio.button.checkmark";  
  alpha = 255;
  inactivealpha = 255;
  activealpha = 255;
  rectrgn=0;
  hilited=0;
}

ButtonWnd::~ButtonWnd() {
  delete normalrgn;
  delete pushedrgn;
  delete hirgn;
  delete activatedrgn;
}

void ButtonWnd::setRectRgn(int r) {
  rectrgn = r;
}

int ButtonWnd::onMouseMove(int x, int y) {
  hilited=0;

  if (getDown() && pushedrgn) 
    currgn = pushedrgn;
  else {
    Region *old = currgn;
    if (hirgn) 
      currgn = hirgn;
    else
      currgn = NULL;
    if (mouseInRegion(x, y))
      hilited=1;
    else
      currgn = normalrgn;
    }

  return BUTTONWND_PARENT::onMouseMove(x, y);
}
void ButtonWnd::onLeaveArea() {
  hilited=0;
  if (hirgn) invalidate();
}

BOOL ButtonWnd::mouseInRegion(int x, int y) {
  POINT pos={x,y};
  POINT p2=pos;

  RECT r;
  getClientRect(&r);
  pos.x-=r.left;
  pos.y-=r.top;
  if (((!currgn || rectrgn) && PtInRect(&r, p2)) || (currgn && currgn->ptInRegion(&pos)))
    return TRUE;
  return FALSE;
}

int ButtonWnd::setBitmaps(const char *_normal, const char *_pushed, const char *_hilited, const char *_activated) {

  if (_normal) { delete normalrgn; normalrgn = NULL; }
  if (_pushed) { delete pushedrgn; pushedrgn = NULL; }
  if (_hilited) { delete hirgn; hirgn = NULL; }
	if (_activated) { delete activatedrgn; activatedrgn = NULL; }

  if (_normal) {
    normalbmp = _normal;
    normalrgn = new Region(normalbmp.getBitmap(), 0, 0);
  }

  if (_pushed) {
    pushedbmp = _pushed;
    pushedrgn = new Region(pushedbmp.getBitmap(), 0, 0);
  }

  if (_hilited) {
    hilitebmp = _hilited;
    hirgn = new Region(hilitebmp.getBitmap(), 0, 0);
  }

  if (_activated) {
    activatedbmp = _activated;
    activatedrgn = new Region(activatedbmp.getBitmap(), 0, 0);
  }


  return 1;
}

void ButtonWnd::freeResources() {
  BUTTONWND_PARENT::freeResources();
  delete normalrgn;
  delete pushedrgn;
  delete hirgn;
  delete activatedrgn;
  pushedrgn=NULL;
  normalrgn=NULL;
  hirgn=NULL;
  activatedrgn=NULL;
  currgn=NULL;
}

void ButtonWnd::reloadResources() {
  BUTTONWND_PARENT::reloadResources();
  if (normalbmp.getBitmap())
    normalrgn = new Region(normalbmp.getBitmap(), 0, 0);
  if (pushedbmp.getBitmap())
    pushedrgn = new Region(pushedbmp.getBitmap(), 0, 0);
  if (hilitebmp.getBitmap())
    hirgn = new Region(hilitebmp.getBitmap(), 0, 0);
  if (activatedbmp.getBitmap())
    activatedrgn = new Region(activatedbmp.getBitmap(), 0, 0);
}

int ButtonWnd::setBitmapCenter(int centerit) {
  return center_bitmap = !!centerit;
}

int ButtonWnd::setBitmaps(HINSTANCE hInst, int _normal, int _pushed, int _hilited, int _activated){
  if (_normal) { delete normalrgn; normalrgn = NULL; }
  if (_pushed) { delete pushedrgn; pushedrgn = NULL; }
  if (_hilited) { delete hirgn; hirgn = NULL; }
	if (_activated) { delete activatedrgn; activatedrgn = NULL; }

  if (_normal) {
    normalbmp.setHInstance(hInst);
    normalbmp = _normal;
    normalrgn = new Region(normalbmp.getBitmap(), 0, 0);
  }

  if (_pushed) {
    pushedbmp.setHInstance(hInst);
    pushedbmp = _pushed;
    pushedrgn = new Region(pushedbmp.getBitmap(), 0, 0);
  }

  if (_hilited) {
    hilitebmp.setHInstance(hInst);
    hilitebmp = _hilited;
    hirgn = new Region(hilitebmp.getBitmap(), 0, 0);
  }

  if (_activated) {
    activatedbmp.setHInstance(hInst);
    activatedbmp = _activated;
    activatedrgn = new Region(activatedbmp.getBitmap(), 0, 0);
  }

  return 1;
}

void ButtonWnd::setUseBaseTexture(int useit) {
  use_base_texture = useit;
  invalidate();
}

void ButtonWnd::setBaseTexture(SkinBitmap *bmp, int x, int y, int tile) {
  base_texture = bmp;
  use_base_texture = TRUE;
  tile_base_texture=tile;
  xShift=x;
  yShift=y;
  invalidate();
}

int ButtonWnd::setButtonText(const char *text, int size) {
  textsize = size;
  ASSERT(textsize > 0);
  setName(text);
  invalidate();
  return 1;
}

void ButtonWnd::setTextJustification(ButtonJustify jus) {
  textjustify = jus;
  invalidate();
}

int ButtonWnd::getWidth() {
  int addl=0;
  if (checked) {
    addl=checkbmp.getWidth()+3;
  }
  if (rightbmp.getBitmap())
    addl+=rightbmp.getWidth()+3;
  if (normalbmp == NULL) {
    BltCanvas blt(10,10);
    blt.pushTextSize(textsize);
    char *lstr=(char*)MALLOC(STRLEN(getName())+4);
    STRCPY(lstr,getName());
    if (STRSTR(lstr,"\t")) STRCAT(lstr,"   ");
    int a=max((blt.getTextWidth(lstr)*11)/10,8)+addl;
    FREE(lstr);
    blt.popTextSize();
    return a;
  }
  return normalbmp.getWidth()+addl;
}

int ButtonWnd::getHeight() {
  int minh=0;
  if (checked>0)
    minh=checkbmp.getHeight();
  if (rightbmp.getBitmap())
    minh=max(rightbmp.getHeight(),minh);
  if (normalbmp == NULL) {
    BltCanvas blt(10,10);
    blt.pushTextSize(textsize);
    int r = max(max((blt.getTextHeight(getName())*11)/10,minh),4);
    blt.popTextSize();
    return r;
  }
  return max(normalbmp.getHeight(),minh);
}

void ButtonWnd::enableButton(int _enabled) {
  _enabled = !!_enabled;
  if (enabled != _enabled) invalidate();
  enabled = _enabled;
  onEnable(enabled);
}

int ButtonWnd::getEnabled() const {
  return enabled;
}

int ButtonWnd::onPaint(Canvas *canvas) {
  PaintBltCanvas paintcanvas;
  SkinBitmap *bmp;
  RECT r;
  int labelxoffs=0;

  if (checked) labelxoffs+=3+checkbmp.getWidth();

  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  BUTTONWND_PARENT::onPaint(canvas);

  bmp = normalbmp;
  if (pushedbmp && (getDown() || userdown)) bmp = pushedbmp;
  else if ((getHilite() || userhilite) && hilitebmp) bmp = hilitebmp;
  else if (activatedbmp && activated) bmp = activatedbmp;

  getClientRect(&r);
  int set = (userdown||getDown()) ? Paintset::BUTTONDOWN : Paintset::BUTTONUP;
  if (!enabled) set=Paintset::BUTTONDISABLED;

  RECT nr = r;
//  RECT fcr = r;
  if (use_base_texture) {
    if (!base_texture)
      renderBaseTexture(canvas, r);
    else {
      RECT cr;
      cr.left = xShift;
      cr.top = yShift;
      cr.right = cr.left + (r.right-r.left);
      cr.bottom = cr.top + (r.bottom-r.top);
      if (tile_base_texture) base_texture->blitTile(canvas, &r,xShift,yShift);
      else base_texture->stretchToRectAlpha(canvas, &cr, &r, alpha);
    }
  } else {
    if (folderstyle) nr.bottom += 3;
    if (borders) api->paintset_render(set, canvas, &nr, alpha);
  }

  if (checked>0) {
    RECT ar;
    int c=(r.top+r.bottom)/2;
    ar.left=r.left;
    ar.top=c-checkbmp.getHeight()/2;
    ar.bottom=ar.top+checkbmp.getHeight();
    ar.right=r.left+checkbmp.getWidth();
    checkbmp.getBitmap()->stretchToRectAlpha(canvas,&ar,alpha);
  }
  if (rightbmp.getBitmap()) {
    RECT ar;
    int c=(r.top+r.bottom)/2;
    ar.top=c-rightbmp.getHeight()/2;
    ar.bottom=ar.top+rightbmp.getHeight();
    ar.right=r.right;
    ar.left=ar.right-rightbmp.getWidth();
    
    rightbmp.getBitmap()->stretchToRectAlpha(canvas,&ar,alpha);
  }
  
  if (bmp != NULL) {
    RECT br = r;
    if (center_bitmap) {
      int w = (r.right - r.left) - getWidth() - labelxoffs;
      int h = (r.bottom - r.top) - getHeight();
      br.top = r.top + h/2;
      br.bottom = br.top + getHeight();
      br.left = r.left + w/2 + labelxoffs;
      br.right = br.left + getWidth() - (rightbmp.getBitmap()?rightbmp.getWidth()+3:0);
    } else {
      br.left += labelxoffs;
      br.right -= (rightbmp.getBitmap()?rightbmp.getWidth()+3:0);
    }
    int alpha2;
    if (!hilitebmp && enabled && autodim) {
      alpha2=128;
      if (getHilite() || userhilite) alpha2=255;
    } else alpha2 = enabled ? 255 : 64;
    bmp->stretchToRectAlpha(canvas, &br,autodim ? (alpha+alpha2)>>1 : alpha);
  }

  if (getName() != NULL) {
    int offset = (enabled&&(userdown||getDown())) ? 1 : 0;
    canvas->setTextOpaque(FALSE);
    //canvas->setTextVariable(TRUE); //FGTEMP!
    canvas->pushTextSize(textsize);
    if (!enabled)
      canvas->setTextColor(color_dimmed);
    else if (userhilite)
      canvas->setTextColor(color_hilite);
    else
      canvas->setTextColor(color_text);
    int h=(r.bottom-r.top-canvas->getTextHeight(getName()))/2;
    if (h<0) h=0;

    r.left += offset + labelxoffs;
    r.right += offset - (rightbmp.getBitmap()?rightbmp.getWidth()+3:0);
    r.top += offset+h;
    r.bottom = r.top+canvas->getTextHeight(getName());

    if (textjustify == BUTTONJUSTIFY_CENTER)
      canvas->textOutCentered(&r, getName());
    else if (textjustify == BUTTONJUSTIFY_LEFT)
    {
      if (!STRSTR(getName(),"\t"))
        canvas->textOutEllipsed(r.left, r.top, r.right-r.left, r.bottom-r.top, getName());
      else
      {
        char *lstr=STRDUP(getName());
        char *p=STRSTR(lstr,"\t");
        if (p) *p++=0;
        else p="";
        int tw=canvas->getTextWidth(p);
        canvas->textOutEllipsed(r.left, r.top, r.right-r.left-tw, r.bottom-r.top, lstr);
        canvas->textOutEllipsed(r.right-tw, r.top, tw, r.bottom-r.top, p);
        FREE(lstr);
      }
    }

    canvas->popTextSize();
  }


/*  if (enabled && gotFocus() && wantFocus()) { // SKIN ME
    fcr.left+=3;
    fcr.right-=3;
    fcr.top+=3;
    fcr.bottom-=3;
    DrawFocusRect(canvas->getHDC(), &fcr);
  }*/

  return 1;
}

int ButtonWnd::onChar(char c) {
  if (c == 13) {
  
  } else return BUTTONWND_PARENT::onChar(c);
  return 1;
}

void ButtonWnd::onLeftPush(int x, int y) {
  notifyParent(CHILD_NOTIFY_LEFTPUSH, buttonid);
}
void ButtonWnd::onRightPush(int x, int y) {
  notifyParent(CHILD_NOTIFY_RIGHTPUSH, buttonid);
}
void ButtonWnd::onLeftDoubleClick(int x, int y) {
  notifyParent(CHILD_NOTIFY_LEFTDOUBLECLICK, buttonid);
}
void ButtonWnd::onRightDoubleClick(int x, int y) {
  notifyParent(CHILD_NOTIFY_RIGHTDOUBLECLICK, buttonid);
}

void ButtonWnd::setHilite(int h) {
  h = !!h;
  if (userhilite != h) {
    userhilite = h;
    invalidate();
  }
}

int ButtonWnd::getHilite() {
  return userhilite || hilited;
}

int ButtonWnd::getPushed() const {
  return userdown;
}

void ButtonWnd::setPushed(int p) {
  p = !!p;
  if (userdown != p)
  {
    userdown=p;
    invalidate();
  }
}

int ButtonWnd::onResize() {
invalidate();
return 1;
}

void ButtonWnd::setCheckBitmap(char *checkbm) {
  checkbmp = checkbm;
}

int ButtonWnd::setRightBitmap(char *bitmap) {
  rightbmp=bitmap;
  return 1;
}

void ButtonWnd::setActivatedButton(int a) {
  if (activated != a) {
    activated = a;
    invalidate();
    onActivateButton(activated);
  }
}

int ButtonWnd::onActivateButton(int active) {
  return 1;
}

int ButtonWnd::getActivatedButton() const {
  return activated;
}

int ButtonWnd::onGetFocus() {
  BUTTONWND_PARENT::onGetFocus();
//  invalidate();
  return 1;
}

int ButtonWnd::onKillFocus() {
  BUTTONWND_PARENT::onKillFocus();
//  invalidate();
  return 1;
}

void ButtonWnd::setColors(const char *text, const char *hilite, const char *dimmed) {
  color_text=text;
  color_hilite=hilite;
  color_dimmed=dimmed;
}

int ButtonWnd::onEnable(int is) {
  return BUTTONWND_PARENT::onEnable(is);
}

void ButtonWnd::setAlpha(int a) {
  if (a != alpha) {
    alpha = a;
    invalidate();
  }
}

int ButtonWnd::getAlpha(void) const {
  return alpha;
}

int ButtonWnd::onActivate() {
  if (activealpha != alpha) {
    alpha = activealpha;
    invalidate();
  }
  return 0;
}

int ButtonWnd::onDeactivate() {
  if (inactivealpha != alpha) {
    alpha = inactivealpha;
    invalidate();
  }
  return 0;
}

int ButtonWnd::getPreferences(int what) {
  switch (what) {
    case SUGGESTED_W: return normalbmp.getWidth();
    case SUGGESTED_H: return normalbmp.getHeight();
  }
  return BUTTONWND_PARENT::getPreferences(what);
}
