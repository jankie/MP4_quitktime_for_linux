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

#ifndef __BUCKETITEM_H
#define __BUCKETITEM_H

#include "buttwnd.h"
#include "common.h"
#include "wndcb.h"

#define BUCKETITEM_PARENT ButtonWnd

class COMEXP BucketItem : public BUCKETITEM_PARENT, public WndCallbackI {
  public:

    BucketItem();
    virtual ~BucketItem();
   
    virtual void setBucketText(const char *txt);
    virtual void onLeftPush(int x, int y);

    virtual int onShowWindow(GUID g);
    virtual int onHideWindow(GUID g);

    virtual void onEnterArea();
    virtual void onLeaveArea();

    void setAutoOpen(GUID g);
    void setAutoText(const char *txt);

    int onInit();
  
  private:
    GUID guid_target;
    String target_txt;
};

#endif // __BUCKETITEM_H
