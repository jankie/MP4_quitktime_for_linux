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

#include <windows.h>
#include <ddraw.h>


#include "blending.h"
#include "canvas.h"
#include "bitmap.h"
#include "region.h"
#include "basewnd.h"
#include "fontdef.h"

#include "../studio/assert.h"
#include "../studio/api.h"

#define CBCLASS Canvas
START_DISPATCH;
  CB(GETHDC, getHDC);
  CB(GETROOTWND, getRootWnd);
  CB(GETBITS, getBits);
  VCB(GETOFFSETS, getOffsets);
  CB(ISFIXEDCOORDS, isFixedCoords);
  CB(GETDIM, getDim);
  CB(GETTEXTFONT, getTextFont);
  CB(GETTEXTSIZE, getTextSize);
  CB(GETTEXTBOLD, getTextBold);
  CB(GETTEXTOPAQUE, getTextOpaque);
  CB(GETTEXTALIGN, getTextAlign);
  CB(GETTEXTCOLOR, getTextColor);
END_DISPATCH;
#undef CBCLASS

//NONPORTABLE

Canvas::Canvas() {
  hdc = NULL;
  bits = NULL;
  srcwnd = NULL;
  fcoord = FALSE;
  xoffset = yoffset = 0;

  align = DT_LEFT;
  tsize = 12;
  tbold = 0;
  topaque = 0;
  width=height=pitch=0;
  tcolor = RGB(0,0,0);
  tfont = new String; // using dynamic tfont here coz we need to manage em with stack, so stacking fonts won't take sizeof(String) and their destruction will not fuxor everything
  tfont->setValue("Arial");
//  tvariable = 0;
}

Canvas::~Canvas() {
  ASSERT(hdc == NULL);
  delete tfont;
  if (!fontstack.isempty()) {
    OutputDebugString("Font stack not empty in Canvas::~Canvas !");
    while (!fontstack.isempty()) {
      String *s;
      fontstack.pop(&s);
      delete s;
    }
  }
  if (!boldstack.isempty() || !opstack.isempty() || !alstack.isempty() || !colorstack.isempty() || !sizestack.isempty())
    OutputDebugString("Font property stack not empty in Canvas::~Canvas !");
}

void Canvas::setBaseWnd(BaseWnd *b) {
  srcwnd=b;
}

HDC Canvas::getHDC() {
  return hdc;
}

RootWnd *Canvas::getRootWnd() {
  return srcwnd;
}

void *Canvas::getBits() {
  return bits;
}

BOOL Canvas::getDim(int *w, int *h, int *p) {
  if (w) *w=width;
  if (h) *h=height;
  if (p) *p=pitch;
  return FALSE;
}

void Canvas::getOffsets(int *x, int *y) {
  if (x != NULL) *x = getXOffset();
  if (y != NULL) *y = getYOffset();
}

BOOL Canvas::isFixedCoords() {
  return fcoord;
}

BaseWnd *Canvas::getBaseWnd() {
  return srcwnd;
}

