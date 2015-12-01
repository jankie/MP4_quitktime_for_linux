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

#ifndef _STRING_H
#define _STRING_H

#include "common.h"

// a very, very basic string... I just got sick of all the STRDUPs --BU
// ok, not so basic any more =) --BU

class COMEXP String {
public:
  String(const char *initial_val=NULL);
  String(const String &s);
  ~String();

  const char *getValue() const { return val; }
  operator const char *() const { return getValue(); }

  const char *setValue(const char *newval);
  char *getNonConstVal() { return val; }
  const char *operator =(const char *newval) { return setValue(newval); }
  const char *operator +=(const char *addval) {
    return cat(addval);
  }
  const char *operator +=(int value);

  // copy assignment operator
  String& operator =(const String &s) {
    if (this != &s)
      setValue(s);
    return *this;
  }

  int len() const;
  int isempty();
  void toupper();	// converts entire string to uppercase
  void tolower();	// converts entire string to lowercase
  int isequal(const char *val) const;	// basically !strcmp

  // replaces string
  const char *printf(const char *format, ...);
  // adds to the end
  const char *cat(const char *value);

protected:
  void vsprintf(const char *format, va_list args);

  char *val;
  enum { wastage_allowed = 16 };
};

// use this one if you want to use a printf-style constructor
class COMEXP StringPrintf : public String {
public:
  StringPrintf(const char *format=NULL, ...);
  StringPrintf(int val);
  StringPrintf(GUID g);
};

class COMEXP RecycleString : public String {
public:
  RecycleString(const char *initial_val=NULL);
  const char *operator =(const char *newval) { return reuseValue(newval); }

protected:
  const char *reuseValue(const char *newval);
};

inline const char *String::operator +=(int value)
{
  return cat(StringPrintf("%i",value));
}

class StringComparator {
public:
  // comparator for sorting
  static int compareItem(String *p1, String* p2) {
    return STRCMP(p1->getValue(), p2->getValue());
  }
  // comparator for searching
  static int compareAttrib(char *attrib, String *item) {
    return STRCMP(attrib, item->getValue());
  }
};

#endif
