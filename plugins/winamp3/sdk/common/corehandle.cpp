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

#include "rootcomp.h"
#include "metatags.h"
#include "../attribs/cfgitem.h"
#include "corehandle.h"
#include "../studio/api.h"
#include "../studio/buttons.h"

using namespace UserButton;

CoreHandle::CoreHandle(CoreToken tok) {
  if (tok != 0)
    token = api->core_create();
  else token = 0;
  api->core_addCallback(token, this);
}

CoreHandle::~CoreHandle() {
  api->core_delCallback(token, this);
  if (token != 0) api->core_free(token);
}

int CoreHandle::setNextFile(const char *playstring) {
  return api->core_setNextFile(token, playstring);
}

void CoreHandle::prev() {
  userButton(PREV);
}

void CoreHandle::play() {
  userButton(PLAY);
}

void CoreHandle::pause() {
  userButton(PAUSE);
}

void CoreHandle::stop() {
  userButton(STOP);
}

void CoreHandle::next() {
  userButton(NEXT);
}

void CoreHandle::userButton(int button) {
  api->core_userButton(token, button);
}