void Canvas::fillRect(RECT *r, COLORREF color) {
  ASSERT(r != NULL);
  HBRUSH brush;
  if (color == RGB(0,0,0)) {
    FillRect(hdc, r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    return;
  }
  RECT rr = *r;
  offsetRect(&rr);

  brush = CreateSolidBrush(color);
  FillRect(hdc, &rr, brush);
  DeleteObject(brush);
}

void Canvas::drawRect(RECT *r, int solid, COLORREF color) {
  HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
  HPEN oldpen, pen;
  pen = CreatePen(solid ? PS_SOLID : PS_DOT, 0, color);
  oldpen = (HPEN)SelectObject(hdc, pen);
  ASSERT(r != NULL);
  RECT rr = *r;
  offsetRect(&rr);
  Rectangle(hdc, rr.left, rr.top, rr.right, rr.bottom);
  SelectObject(hdc, oldpen);
  SelectObject(hdc, oldbrush);
  DeleteObject(pen);
}

void Canvas::setTextAlign(int al) {
	switch(al) {
		case ALIGN_LEFT: align=DT_LEFT; break;
		case ALIGN_CENTER: align=DT_CENTER; break;
		case ALIGN_RIGHT: align=DT_RIGHT; break;
	}
}

int Canvas::getTextAlign() {
  return align;
}

void Canvas::pushTextAlign(int al) {
  alstack.push(align);
  align = al;
}

int Canvas::popTextAlign() {
  if (alstack.isempty()) return 0;
  int old = align;
  alstack.pop(&align);
  return old;
}

void Canvas::setTextOpaque(int opaque) {
  topaque = opaque;
}

int Canvas::getTextOpaque() {
  return topaque;
}

void Canvas::pushTextOpaque(int op) {
  opstack.push(topaque);
  topaque = op;
}

int Canvas::popTextOpaque() {
  if (opstack.isempty()) return 0;
  int old = topaque;
  opstack.pop(&topaque);
  return old;
}

void Canvas::setTextBold(int b) {
  tbold = b;
}

int Canvas::getTextBold() {
  return tbold;
}

void Canvas::pushTextBold(int b) {
  boldstack.push(tbold);
  tbold = b;
}

int Canvas::popTextBold() {
  if (boldstack.isempty()) return 0;
  int old = tbold;
  boldstack.pop(&tbold);
  return old;
}

/*void Canvas::setTextVariable(int v) {
  tvariable = v;
}

int Canvas::getTextVariable() {
  return tvariable;  
}

void Canvas::pushTextVariable(int v) {
  varstack.push(tvariable);
  tvariable = v;
}

int Canvas::popTextVariable() {
  int old = tvariable;
  varstack.pop(&tvariable);
  return old;
} */

void Canvas::setTextColor(COLORREF color) {
  tcolor = color;
}

COLORREF Canvas::getTextColor() {
  return tcolor;
}

void Canvas::pushTextColor(COLORREF color) {
  colorstack.push(tcolor);
  tcolor = color;
}

COLORREF Canvas::popTextColor() {
  if (colorstack.isempty()) return RGB(0,0,0);
  COLORREF oldcolor = tcolor;
  colorstack.pop(&tcolor);
  return oldcolor;
}

void Canvas::setTextSize(int points) {
  tsize = points;
}

int Canvas::getTextSize() {
  return tsize;
}

void Canvas::pushTextSize(int point) {
  sizestack.push(tsize);
  tsize = point;
}

int Canvas::popTextSize() {
  if (sizestack.isempty()) return 0;
  int oldsize = tsize;
  sizestack.pop(&tsize);
  return oldsize;
}

void Canvas::setTextFont(const char *font_id_name) {
  tfont->setValue(font_id_name);
}

const char *Canvas::getTextFont() {
  return tfont->getValue();
}

void Canvas::pushTextFont(const char *font_id_name) {
  fontstack.push(tfont);
  tfont = new String;
  tfont->setValue(font_id_name);
}

const char *Canvas::popTextFont() {
  if (fontstack.isempty()) return NULL;
  String *old = tfont;
  fontstack.pop(&tfont);
  Canvas::retfontname[0] = 0;
  if (!old->isempty())
    STRCPY(Canvas::retfontname, old->getValue());
  delete old;
  return Canvas::retfontname;
}

void Canvas::moveTo(int x, int y) {
MoveToEx(hdc, x, y, NULL);
}

void Canvas::lineTo(int x, int y) {
LineTo(hdc, x, y);
}

void Canvas::textOut(int x, int y, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_NORMAL, x, y, 0, 0, txt);
}

void Canvas::textOut(int x, int y, int w, int h, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_RECT, x, y, w, h, txt);
}

void Canvas::textOutEllipsed(int x, int y, int w, int h, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_ELLIPSED, x, y, w, h, txt);
}

void Canvas::textOutWrapped(int x, int y, int w, int h, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_WRAPPED, x, y, w, h, txt);
}

void Canvas::textOutWrappedPathed(int x, int y, int w, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_WRAPPEDPATHED, x, y, w, 0, txt);
}

void Canvas::textOutCentered(RECT *r, const char *txt) {
  api->font_textOut(this, WA_FONT_TEXTOUT_CENTERED, r->left, r->top, r->right, r->bottom, txt);
}

