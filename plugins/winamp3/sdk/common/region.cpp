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
#include "region.h"

#include "canvas.h"
#include "basewnd.h"

Region::Region() {
	hrgn = CreateRectRgn(0,0,0,0);
}

Region::Region(RECT *r) {
	hrgn = CreateRectRgn(r->left,r->top,r->right,r->bottom);
}

Region::Region(int l, int t, int r, int b) {
	hrgn = CreateRectRgn(l,t,r,b);
}

Region::Region(HRGN r) {
  HRGN R = CreateRectRgn(0, 0, 0,0);
  CombineRgn(R, r, r, RGN_COPY);
  hrgn = R;
}

Region::Region(Canvas *c) {
	hrgn = CreateRectRgn(0,0,0,0);
	GetClipRgn(c->getHDC(), hrgn);
}

Region::~Region() {
	if (hrgn)
		DeleteObject(hrgn);
} 

Region *Region::clone() {
  Region *newregion = new Region(getHRGN());
  return newregion;
}

void Region::disposeClone(Region *r) {
  delete r; // todo: validate pointer before deleting
}

// returns a handle that SetWindowRgn understands (non portable). We should NOT delete this handle, windows will delete 
// it by itself upon setting a new region of destroying the window
HRGN Region::makeWindowRegion() {
  HRGN R = CreateRectRgn(0, 0, 0,0);
  CombineRgn(R, hrgn, hrgn, RGN_COPY);
  return R;
}

Region::Region(SkinBitmap *bitmap, int xoffset, int yoffset, BOOL inverted, int dothreshold, char threshold, int thinverse) {
  hrgn = alphaToRegionRect(bitmap, xoffset, yoffset, FALSE, 0, 0, 0, 0, inverted, dothreshold, threshold, thinverse);
}

Region::Region(SkinBitmap *bitmap, int xoffset, int yoffset, int _x, int _y, int _w, int _h, BOOL inverted, int dothreshold, char threshold, int thinverse) {
  hrgn = alphaToRegionRect(bitmap, xoffset, yoffset, TRUE, _x, _y, _w, _h, inverted, dothreshold, threshold, thinverse);
}

