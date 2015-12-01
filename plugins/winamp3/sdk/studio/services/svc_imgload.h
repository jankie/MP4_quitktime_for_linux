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

#ifndef _SVC_IMGLOAD_H
#define _SVC_IMGLOAD_H

#include "../../common/dispatch.h"
#include "services.h"

class svc_imageLoader : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::IMAGELOADER; }
  
  int testHeader(const unsigned char *text);
  int *loadImage(const char *filename, int *has_alpha, int *w, int *h);

  enum {
    TESTHEADER=10,
    LOADIMAGE=20,
  };
};

inline int svc_imageLoader::testHeader(const unsigned char *text) {
  return _call(TESTHEADER, 0, text);
}

inline int *svc_imageLoader::loadImage(const char *filename, int *has_alpha, int *w, int *h) {
  return _call(LOADIMAGE, (int *)0, filename, has_alpha, w,h);
}

// derive from this one
class svc_imageLoaderI : public svc_imageLoader {
public:
  virtual int testHeader(const unsigned char *text)=0;
  virtual int *loadImage(const char *filename, int *has_alpha, int *w, int *h)=0;

protected:
  RECVS_DISPATCH;
};

#endif