int Canvas::getTextWidth(const char *text) {
  return api->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_WIDTH, text, NULL, NULL);
}

int Canvas::getTextHeight(const char *text) {
  return api->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_HEIGHT, text, NULL, NULL);
}

void Canvas::getTextExtent(const char *txt, int *w, int *h) {
  api->font_getInfo(this, tfont->getValue(), WA_FONT_GETINFO_WIDTHHEIGHT, txt, w, h);
}

void Canvas::offsetRect(RECT *r) {
  ASSERT(r != NULL);
  r->left += xoffset;
  r->right += xoffset;
  r->top += yoffset;
  r->bottom += yoffset;
}

void Canvas::selectClipRgn(Region *r) {
	SelectClipRgn(hdc, r ? r->getHRGN() : NULL);
}

int Canvas::getClipBox(RECT *r) {
  return GetClipBox(hdc, r);
}

int Canvas::getClipRgn(Region *r) {
  ASSERT(r != NULL);
  return GetClipRgn(hdc, r->getHRGN());
}

//FG> added blit canvas to canvas
void Canvas::blit(int srcx, int srcy, Canvas *dest, int dstx, int dsty, int dstw, int dsth) { 
  char *srcbits=(char *)getBits();
  char *destbits=(char *)dest->getBits();
  RECT clipr;
  if (srcbits && destbits && GetClipBox(dest->getHDC(),&clipr) == SIMPLEREGION)
  {
    int srcimg_w,srcimg_h,srcimg_p;
    getDim(&srcimg_w,&srcimg_h,&srcimg_p);
    int dstimg_w,dstimg_h,dstimg_p;
    dest->getDim(&dstimg_w,&dstimg_h,&dstimg_p);
    
    if (srcx < 0) { dstx-=srcx; dstw+=srcx; srcx=0; }
    if (srcy < 0) { dsty-=srcy; dsth+=srcy; srcy=0; }
    if (srcx+dstw >= srcimg_w) dstw=srcimg_w-srcx;
    if (srcy+dsth >= srcimg_h) dsth=srcimg_h-srcy;

    if (dstx < clipr.left) { srcx+=clipr.left-dstx; dstw-=clipr.left-dstx; dstx=clipr.left; }
    if (dsty < clipr.top) { srcy+=clipr.top-dsty; dsth-=clipr.top-dsty; dsty=clipr.top; }

    if (dstx+dstw >= clipr.right) dstw=clipr.right-dstx;
    if (dsty+dsth >= clipr.bottom) dsth=clipr.bottom-dsty;

    if (!dstw || !dsth) return;

    int y;
    int yl=dsty+dsth;
    for (y = dsty; y < yl; y++)
    {
      MEMCPY(destbits+y*dstimg_p+dstx*4,srcbits+srcy*srcimg_p+srcx*4,dstw*4);
      srcy++;
    }
  }
  else
  {
    GdiFlush();
    BitBlt(dest->getHDC(), dstx, dsty, dstw, dsth, getHDC(), srcx, srcy, SRCCOPY);
  }
}

