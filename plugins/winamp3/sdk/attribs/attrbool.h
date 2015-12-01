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

#ifndef _ATTRBOOL_H
#define _ATTRBOOL_H

#include "attribute.h"
#include "attrint.h"

// inherit from this one, or just use it
class _bool : public _int {
public:

  _bool(const char *name=NULL, int val=0);

  // convenience operators
  operator int() { return getValue(); }
  int operator =(int newval) { return setValue(!!newval) ? !!newval : getValue(); }

  // from AttributeI
  virtual int getAttributeType() { return AttributeType::BOOL; }
  virtual int getValue();
};

#endif
