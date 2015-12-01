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

#include "attrstr.h"

#include "../common/string.h"

_string::_string(const char *_name, const char *val)
  : Attribute(_name), value(val)
{}

const char *_string::getValue() {
  return value;
}

const char *_string::setValue(const char *val) {
  return (value = val);
}

int _string::getDataLen() {
  return value.len()+1;
}

int _string::getData(void *ptr, int len) {
  int l = MIN(len, value.len()+1);
  MEMCPY(ptr, value.getValue(), l);
  return l;
}

int _string::setData(void *ptr, int len) {
  value = (const char *)ptr;
  return 1;
}

//CUT #define CBCLASS _string
//CUT START_DISPATCH;
//CUT   CB(AttributeString::GETVALUE, getValue);	//yay, cheating! :)
//CUT   CB(AttributeString::SETVALUE, setValue);
//CUT FORWARD_DISPATCH(AttributeI);
//CUT #undef CBCLASS