// src* are in fixed point
void Canvas::stretchblit(int srcx, int srcy, int srcw, int srch, Canvas *dest, int dstx, int dsty, int dstw, int dsth) { 
  GdiFlush();
  int done=0;
  void *srcdib=getBits();

  if (!dstw || !dsth || !srcw || !srch) return;
  if (srcdib)
  {
    int srcdib_w, srcdib_h, srcdib_p;
    getDim(&srcdib_w,&srcdib_h,&srcdib_p);
    int nofilter=0;//FUCKO :) _intVal(api->config_enumRootCfgItem(0), "NoWindowStretchFilter");

    void *dstdib;
    BITMAPINFO dstbmi={0,};
    HDC hMemDC;
    HBITMAP hsrcdib;
    dstbmi.bmiHeader.biSize=sizeof(dstbmi.bmiHeader);
    dstbmi.bmiHeader.biWidth=dstw;
    dstbmi.bmiHeader.biHeight=-ABS(dsth);
    dstbmi.bmiHeader.biPlanes=1;
    dstbmi.bmiHeader.biBitCount=32;
    dstbmi.bmiHeader.biCompression=BI_RGB;
    hMemDC = CreateCompatibleDC(NULL);
    hsrcdib=CreateDIBSection(hMemDC,&dstbmi,DIB_RGB_COLORS,&dstdib,NULL,0);
    if (hsrcdib)
    {
      HBITMAP hprev=(HBITMAP)SelectObject(hMemDC,hsrcdib);

      int y;
      int SY, dX, dY;
      int Xend=(srcdib_w-2)<<16;
      SY=srcy;
      dX=srcw/dstw;
      dY=srch/dsth;

      int xstart=0;
      int xp=srcx>>16;
      if (xp < 0) 
      {
        xstart=-xp;
        srcx+=xstart*dX;
      }

      int xend=dstw;
      xp=(srcx+(dX*(xend-xstart)))>>16;
      if (xp > srcdib_w)
      {
        xend=xstart+srcdib_w - (srcx>>16);
      }
    
      for (y = 0; y < dsth; y ++)
      {
        int yp=(SY>>16);
        if (yp >= 0)
        {
          int x;
          int SX=srcx;
          unsigned int *out=(unsigned int*)dstdib + xstart + y*dstw;
          int end=yp >= srcdib_h-1;
          if (nofilter || end) 
          {
            if (end) yp=srcdib_h-1;
            unsigned int *in=(unsigned int*) ((char *)srcdib + yp*srcdib_p);
            for (x = xstart; x < xend; x ++) // quick hack to draw last line
            {
              *out++=in[SX>>16];
              SX+=dX;
            }
            if (end) break;
          }
          else
          {
            unsigned int *in=(unsigned int*) ((char *)srcdib + yp*srcdib_p);

  #ifndef NO_MMX
            if (Blenders::MMX_AVAILABLE())
            {
              for (x = xstart; x < xend; x ++)
              {
                if (SX>Xend) *out++=Blenders::BLEND4_MMX(in+(Xend>>16),srcdib_w,0xffff,SY);
                else *out++=Blenders::BLEND4_MMX(in+(SX>>16),srcdib_w,SX,SY);
                SX+=dX;
              }
            }
            else
  #endif
            {
              for (x = xstart; x < xend; x ++)
              {
                if (SX>Xend) *out++=Blenders::BLEND4(in+(Xend>>16),srcdib_w,0xffff,SY);
                else *out++=Blenders::BLEND4(in+(SX>>16),srcdib_w,SX,SY);
                SX+=dX;
              }
            }
          }
        }
        SY+=dY;
      }


#ifndef NO_MMX
      Blenders::BLEND_MMX_END();
#endif


      BitBlt(dest->getHDC(),dstx,dsty,dstw,dsth,hMemDC,0,0,SRCCOPY);
      done++;

      SelectObject(hMemDC,hprev);
      DeleteObject(hsrcdib);
    }
    DeleteDC(hMemDC);
  }

  if (!done)
  {
    SetStretchBltMode(dest->getHDC(),COLORONCOLOR);
    StretchBlt(dest->getHDC(), dstx, dsty, dstw, dsth, getHDC(), srcx>>16, srcy>>16, srcw>>16, srch>>16, SRCCOPY);
  }
}

void Canvas::debug() {
#ifdef DEBUG_SCREEN_SHIFT
  Canvas *c = new SysCanvas();
  int w, h;
  getDim(&w, &h, NULL);
  blit(0, 0, c, DEBUG_SCREEN_SHIFT, 0, w, h);
  delete c;
#endif
}


#define BF2 (~((3<<24)|(3<<16)|(3<<8)|3))

