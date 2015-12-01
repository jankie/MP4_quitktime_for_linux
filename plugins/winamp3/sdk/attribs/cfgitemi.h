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

#ifndef _CFGITEMI_H
#define _CFGITEMI_H

#include "cfgitem.h"

#include "../common/named.h"
#include "../common/ptrlist.h"
#include "../common/multimap.h"
#include "../common/depend.h"

class AttrCallback;
class Attribute;

// this is the one you inherit from/use
class CfgItemI : public CfgItem, public DependentI, private Named {
public:
  CfgItemI(const char *name=NULL);
  virtual ~CfgItemI();

  const char *cfgitem_getName();
  int cfgitem_getNumAttributes();
  const char *cfgitem_enumAttribute(int n);
  Dependent *cfgitem_getDependencyPtr() { return this; };

  virtual int cfgitem_getNumChildren();
  virtual CfgItem *cfgitem_enumChild(int n);
  virtual void cfgitem_shutdown();

  virtual int getAttributeType(const char *name);
  virtual int getDataLen(const char *name);
  virtual int getData(const char *name, void *data, int data_len);
  virtual int setData(const char *name, void *data, int data_len);

  // override these to catch notifications from attribs, call down
  virtual int cfgitem_onAttribSetValue(Attribute *attr);

  int setName(const char *name);
  int registerAttribute(Attribute *attr, AttrCallback *acb=NULL);
  // does not call delete on the attribute
  int deregisterAttribute(Attribute *attr);
  void deregisterAll();

  void addCallback(Attribute *attr, AttrCallback *acb);

  virtual void cfgitem_onRegisterAttribute(Attribute *attr);
  virtual void cfgitem_onDeregisterAttribute(Attribute *attr);

  

protected:
  RECVS_DISPATCH;

  Attribute *getAttributeByName(const char *name);
 
  void addChildItem(CfgItemI *child);

private:
  PtrList<Attribute> attributes;
  MultiMap<Attribute*, AttrCallback> callbacks;
  PtrList<CfgItemI> children;
};

#endif
