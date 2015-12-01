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

#include "bucketitem.h"
#include "usermsg.h"
#include "../studio/api.h"

BucketItem::BucketItem() {
  guid_target = INVALID_GUID;
  setBorders(0);
}

BucketItem::~BucketItem() {
  api->syscb_deregisterCallback(this);
}

int BucketItem::onInit() {
  BUCKETITEM_PARENT::onInit();
  api->syscb_registerCallback(this);
  return 1;
}

void BucketItem::setBucketText(const char *txt) {
  notifyParent(UMSG_COMPONENTBUCKET_SETTEXT, reinterpret_cast<int>(txt), 0);
}

void BucketItem::onLeftPush(int x, int y) {
  BUCKETITEM_PARENT::onLeftPush(x, y);
  if (MEMCMP(&guid_target, &INVALID_GUID, sizeof(GUID))) {
    RECT r;
    getClientRect(&r);
    clientToScreen(&r);
    if (!api->isComponentVisible(guid_target)) {
      api->setComponentVisible(guid_target, 1, &r);
      setActivatedButton(1);
    } else {
      api->setComponentVisible(guid_target, 0, &r);
      setActivatedButton(0);
    }
  }
}

void BucketItem::onEnterArea() {
  BUCKETITEM_PARENT::onEnterArea();
  if (!target_txt.isempty()) setBucketText(target_txt);
}

void BucketItem::onLeaveArea() {
  BUCKETITEM_PARENT::onLeaveArea();
  if (!target_txt.isempty()) setBucketText(NULL);
}

void BucketItem::setAutoOpen(GUID g) {
  guid_target = g;
}

void BucketItem::setAutoText(const char *txt) {
  target_txt = txt;
}

int BucketItem::onShowWindow(GUID g) {
  if (g == guid_target) setActivatedButton(1);
  return 1;
}

int BucketItem::onHideWindow(GUID g) {
  if (g == guid_target) setActivatedButton(0);
  return 1;
}