void Canvas::antiAliasTo(Canvas *dest, int w, int h, int aafactor) {
  ASSERT(aafactor != 0);
  if (aafactor == 1) {
    blit(0, 0, dest, 0, 0, w, h);
    return;
  }
  ASSERT(getBits() != NULL);
  ASSERT(dest->getBits() != NULL);
  if (getBits() == NULL || dest->getBits() == NULL) return;
  ASSERTPR(aafactor <= 2, "too lazy to generalize the code right now :)");
  GdiFlush();
  // we should really store the bpp too
  int aaw = w * aafactor;
  unsigned long *s1 = (unsigned long *)getBits(), *s2 = s1 + 1;
  unsigned long *s3 = s1 + aaw, *s4 = s3 + 1;
  unsigned long *d = (unsigned long *)dest->getBits();
#if 1
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      unsigned long tmp = ((*s1 & BF2)>>2) + ((*s2 & BF2)>>2) + ((*s3 & BF2)>>2) + ((*s4 & BF2)>>2); 
      *d++ = tmp;

      s1 += 2; s2 += 2;
      s3 += 2; s4 += 2;
    }
    s1 += aaw; s2 += aaw;
    s3 += aaw; s4 += aaw;
  }
#else
for (int x = 0; x < w * h; x++) d[x] = s1[x];
#endif
}

char Canvas::retfontname[256];

WndCanvas::WndCanvas() {
  hWnd = NULL;
}

WndCanvas::~WndCanvas() {
  ASSERT((hWnd != NULL && hdc != NULL) || (hWnd == NULL && hdc == NULL));
  if (hWnd != NULL) ReleaseDC(hWnd, hdc);
  hdc = NULL;
}

int WndCanvas::attachToClient(BaseWnd *basewnd) {
  ASSERT(basewnd != NULL);
  hWnd = basewnd->gethWnd();
  ASSERT(hWnd != NULL);
  hdc = GetDC(hWnd);
  ASSERT(hdc != NULL);
  srcwnd = basewnd;
  return 1;
}

#if 0//CUT
int WndCanvas::attachToWnd(HWND _hWnd) {
  hWnd = _hWnd;
  ASSERT(hWnd != NULL);
  hdc = GetWindowDC(hWnd);
  ASSERT(hdc != NULL);
  return 1;
}
#endif

PaintCanvas::PaintCanvas() {
  hWnd = NULL;
}

PaintCanvas::~PaintCanvas() {
  
  if (hdc != NULL) EndPaint(hWnd, &ps);
  hdc = NULL;
}

void PaintCanvas::getRcPaint(RECT *r) {
  *r = ps.rcPaint; 
}

int PaintCanvas::beginPaint(BaseWnd *basewnd) {
  hWnd = basewnd->gethWnd();	// NONPORTABLE
  ASSERT(hWnd != NULL);
  hdc = BeginPaint(hWnd, &ps);
  ASSERT(hdc != NULL);
  srcwnd = basewnd;
  return 1;
}

PaintBltCanvas::PaintBltCanvas() {
  hWnd = NULL;
  wnddc = NULL;
  hbmp = NULL;
  prevbmp = NULL;
  bits = NULL;
  fcoord = TRUE;
  nonclient = FALSE;
}

PaintBltCanvas::~PaintBltCanvas() {
  RECT r;

  if (hdc == NULL) return;

  ASSERT(srcwnd != NULL);
	if (nonclient) //FG> nonclient painting fix
		srcwnd->getNonClientRect(&r);
	else
		srcwnd->getClientRect(&r); 

  // blt here
  GdiFlush();
  BitBlt(wnddc, r.left, r.top, r.right-r.left, r.bottom-r.top, hdc, 0, 0, SRCCOPY);

  //SelectClipRgn(hdc, NULL);
  // kill the bitmap and its DC
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  DeleteObject(hbmp);
  bits=NULL;
  width=0;
  height=0;
  pitch=0;

  EndPaint(hWnd, &ps);	// end of wnddc
  wnddc = NULL;
}

//FG> nonclient painting fix
int PaintBltCanvas::beginPaintNC(BaseWnd *basewnd) {
  nonclient = TRUE;
  return beginPaint(basewnd);
}

void PaintBltCanvas::getRcPaint(RECT *r) {
  *r = ps.rcPaint; 
}

