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

#ifndef _SVC_XMLPROVIDER_H
#define _SVC_XMLPROVIDER_H

#include "../../common/dispatch.h"
#include "services.h"

class svc_xmlProvider : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::XMLPROVIDER; }

  int testDesc(const char *desc);
  const char *getXmlData(const char *desc, int *length);
  void doneWithXmlData(const char *data);

  enum {
    TESTDESC=10,
    GETXMLDATA=20,
    DONEWITHXMLDATA=30,
  };
};

inline int svc_xmlProvider::testDesc(const char *desc) {
  return _call(TESTDESC, 0, desc);
}

inline const char *svc_xmlProvider::getXmlData(const char *desc, int *size) {
  return _call(GETXMLDATA, (const char *)0, desc, size);
}

inline void svc_xmlProvider::doneWithXmlData(const char *data) {
  _call(DONEWITHXMLDATA, 0, data);
}

// derive from this one
class svc_xmlProviderI : public svc_xmlProvider {
public:
  virtual int testDesc(const char *desc)=0;
  virtual const char *getXmlData(const char *desc, int *size)=0;
  virtual void doneWithXmlData(const char *data)=0;

protected:
  RECVS_DISPATCH;
};

#endif


