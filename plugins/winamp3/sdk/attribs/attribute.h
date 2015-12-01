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

#ifndef _ATTRIBUTE_H
#define _ATTRIBUTE_H

#include "../common/std.h"
#include "../common/dispatch.h"
#include "../common/named.h"

class CfgItem;
class CfgItemI;

// lowercase types are reserved for official Nullsoft use
// uppercase are 3rd-party defined
namespace AttributeType {
  enum {
    NONE = 0,
    INT = 'int',
    STRING = 'str',
    BOOL = 'bool',
  };
};

class NOVTABLE Attribute : private Named {
protected:
  Attribute(const char *newname=NULL);
public:
  virtual ~Attribute();

  void setName(const char *newname);
  const char *getAttributeName();
  virtual int getAttributeType()=0;

  // the cfgitem this attribute is attached to (convenience fn)
  void setCfgItem(CfgItemI *cfgitem);
  CfgItem *getCfgItem();

  virtual int getDataLen()=0;
  virtual int getData(void *ptr, int len)=0;
  virtual int setData(void *ptr, int len)=0;

protected:
  void _valueSet();

private:
  CfgItemI *cfgitem;
};

inline int _int_getValue(Attribute *attr, int def_val=0) {
  ASSERT(attr->getAttributeType() == AttributeType::INT ||
         attr->getAttributeType() == AttributeType::BOOL);
  int ret;
  if (!attr->getData(&ret, sizeof(ret))) return def_val;
  return ret;
}

inline int _int_setValue(Attribute *attr, int val) {
  ASSERT(attr->getAttributeType() == AttributeType::INT ||
         attr->getAttributeType() == AttributeType::BOOL);
  return attr->setData(&val, sizeof(int));
}


#if 0//CUT
class NOVTABLE AttributeInt : public Attribute {
public:
  int getValue();
  int setValue(int val);
//  int getRange(int *lower, int *upper, int *step);

  enum {
    GETVALUE = 10000,
    SETVALUE = 10010,
  };
};

class AttributeString : public Attribute {
public:
  const char *getValue();
  const char *setValue(const char *val);

  enum {
    GETVALUE = 11000,
    SETVALUE = 11010,
  };
};

inline const char *Attribute::getAttributeName() {
  return _call(GETATTRIBUTENAME, "");
}

inline int Attribute::getAttributeType() {
  return _call(GETATTRIBUTETYPE, -1);
}

inline int Attribute::getPermissions() {
  return _call(GETPERMISSIONS, 0);
}

inline CfgItem *Attribute::getCfgItem() {
  return _call(GETCFGITEM, (CfgItem*)0);
}

inline int Attribute::getRawDataLen() {
  return _call(GETRAWDATALEN, 0);
}

inline int Attribute::getRawData(void *ptr, int maxlen) {
  return _call(GETRAWDATA, 0, ptr, maxlen);
}

inline int AttributeInt::getValue() {
  return _call(GETVALUE, 0);
}

inline int AttributeInt::setValue(int val) {
  return _call(SETVALUE, 0, val);
}

inline const char *AttributeString::getValue() {
  return _call(GETVALUE, (const char *)0);
}

inline const char *AttributeString::setValue(const char *val) {
  return _call(SETVALUE, (const char *)0, val);
}
#endif

#define ATTR_PERM_READ	1
#define ATTR_PERM_WRITE	2

#define ATTR_PERM_ALL	(~0)

// render hints for getRenderHint
enum {
  ATTR_RENDER_HINT_INT_CHECKMARK
};

#if 0
// an AttributeInt adds these methods
class NOVTABLE AttributeInt {
public:
  virtual int getValue()=0;
  virtual int setValue(int val)=0;
  virtual int getRange(int *lower, int *upper, int *step)=0;
//  virtual int setRange(int lower, int upper, int step)=0;
};

class NOVTABLE AttributeString {
public:
  virtual const char *getValue()=0;
  virtual const char *setValue(const char *val)=0;
};
#endif

#endif