int PaintBltCanvas::beginPaint(BaseWnd *basewnd) {

  RECT r;
  
  if (nonclient)
    basewnd->getNonClientRect(&r); //FG> nonclient painting fix
  else
    basewnd->getClientRect(&r);

  if (r.right - r.left <= 0 || r.bottom - r.top <= 0) return 0;

  hWnd = basewnd->gethWnd();	// NONPORTABLE
  ASSERT(hWnd != NULL);

  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof bmi);
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = r.right - r.left;
  bmi.bmiHeader.biHeight = -(r.bottom - r.top);
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;

  wnddc = BeginPaint(hWnd, &ps);

  ASSERT(wnddc != NULL);

  GdiFlush();
  width=r.right-r.left;
  height=-ABS(r.bottom-r.top);
  pitch=width*4;
  hbmp = CreateDIBSection(wnddc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);  

  if (hbmp == NULL) {
    EndPaint(hWnd, &ps);	// end of wnddc
    wnddc = NULL;
    return 0;
  }

  // create tha DC
  hdc = CreateCompatibleDC(wnddc);
  if (hdc == NULL) {
    DeleteObject(hbmp);
    EndPaint(hWnd, &ps);	// end of wnddc
    wnddc = NULL;
    return 0;
  }
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);

  Region *clip = new Region(&ps.rcPaint);
  
  selectClipRgn(clip);

  delete clip;

  srcwnd = basewnd;

  return 1;
}

void *PaintBltCanvas::getBits() {
  return bits;
}

MemCanvas::MemCanvas() {
}

MemCanvas::~MemCanvas() {
  DeleteDC(hdc);
  hdc = NULL;
}

int MemCanvas::createCompatible(Canvas *canvas) {
  ASSERT(canvas != NULL);
  ASSERT(canvas->getHDC() != NULL);
  hdc = CreateCompatibleDC(canvas->getHDC());
  ASSERT(hdc != NULL);
  srcwnd = canvas->getBaseWnd();
  return 1;
}

DCCanvas::DCCanvas(HDC clone, BaseWnd *srcWnd) {
  if (clone != NULL) cloneDC(clone, srcWnd);
}

DCCanvas::~DCCanvas() {
  hdc = NULL;
}

int DCCanvas::cloneDC(HDC clone, BaseWnd *srcWnd) {

  ASSERT(clone != NULL);
  hdc = clone;
  srcwnd = srcWnd;
  return 1;
}

SysCanvas::SysCanvas() {
  hdc = GetDC(NULL);
}

SysCanvas::~SysCanvas() {
  ReleaseDC(NULL, hdc);
  hdc = NULL;
}

DCBltCanvas::DCBltCanvas() {
  origdc = NULL;
  hbmp = prevbmp = NULL;
}

DCBltCanvas::~DCBltCanvas() {

	commitDC();

  // kill the bitmap and its DC
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  DeleteObject(hbmp);

  // don't kill origdc, it's been cloned
}

int DCBltCanvas::setOrigDC(HDC neworigdc) { // FG> allows custom draw on lists to be much faster
	origdc = neworigdc;
	return 1;
}

int DCBltCanvas::commitDC(void) { //FG

	if (origdc) {

	  RECT c;

	  if (GetClipBox(origdc, &c) == NULLREGION)
			c = rect;
		
	  // shlap it down in its original spot
    GdiFlush();
	  BitBlt(origdc, c.left, c.top,
	         c.right-c.left, c.bottom-c.top, hdc, c.left, c.top, SRCCOPY);

	}

	return 1;
}

int DCBltCanvas::cloneDC(HDC clone, RECT *r, BaseWnd *srcWnd) {
  origdc = clone;

  srcwnd = srcWnd;

  ASSERT(r != NULL);
  rect = *r;

#if 1
  BITMAPINFO bmi;
  ZeroMemory(&bmi, sizeof bmi);
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = r->right - r->left;
  bmi.bmiHeader.biHeight = -ABS(r->bottom - r->top);
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
  hbmp = CreateDIBSection(origdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  width=bmi.bmiHeader.biWidth;
  height=ABS(bmi.bmiHeader.biHeight);
  pitch=width*4;
#else
  hbmp = CreateCompatibleBitmap(clone, r->right - r->left, r->bottom - r->top);
#endif
  ASSERT(hbmp != NULL);

  // create tha DC
  hdc = CreateCompatibleDC(origdc);
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);

  // adjust their rect for them
  r->right -= r->left;
  r->left = 0;
  r->bottom -= r->top;
  r->top = 0;

  return 1;
}


