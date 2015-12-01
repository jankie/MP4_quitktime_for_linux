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


#include "attrint.h"

_int::_int(const char *name, int val) : Attribute(name) {
  value = val;
}

//CUT int _int::getPermissions() {
//CUT   return perms;
//CUT }

int _int::getData(void *ptr, int len) {
  if (len == sizeof(getDataLen())) {
    *(int *)ptr = getValue();
    return 1;
  }
  return 0;
}

int _int::setData(void *ptr, int len) {
  if (len != 4) return 0;
  return setValue(*(int *)ptr);
}

int _int::getValue() {
  return value;
}

int _int::setValue(int val) {
  value = val;
  _valueSet();	// notify cfgitem
  return 1;
}