HRGN Region::alphaToRegionRect(SkinBitmap *bitmap, int xoffset, int yoffset, BOOL portion, int _x, int _y, int _w, int _h, BOOL inverted, int dothreshold, unsigned char threshold, int thinverse) {

	HRGN hRgn = NULL;
  int bmWidth=bitmap->getWidth();
  int bmHeight=bitmap->getHeight();
  int fullw=bitmap->getFullWidth();

  void *pbits32=bitmap->getBits();
  if (!pbits32) return NULL;

	RGNDATA *pData;
	int y, x;

	// For better performances, we will use the ExtCreateRegion() function to create the
	// region. This function take a RGNDATA structure on entry. We will add rectangles by
	// amount of ALLOC_UNIT number in this structure.
  // JF> rects are 8 bytes, so this allocates just under 16kb of memory, no need to REALLOC
	#define MAXRECTS 2000
	pData = (RGNDATA *)MALLOC(sizeof(RGNDATAHEADER) + (sizeof(RECT) * MAXRECTS));
  if (!pData) return NULL;

	pData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pData->rdh.iType = RDH_RECTANGLES;
	pData->rdh.nCount = pData->rdh.nRgnSize = 0;

  SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

  int x_end=(portion ? _w+_x-bitmap->getX() : bmWidth);
  int y_end=(portion ? _h+_y-bitmap->getY() : bmHeight);
  int x_start=(portion ? _x-bitmap->getX() : 0);
  int y_start=(portion ? _y-bitmap->getY() : 0);

  unsigned int iv=inverted?0xff000000:0;

  int xo=bitmap->getX();
  int yo=bitmap->getY();
	for (y = y_start; y < y_end; y++) {
		// Scan each bitmap pixel from left to right
    unsigned int *lineptr=((unsigned int *)pbits32) + fullw*(y+yo) + xo;
		for (x = x_start; x < x_end; x++) {
			// Search for a continuous range of "non transparent pixels"
			int x0 = x;
    	unsigned int *p = lineptr;
      if (dothreshold) {
        if (thinverse) {
				  while (x < x_end) {
            unsigned int a=p[x];
					  if ((a&0xff000000) == iv || 
                (((((a & 0xFF) > threshold || ((a & 0xFF00) >> 8) > threshold || ((a & 0xFF0000) >> 16) > threshold)))))
								  break;
					  x++;
				  }
        }
        else {
				  while (x < x_end) {
            unsigned int a=p[x];
					  if ((a&0xff000000) == iv || 
                (((((a & 0xFF) < threshold || ((a & 0xFF00) >> 8) < threshold || ((a & 0xFF0000) >> 16) < threshold)))))
								  break;
					  x++;
				  }
        }
      }
      else {
  			while (x < x_end) {
		  		if ((p[x] & 0xFF000000) == iv) break;
          x++;
        }
      }

			if (x > x0) {
				SetRect(((RECT *)&pData->Buffer) + pData->rdh.nCount, x0, y, x, y+1);

        pData->rdh.nCount++;

				if (x0 < pData->rdh.rcBound.left) pData->rdh.rcBound.left = x0;
				if (y < pData->rdh.rcBound.top) pData->rdh.rcBound.top = y;
				if (x > pData->rdh.rcBound.right) pData->rdh.rcBound.right = x;
				if (y >= pData->rdh.rcBound.bottom) pData->rdh.rcBound.bottom = y+1;

				// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
				// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
				if (pData->rdh.nCount == MAXRECTS) {
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * pData->rdh.nCount), pData);
					if (hRgn) {
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else hRgn = h;
					pData->rdh.nCount = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
				}
			}
		}
	}

	// Create or extend the region with the remaining rectangles
	HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * pData->rdh.nCount), pData);
	if (hRgn)
	{
		CombineRgn(hRgn, hRgn, h, RGN_OR);
		DeleteObject(h);
	}
	else
		hRgn = h;

	// Clean up
	FREE(pData);

  if (hRgn && (xoffset || yoffset))
    OffsetRgn(hRgn, xoffset, yoffset);


	return hRgn;
}

BOOL Region::ptInRegion(POINT *pt) {
  ASSERT(hrgn != NULL);
  return PtInRegion(hrgn, pt->x, pt->y);
}

void Region::offset(int x, int y) {
  ASSERT(hrgn != NULL);
  OffsetRgn(hrgn, x, y);
}

void Region::getBox(RECT *r) {
  GetRgnBox(hrgn, r);
}

HRGN Region::getHRGN() {
  return hrgn;
}

void Region::subtract(RECT *r) {
  Region *s = new Region(r);
  subtract(s);
  delete s;
}

void Region::subtract(Region *reg) {
  CombineRgn(hrgn, hrgn, reg->getHRGN(), RGN_DIFF);
}

void Region::and(Region *reg) {
  CombineRgn(hrgn, hrgn, reg->getHRGN(), RGN_AND);
}

void Region::addRect(RECT *r) {
  Region *a = new Region(r);
  add(a);
  delete a;
}

void Region::add(Region *reg) {
  ASSERT(reg != NULL);
  CombineRgn(hrgn, hrgn, reg->getHRGN(), RGN_OR);
}

int Region::isEmpty() {
  RECT r;
  getBox(&r);
  if (r.left == r.right && r.bottom == r.top) return 1;
  return 0;
}

int Region::enclosed(Region *r) {
  HRGN h = CreateRectRgn(0,0,0,0);
  int rs = CombineRgn(h, hrgn, r->getHRGN(), RGN_DIFF);
  DeleteObject(h);
  return rs == NULLREGION;
}

int Region::enclosed(Region *r, Region *outside) {
  int rs = CombineRgn(outside->getHRGN(), hrgn, r->getHRGN(), RGN_DIFF);
  return rs == NULLREGION;
}