BaseCloneCanvas::BaseCloneCanvas(CanvasBase *cloner) {
  if (cloner != NULL) clone(cloner);
}

int BaseCloneCanvas::clone(CanvasBase *cloner) {
  ASSERTPR(hdc == NULL, "can't clone twice");
  hdc = cloner->getHDC();
  bits = cloner->getBits();
  cloner->getDim(&width,&height,&pitch);
//  srcwnd = cloner->getBaseWnd();
  cloner->getOffsets(&xoffset, &yoffset);
  setTextFont(cloner->getTextFont());
  setTextSize(cloner->getTextSize());
  setTextBold(cloner->getTextBold());
  setTextOpaque(cloner->getTextOpaque());
  setTextAlign(cloner->getTextAlign());
  setTextColor(cloner->getTextColor());
  return (hdc != NULL);
}

BaseCloneCanvas::~BaseCloneCanvas() {
  hdc = NULL;
}



BltCanvas::~BltCanvas() {
  if (hdc == NULL) return;

  // kill the bitmap and its DC
  SelectObject(hdc, prevbmp);
  DeleteDC(hdc);
  hdc = NULL;
  if (ourbmp) {
    GdiFlush();
    DeleteObject(hbmp);
  }
  if (skinbmps) {
    if (skinbmps->getNumItems() > 0) {
      OutputDebugString("disposeSkinBitmap not called for makeSkinBitmap, deleting the clone\n");
      skinbmps->deleteAll();
    }
    delete skinbmps;
  }
  if (envelope)
    delete envelope;
}

BltCanvas::BltCanvas(HBITMAP bmp) {
  prevbmp = NULL;
  bits = NULL;
  fcoord = TRUE;
  ourbmp = FALSE;
  skinbmps=NULL;
  envelope = NULL;
  
  hbmp = bmp;
  ASSERT(hbmp != NULL);

  // create tha DC
  hdc = CreateCompatibleDC(NULL);
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);
}

BltCanvas::BltCanvas(int w, int h, int nb_bpp, unsigned char *pal, int palsize){

  hbmp = NULL;
  prevbmp = NULL;
  bits = NULL;
  fcoord = TRUE;
  ourbmp = TRUE;
  bpp = nb_bpp;
  skinbmps=NULL;
  envelope = NULL;

  ASSERT(w != 0 && h != 0);
  if (w==0) w=1;
  if (h==0) h=1;

	static struct
	{
		BITMAPINFO bmi;
		RGBQUAD more_bm7iColors[256];
	} bitmap;
  
//  BITMAPINFO bmi;
  ZeroMemory(&bitmap, sizeof bitmap);
	if(pal)
	{
		for (int c = 0; c < palsize/(3); c ++) {
			bitmap.bmi.bmiColors[c].rgbRed = pal[c*3];
			bitmap.bmi.bmiColors[c].rgbGreen = pal[c*3+1];
			bitmap.bmi.bmiColors[c].rgbBlue = pal[c*3+2];
			bitmap.bmi.bmiColors[c].rgbReserved = 0;
		}
		bitmap.bmi.bmiHeader.biClrUsed = palsize/(3);
		bitmap.bmi.bmiHeader.biClrImportant = palsize/(3);
	}
	else
	{
	  bitmap.bmi.bmiHeader.biClrUsed = 0;
		bitmap.bmi.bmiHeader.biClrImportant = 0;
	}
  bitmap.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmap.bmi.bmiHeader.biWidth = ABS(w);
  bitmap.bmi.bmiHeader.biHeight = -ABS(h);
  bitmap.bmi.bmiHeader.biPlanes = 1;
  bitmap.bmi.bmiHeader.biBitCount = nb_bpp;
  bitmap.bmi.bmiHeader.biCompression = BI_RGB;
  bitmap.bmi.bmiHeader.biSizeImage = 0;
  bitmap.bmi.bmiHeader.biXPelsPerMeter = 0;
  bitmap.bmi.bmiHeader.biYPelsPerMeter = 0;
  GdiFlush();
  hbmp = CreateDIBSection(NULL, &bitmap.bmi, DIB_RGB_COLORS, &bits, NULL, 0);
  if (hbmp == NULL) {
    return;
  }
  GetObject(hbmp, sizeof(BITMAP), &bm);
  width=bm.bmWidth;
  height=ABS(bm.bmHeight);
  pitch=bm.bmWidthBytes;

  // create tha DC
  hdc = CreateCompatibleDC(NULL);
  if (!hdc) __asm int 3;
  prevbmp = (HBITMAP)SelectObject(hdc, hbmp);
}

