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

#ifndef _SVC_ENUM_H
#define _SVC_ENUM_H

#include "common.h"

#include "memblock.h"

#include "../studio/services/service.h"
#include "../studio/services/services.h"

template <class T>
class castService {
public:
  castService(waService *w) : was(w) { }
  operator T *() {
    if (was == NULL) return NULL;
    return static_cast<T *>(was->getInterface());
  }
private:
  waService *was;
};

class NOVTABLE COMEXP SvcEnum {
protected:
  SvcEnum();

  waService *getNext();
  void reset();

  virtual int type()=0;
  virtual int _testService(waService *)=0;

public:
  static int release(void *ptr);

private:
  int pos;
};

template <class T>
class SvcEnumT : private SvcEnum {
public:
  void reset() { SvcEnum::reset(); }
  T *getFirst() { reset(); return getNext(); }
  T *getNext() { return castService<T>(SvcEnum::getNext()); }

  static int release(T *ptr) { return SvcEnum::release(ptr); }

protected:
  virtual int type() { return T::getServiceType(); }
  virtual int _testService(waService *svc) {
    return testService(castService<T>(svc));
  }

  virtual testService(T *svc)=0;
};

#include "named.h"
#include "../studio/services/svc_itemmgr.h"

class ItemMgrEnum : private Named, public SvcEnumT<svc_itemMgr> {
public:
  ItemMgrEnum(const char *ps) : Named(ps) { }

  void setPlaystring(const char *ps) { Named::setName(ps); }

protected:
  virtual int testService(svc_itemMgr *svc) {
    return svc->isMine(getName());
  }
};

#include "../studio/services/svc_imgload.h"

class ImgLoaderEnum : public SvcEnumT<svc_imageLoader> {
public:
  ImgLoaderEnum(unsigned char *data, int datalen) : mem(datalen, data) { }

protected:
  virtual int testService(svc_imageLoader *svc) {
    return svc->testHeader(mem);
  }

private:
  MemBlock<unsigned char> mem;
};

#include "../studio/services/svc_imggen.h"

class ImgGeneratorEnum : public SvcEnumT<svc_imageGenerator> {
public:
  ImgGeneratorEnum(const char *_desc) : desc(_desc) { }

protected:
  virtual int testService(svc_imageGenerator *svc) {
    return svc->testDesc(desc);
  }

private:
  String desc;
};


#include "../studio/services/svc_xmlprov.h"

class XmlProviderEnum : public SvcEnumT<svc_xmlProvider> {
public:
  XmlProviderEnum(const char *_desc) : desc(_desc) { }

protected:
  virtual int testService(svc_xmlProvider *svc) {
    return svc->testDesc(desc);
  }

private:
  String desc;
};

#include "../studio/services/svc_scriptobj.h"

class ExternalScriptObjectEnum : public SvcEnumT<svc_scriptObject> {
public:
  ExternalScriptObjectEnum() { }

protected:
  virtual int testService(svc_scriptObject*svc) {
    return (svc->getControler() != NULL);
  }
};


#include "../studio/services/svc_filesel.h"

class FileSelectorEnum : public SvcEnumT<svc_fileSelector> {
public:
  FileSelectorEnum(const char *_prefix="files") : prefix(_prefix) { }

protected:
  virtual int testService(svc_fileSelector *svc) {
    return svc->testPrefix(prefix);
  }

private:
  String prefix;
};

#include "../studio/services/svc_wndcreate.h"

class WindowCreateEnum : public SvcEnumT<svc_windowCreate> {
public:
  WindowCreateEnum(GUID _g) : targetguid(_g) { }

protected:
  virtual int testService(svc_windowCreate *svc) {
    if (!MEMCMP(&targetguid, &GENERIC_GUID, sizeof(GUID))) return 1;
    return (svc->testGuid(targetguid) != NULL);
  }

private:
  GUID targetguid;
};


#endif
