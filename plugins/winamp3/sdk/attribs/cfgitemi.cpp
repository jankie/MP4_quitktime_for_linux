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


#include "cfgitemi.h"
#include "attrcb.h"
#include "attribute.h"

#include "../common/std.h"
#include "../studio/api.h"

CfgItemI::CfgItemI(const char *name)
  : Named(name)
{ }

CfgItemI::~CfgItemI() {
  deregisterAll();
}

const char *CfgItemI::cfgitem_getName() {
  return Named::getName();
}

int CfgItemI::cfgitem_getNumAttributes() {
  return attributes.getNumItems();
}

const char *CfgItemI::cfgitem_enumAttribute(int n) {
  Attribute *attr = attributes[n];
  if (attr) return attr->getAttributeName();
  return NULL;
}

int CfgItemI::cfgitem_getNumChildren() {
  return children.getNumItems();
}

CfgItem *CfgItemI::cfgitem_enumChild(int n) {
  return children[n];
}

Attribute *CfgItemI::getAttributeByName(const char *name) {
  Attribute *attr;
  for (int i = 0; i < attributes.getNumItems(); i++) {
    attr = attributes[i];
    if (!STRICMP(name, attr->getAttributeName())) return attr;
  }
  return NULL;
}

int CfgItemI::getAttributeType(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return AttributeType::NONE;
  return attr->getAttributeType();
}

int CfgItemI::getDataLen(const char *name) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  return attr->getDataLen();
}

int CfgItemI::getData(const char *name, void *data, int data_len) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  return attr->getData(data, data_len);
}

int CfgItemI::setData(const char *name, void *data, int data_len) {
  Attribute *attr = getAttributeByName(name);
  if (attr == NULL) return -1;
  return attr->setData(data, data_len);
}

int CfgItemI::cfgitem_onAttribSetValue(Attribute *attr) {
  // notify dependency watchers that an attribute changed
  dependent_changedAttr(attr->getAttributeName());

  for (int i = 0; ; i++) {
    AttrCallback *acb;
    if (!callbacks.multiGetItem(attr, i, &acb)) break;
    acb->onValueChange(attr);
  }
  
  return 0;
}

int CfgItemI::setName(const char *name) {
  Named::setName(name);
//FUCKO: notify dependency watchers that name changed?
  return 1;
}

int CfgItemI::registerAttribute(Attribute *attr, AttrCallback *acb) {
  if (attributes.haveItem(attr)) return 0;
  int ret = attributes.addItem(attr) != NULL;
  if (!ret) return ret;
  attr->setCfgItem(this);

  // set optional callback
  if (acb != NULL)
    addCallback(attr, acb);

  // notify derived classes too
  cfgitem_onRegisterAttribute(attr);

  // notify dependency watchers of new attribute
  dependent_addedAttr(attr->getAttributeName());

  return ret;
}

int CfgItemI::deregisterAttribute(Attribute *attr) {
  if (!attributes.haveItem(attr)) return 0;
  int ret = attributes.removeItem(attr);
  // notify dependency watchers of attribute going away
//  dependent_deletedAttr(attr->getAttributeName());
  // tell attribute
  attr->setCfgItem(NULL);
  // notify derived classes too, should be safe if they want to delete here
  cfgitem_onDeregisterAttribute(attr);

  // remove callbacks
  callbacks.multiDelAllForItem(attr, TRUE);

  return ret;
}

void CfgItemI::addCallback(Attribute *attr, AttrCallback *acb) {
  ASSERT(attr != NULL);
  ASSERT(acb != NULL);
  callbacks.multiAddItem(attr, acb);
}

void CfgItemI::cfgitem_onRegisterAttribute(Attribute *attr) {
  switch (attr->getAttributeType()) {
    case AttributeType::BOOL: 
    case AttributeType::INT: {
      int v = _int_getValue(attr);	// current val
      int ov = api->getIntPrivate(attr->getAttributeName(), v); // disk val
      // force setting of it in case they have a callback or dependency viewer
      _int_setValue(attr, ov);
    }
    break;
  }
}

void CfgItemI::cfgitem_onDeregisterAttribute(Attribute *attr) {
  switch (attr->getAttributeType()) {
    case AttributeType::BOOL: 
    case AttributeType::INT: {
      api->setIntPrivate(attr->getAttributeName(), _int_getValue(attr));
    }
    break;
  }
//  delete attr;//FUCKO: how do we know which to delete?
}

void CfgItemI::deregisterAll() {
  while (attributes.getNumItems()) deregisterAttribute(attributes[0]);
}

void CfgItemI::addChildItem(CfgItemI *child) {
  if (!children.haveItem(child))
    children.addItem(child);
}

void CfgItemI::cfgitem_shutdown() {
  deregisterAll();
}

#define CBCLASS CfgItemI
START_DISPATCH
  CB(GETNAME, cfgitem_getName);
  CB(GETNUMATTRIBUTES, cfgitem_getNumAttributes);
  CB(ENUMATTRIBUTE, cfgitem_enumAttribute);
//CUT  CB(GETATTRIBUTEBYNAME, cfgitem_getAttributeByName);
  CB(GETDEPENDENCYPTR, cfgitem_getDependencyPtr);
  CB(GETNUMCHILDREN, cfgitem_getNumChildren);
  CB(ENUMCHILD, cfgitem_enumChild);
  VCB(SHUTDOWN, cfgitem_shutdown);
END_DISPATCH
#undef CBCLASS