void *BltCanvas::getBits() {
  return bits;
}

HBITMAP BltCanvas::getBitmap() {
  return hbmp;
}

SkinBitmap *BltCanvas::getSkinBitmap() { 
	// make a SkinBitmap envelope
  if (!envelope)
	  envelope = new SkinBitmap(getBitmap(), getHDC(), 1, getBits());

  // do not delete enveloppe, it's deleted in destructor
  return envelope;
}

SkinBitmap *BltCanvas::makeSkinBitmap() { 
	// make a clone of the bitmap - JF> what was that crap about envelopes?
	SkinBitmap *clone = new SkinBitmap(getBitmap(), getHDC(), 1);

	if (!skinbmps)
	  skinbmps = new PtrList<SkinBitmap>;
	skinbmps->addItem(clone);

	return clone;
}

void BltCanvas::disposeSkinBitmap(SkinBitmap *b) {
  if (skinbmps->haveItem(b)) {
    skinbmps->removeItem(b);
    delete b;
  } else {
    OutputDebugString("disposeSkinBitmap called on unknownj pointer, you should call it from the object used to makeSkinBitmap()\n");
  }
}

void BltCanvas::fillBits(COLORREF color) {
  // TODO: use rep movsl
  if (bpp == 32) {	// clear out the bits
    int nwords = bm.bmWidth * bm.bmHeight;
    DWORD *dwbits = (DWORD *)bits;
    for (int i = 0; i < nwords; i++) *dwbits++ = color;
  }
}

void BltCanvas::vflip() {
  ASSERT(bits != NULL);
//  BITMAP bm;
//  int r = GetObject(hbmp, sizeof(BITMAP), &bm);
//  if (r == 0) return;
  int w = bm.bmWidth, h = bm.bmHeight;
  int bytes = 4 * w;
  char *tmpbuf = (char *)MALLOC(bytes);
  for (int i = 0; i < h/2; i++) {
    char *p1, *p2;
    p1 = (char *)bits + bytes * i;
    p2 = (char *)bits + bytes * ((h - 1) - i);
    if (p1 == p2) continue;
    MEMCPY(tmpbuf, p1, bytes);
    MEMCPY(p1, p2, bytes);
    MEMCPY(p2, tmpbuf, bytes);
  }
  FREE(tmpbuf);
}

void BltCanvas::maskColor(COLORREF from, COLORREF to) {
  int n = bm.bmWidth * bm.bmHeight;
  GdiFlush();
  DWORD *b = (DWORD *)getBits();
  from &= 0xffffff;
  while (n--) {
    if ((*b & 0xffffff) == from) {
      *b = to;
    } else *b |= 0xff000000;	// force all other pixels non masked
    b++;
  }
}

DDSurfaceCanvas::DDSurfaceCanvas(LPDIRECTDRAWSURFACE surface, int w, int h) {
  surf = surface;
  _w = w;
  _h = h;  
  hdc = NULL;
  bits = NULL;
}

DDSurfaceCanvas::~DDSurfaceCanvas() {
  if (isready())
    exit();
}

int DDSurfaceCanvas::isready() {
  return bits != NULL;
}

void DDSurfaceCanvas::enter() {
  DDSURFACEDESC d={sizeof(d),};
  if ((surf->Lock(NULL,&d,DDLOCK_WAIT,NULL)) != DD_OK)
    return;

  surf->GetDC(&hdc);

  bits = d.lpSurface;
}

void DDSurfaceCanvas::exit() {
  surf->ReleaseDC(hdc);
  surf->Unlock(bits);
  bits = NULL;
  hdc = NULL;
}













