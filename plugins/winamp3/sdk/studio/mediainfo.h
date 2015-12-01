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

#ifndef _MEDIAINFO_H
#define _MEDIAINFO_H

#include "../common/tlist.h"
#include "../common/string.h"
#include "../common/dispatch.h"
#include "services/svc_fileread.h"
#include "../common/tlist.h"

// classes used to pass metadatas around all core converters

class MediaInfo : public Dispatchable {
protected:
  MediaInfo() {} // protect constructor

public:
  void setData(const char *name, const char *value) { _voidcall(SETDATA,name,value); }
  void setDataInt(const char *name, int value) { _voidcall(SETDATAINT,name,value); }
  const char *getData(const char *name) { return _call(GETDATA,"",name); }
  int getDataInt(const char *name) { return _call(GETDATAINT,0,name); }

  const char *getFilename() { return _call(GETFILENAME,""); }
  void setFilename(const char *name) { _voidcall(SETFILENAME,name); }

  const char *getTitle() { return _call(GETTITLE,""); }
  void setTitle(const char *str) { _voidcall(SETTITLE,str); }
  const char *getTitle2() { return _call(GETTITLE2,""); }
  void setTitle2(const char *str) { _voidcall(SETTITLE2,str); }
  const char *getInfo() { return _call(GETINFO,""); }
  void setInfo(const char *str) { _voidcall(SETINFO,str); }
  const char *getUrl() { return _call(GETURL,""); }
  void setUrl(const char *str) { _voidcall(SETURL,str); }
  int getLength() { return _call(GETLENGTH,-1); }
  void setLength(int val) { _voidcall(SETLENGTH,val); }

  svc_fileReader *getReader() { return _call(GETREADER,(svc_fileReader *)NULL); }
  void setReader(svc_fileReader *r) { _voidcall(SETREADER,r); }

  void warning(const char *str) { _voidcall(WARNING,str); }
  void error(const char *str) { _voidcall(ERR,str); }
  void status(const char *str) { _voidcall(STATUS,str); }
  
  enum {
    GETFILENAME=100,
    SETFILENAME=110,

    SETDATA=200,
    SETDATAINT=210,
    GETDATA=220,
    GETDATAINT=230,
    
    GETTITLE=300,
    SETTITLE=310,
    GETTITLE2=320,
    SETTITLE2=330,
    GETINFO=340,
    SETINFO=350,
    GETURL=360,
    SETURL=370,
    GETLENGTH=380,
    SETLENGTH=390,

    GETREADER=500,
    SETREADER=510,

    WARNING=600,
    ERR=610,
    STATUS=620,
  };
};

class MediaInfoData {
public:
  MediaInfoData(const char *pname, const char *pvalue) : name(pname), value(pvalue) { }
  const char *getName() { return name; }
  const char *getValue() { return value; }
  void setValue(const char *pvalue) { value=pvalue; }
private:
  String name;
  RecycleString value;
};

class MediaInfoI : public MediaInfo {
public:
  MediaInfoI() { }
  virtual ~MediaInfoI() { infos.deleteAll(); }

  // functions used to set any metadata
  void setData(const char *name, const char *value) { 
    int l=infos.getNumItems();
    for(int i=0;i<l;i++) {
      if(!STRICMP(infos[i]->getName(),name)) {
        infos[i]->setValue(value);
        mediaInfo_onDataSet(name,value);
        return;
      }
    }
    infos.addItem(new MediaInfoData(name,value)); 
    mediaInfo_onDataSet(name,value);
  }
  void setDataInt(const char *name, int value) { char tmp[64]; itoa(value,tmp,10); setData(name,tmp); }
  const char *getData(const char *name) { 
    int l=infos.getNumItems();
    const char *ret = NULL;
    for(int i=0;i<l;i++) {
      MediaInfoData *info = infos[i];
      if (!STRICMP(info->getName(),name)) {
        ret = infos[i]->getValue();
        break;
      }
    }
    return ret ? ret : "";
  }
  int getDataInt(const char *name) { return ATOI(getData(name)); }

  // some basic helper functions
  const char *getFilename() { return getData("filename"); }
  void setFilename(const char *name) { setData("filename",name); }
  const char *getTitle() { return getData("title"); }
  void setTitle(const char *str) { setData("title",str); }
  const char *getTitle2() { return getData("title2"); }
  void setTitle2(const char *str) { setData("title2",str); }
  const char *getInfo() { return getData("info"); }
  void setInfo(const char *str) { setData("info",str); }
  const char *getUrl() { return getData("url"); }
  void setUrl(const char *str) { setData("url",str); }
  int getLength() { return getDataInt("length"); }
  void setLength(int val) { setDataInt("length",val); }
  svc_fileReader *getReader() { return (svc_fileReader *)getDataInt("reader"); }
  void setReader(svc_fileReader *r) { setDataInt("reader",(int)r); }
  
  // status infos
  void warning(const char *str) { setData("warning",str); } /* warning (something recoverable, like file not found, etc) */
  void error(const char *str) { setData("error",str); }     /* playback should stop (soundcard driver error, etc) */
  void status(const char *str) { setData("status",str); }   /* status update (nothing really wrong) */

  // callbacks
  virtual void mediaInfo_onDataSet(const char *name, const char *value) { }

protected:
  PtrList<MediaInfoData> infos;

#undef CBCLASS
#define CBCLASS MediaInfoI
START_DISPATCH;
  CB(GETFILENAME, getFilename);
  VCB(SETFILENAME, setFilename);

  VCB(SETDATA, setData);
  VCB(SETDATAINT, setDataInt);
  CB(GETDATA, getData);
  CB(GETDATAINT, getDataInt);

  CB(GETTITLE, getTitle);
  VCB(SETTITLE, setTitle);
  CB(GETTITLE2, getTitle2);
  VCB(SETTITLE2, setTitle2);
  CB(GETINFO, getInfo);
  VCB(SETINFO, setInfo);
  CB(GETURL, getUrl);
  VCB(SETURL, setUrl);
  CB(GETLENGTH, getLength);
  VCB(SETLENGTH, setLength);

  CB(GETREADER, getReader);
  VCB(SETREADER, setReader);

  VCB(WARNING, warning);
  VCB(ERR, error);
  VCB(STATUS, status);
END_DISPATCH;
#undef CBCLASS
};

#endif
