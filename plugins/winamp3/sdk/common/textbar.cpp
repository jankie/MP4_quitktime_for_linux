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


#include "textbar.h"
#include "canvas.h"

#include "skinclr.h"

static SkinColor bgcolor("studio.textBar.background");
static SkinColor fgcolor("studio.textBar.text");

TextBar::TextBar() {
  size = 16;
  usebt = 0;
}

void TextBar::setUseBaseTexture(int u) {
  usebt = u;
  invalidate();
}

int TextBar::onPaint(Canvas *canvas) {
  RECT r;

  PaintCanvas paintcanvas;
  if (canvas == NULL) {
    if (!paintcanvas.beginPaint(this)) return 0;
    canvas = &paintcanvas;
  }
  TEXTBAR_PARENT::onPaint(canvas);

  getClientRect(&r);

  if (!usebt)
    canvas->fillRect(&r, bgcolor);
  else
    renderBaseTexture(canvas, r);

  const char *name = getName();

  if (name != NULL) {
    canvas->setTextOpaque(FALSE);
//    canvas->setTextVariable(TRUE); //FGTEMP!
    canvas->setTextColor(fgcolor);
    canvas->pushTextSize(size);
    int w, h;
    canvas->getTextExtent(name, &w, &h);
    int y = (r.bottom-r.top - h) / 2;
    int x = centered ? (r.right-r.left - w) / 2 : TEXTBAR_LEFTMARGIN;
    canvas->textOut(r.left+x, r.top+y, getName());
    canvas->popTextSize();
  }
  return 1;
}

int TextBar::setTextSize(int newsize) {
  if (newsize < 1 || newsize > 72) return 0;
  size = newsize;
  invalidate();
  return 1;
}

int TextBar::setInt(int i) {
  char txt[16];
  wsprintf(txt, "%d", i);
  setName(txt);
  invalidate();
  return 1;
}

void TextBar::onSetName() {
  TEXTBAR_PARENT::onSetName();
  invalidate();
}

int TextBar::getTextWidth() {
  if (!getName()) return 0;
  BltCanvas *c = new BltCanvas(10, 10);
//  c->setTextVariable(TRUE); //FGTEMP!
  c->pushTextSize(size);
  int r = c->getTextWidth(getName());
  c->popTextSize();
  delete c;
  return r+4;
}

int TextBar::getTextHeight() {
  return size;
}

void TextBar::setCenter(int center) {
  int was = centered;
  centered = !!center;
  if (was != centered) invalidate();
}