#define IntersectRgn(hrgnResult, hrgnA, hrgnB) CombineRgn(hrgnResult, hrgnA, hrgnB, RGN_AND)

int Region::intersects(Region *r) {
  ASSERT(r != NULL);
  HRGN res = CreateRectRgn(0,0,0,0);
  int rs = IntersectRgn(res, hrgn, r->getHRGN());
  DeleteObject(res);
  return (rs != NULLREGION && rs != ERROR);
}

int Region::intersects(Region *r, Region *intersection) {
  ASSERT(r != NULL);
  ASSERT(intersection != NULL);
  int rs = IntersectRgn(intersection->getHRGN(), hrgn, r->getHRGN());
  return (rs != NULLREGION && rs != ERROR);
}

int Region::intersects(RECT *r) {
  return RectInRegion(hrgn, r);
}

int Region::intersects(RECT *r, Region *intersection) {
  Region *rect = new Region(r);
  int res = intersects(rect, intersection);
  delete rect;
  return res;
}

void Region::getRgnBox(RECT *r) {
  ASSERT(r != NULL);
  GetRgnBox(hrgn, r);
}

void Region::empty() {
  ASSERT(hrgn != NULL);
  DeleteObject(hrgn);
  hrgn = CreateRectRgn(0,0,0,0);
  ASSERT(hrgn != NULL);
}

void Region::setRect(RECT *r) {
  ASSERT(hrgn != NULL);
  SetRectRgn(hrgn, r->left, r->top, r->right, r->bottom);
}

int Region::equals(Region *r) {
  ASSERT(r);
  Region *cl = r->clone();
  cl->subtract(this);
  int ret = cl->isEmpty();
  r->disposeClone(cl);
  cl = clone();
  cl->subtract(r);
  ret &= cl->isEmpty();
  disposeClone(cl);
  return ret;
}

int Region::isRect() {
  RECT r;
  getBox(&r);
  Region *n = new Region(&r);
  ASSERT(n != NULL);
  if (equals(n)) {
    delete n;
    return 1;
  }
  delete n;
  return 0;
}

void Region::scale(double s, BOOL round) {
  scale(s, s, round);
}

void Region::scale(double sx, double sy, BOOL round) {
  ASSERT(hrgn != NULL);
  DWORD size=0;
  size = GetRegionData(hrgn, size, NULL); 
  if (!size) return;
  DWORD res;
  RGNDATA *data = (RGNDATA *)MALLOC(size);
  RECT *r = (RECT *)data->Buffer;

  res = GetRegionData(hrgn, size, (RGNDATA *)data);
  double adj = round?0.99999:0.0;
  int iadj = round?1:0;

  if (data->rdh.nCount == 1) {
    RECT nr = data->rdh.rcBound;
    nr.left = (int)((double)(nr.left-iadj) * sx);
    nr.top = (int)((double)(nr.top-iadj) * sy);
    nr.right = (int)((double)nr.right * sx + adj);
    nr.bottom = (int)((double)nr.bottom * sy + adj);
    setRect(&nr);
    FREE(data);
    return;
  }

  for (int i=0;i<(int)data->rdh.nCount;i++) {
    r[i].left = (int)((double)(r[i].left-iadj) * sx);
    r[i].top = (int)((double)(r[i].top-iadj) * sy);
    r[i].right = (int)((double)r[i].right * sx + adj);
    r[i].bottom = (int)((double)r[i].bottom * sy + adj);
  }

  HRGN nhrgn = ExtCreateRegion(NULL, size, data);
  ASSERTPR(nhrgn != NULL, StringPrintf("size : %s", size));
  FREE(data);
  DeleteObject(hrgn);
  hrgn = nhrgn;
}

void Region::debug() {
  HDC d = GetDC(NULL);
  InvertRgn(d, getHRGN());
  Sleep(1000);
  InvertRgn(d, getHRGN());
}
