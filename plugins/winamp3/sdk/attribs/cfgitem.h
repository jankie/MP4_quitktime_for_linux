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

#ifndef _CFGITEM_H
#define _CFGITEM_H

#include "../common/dispatch.h"

class Dependent;

// abstract base class presented to the world
class NOVTABLE CfgItem : public Dispatchable {
public:
  const char *getName();
  int getNumAttributes();
  const char *enumAttribute(int n);

  // so people can watch you for changes
  Dependent *getDependencyPtr();

  // if you have child cfgitems, list them here
  int getNumChildren();
  CfgItem *enumChild(int n);
  void shutdown();

  virtual int getAttributeType(const char *name)=0;
  virtual int getDataLen(const char *name)=0;
  virtual int getData(const char *name, void *data, int data_len)=0;
  virtual int setData(const char *name, void *data, int data_len)=0;

  enum {
    GETNAME=100,
    GETNUMATTRIBUTES=200,
    ENUMATTRIBUTE=210,
    GETDEPENDENCYPTR=300,
    GETNUMCHILDREN=400,
    ENUMCHILD=410,
    SHUTDOWN=500,
  };
};

inline const char *CfgItem::getName() {
  return _call(GETNAME, "");
}

inline int CfgItem::getNumAttributes() {
  return _call(GETNUMATTRIBUTES, 0);
}

inline const char *CfgItem::enumAttribute(int n) {
  return _call(ENUMATTRIBUTE, (const char*)NULL, n);
}

inline Dependent *CfgItem::getDependencyPtr() {
  return _call(GETDEPENDENCYPTR, (Dependent*)NULL);
}

inline int CfgItem::getNumChildren() {
  return _call(GETNUMCHILDREN, 0);
}

inline CfgItem *CfgItem::enumChild(int n) {
  return _call(ENUMCHILD, (CfgItem*)NULL, n);
}

inline void CfgItem::shutdown() {
  _voidcall(SHUTDOWN);
}

inline int _intVal(CfgItem *cfgitem, const char *name, int def_val=0) {
  int ret;
  if (!cfgitem->getData(name, &ret, sizeof(ret))) return def_val;
  return ret;
}
#define _int_getValue _intVal

inline void _int_setValue(CfgItem *cfgitem, const char *name, int val) {
  cfgitem->setData(name, &val, sizeof(int));
}
// CfgItemI is in cfgitemi.h if you need it

#endif
