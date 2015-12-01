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

#ifndef _SERVICEI_H
#define _SERVICEI_H

#include "service.h"
#include "../../common/ptrlist.h"

class waServiceI : public waService {
protected:
  virtual int svc_serviceType()=0;
  virtual const char *svc_getServiceName()=0;
  virtual void *svc_getInterface()=0;
  virtual int svc_releaseInterface(void *)=0;
  virtual CfgItem *svc_getCfgInterface()=0;
  virtual int svc_notify(int msg, int param1=0, int param2=0)=0;

  RECVS_DISPATCH;
};

template <class SERVICETYPE, class SERVICE>
class waServiceT : public waServiceI {
public:
  virtual int svc_serviceType() { return SERVICETYPE::getServiceType(); }
  virtual const char *svc_getServiceName() { return SERVICE::getServiceName(); }
  virtual void *svc_getInterface() {
    SERVICETYPE *ret = new SERVICE;
    issued.addItem(ret);
    api->service_lock(this, ret);
    return ret;
  }
  virtual int svc_releaseInterface(void *ptr) {
    SERVICETYPE *svc = static_cast<SERVICETYPE*>(ptr);
    ASSERT(issued.haveItem(svc));
    if (issued.haveItem(svc)) {
      issued.removeItem(svc);
      delete static_cast<SERVICE*>(svc);
      return 1;
    }
    return 0;
  }
  virtual CfgItem *svc_getCfgInterface() { return NULL; }

  virtual int svc_notify(int msg, int param1=0, int param2=0) { return 0; }

private:
  PtrList<SERVICETYPE> issued;
};

#endif
