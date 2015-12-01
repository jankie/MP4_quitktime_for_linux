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

#ifndef _SERVICE_H
#define _SERVICE_H

#include "../../common/dispatch.h"
#include "services.h"

namespace SvcNotify {
  enum {
    ONREGISTERED=100,	// init yourself here -- not all other services are registered yet
    ONSTARTUP=200,	// everyone is initialized, safe to talk to other services
    ONSHUTDOWN=300,	// studio is shutting down, release resources from other services
    ONDEREGISTERED=400,	// bye bye
    ONDBREADCOMPLETE=500,// after db is read in (happens asynchronously after ONSTARTUP)
  };
};

class CfgItem;

class waService : public Dispatchable {
public:
  int getServiceType();	// see services.h
  const char *getServiceName();

  void *getInterface();
  int releaseInterface(void *);	// when they're done w/ it

  CfgItem *getCfgInterface();

  int serviceNotify(int msg, int param1=0, int param2=0);

  enum {
    GETSERVICETYPE=100,
    GETSERVICENAME=200,
    GETINTERFACE=300,
    RELEASEINTERFACE=310,
    GETCFGINTERFACE=400,
    SERVICENOTIFY=500,
  };
};

inline int waService::getServiceType() {
  return _call(GETSERVICETYPE, WaSvc::NONE);
}

inline const char *waService::getServiceName() {
  return _call(GETSERVICENAME, "");
}

inline void *waService::getInterface() {
  return _call(GETINTERFACE, (void*)0);
}

inline int waService::releaseInterface(void *ptr) {
  return _call(RELEASEINTERFACE, 0, ptr);
}

inline CfgItem *waService::getCfgInterface() {
  return _call(GETCFGINTERFACE, (CfgItem*)NULL);
}

inline int waService::serviceNotify(int msg, int param1, int param2) {
  return _call(SERVICENOTIFY, 0, msg, param1, param2);
}

#endif
