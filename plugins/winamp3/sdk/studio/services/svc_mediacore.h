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

#ifndef _SVC_MEDIACORE_H
#define _SVC_MEDIACORE_H

#include "../../common/dispatch.h"
#include "services.h"
#include "../corecb.h"

class svc_mediaCore : public Dispatchable {
public:
  static int getServiceType() { return WaSvc::MEDIACORE; }

  void setCallback(CoreCallback *callback) { _voidcall(SETCALLBACK,callback); }

  void setNextFile(const char *file, const char *to=NULL, int uniqueid=0) { _voidcall(SETNEXTFILE,file,to,uniqueid); }

  void start(void) { _voidcall(START); }
  void pause(int pause) { _voidcall(PAUSE,pause); }
  void setPosition(int ms) { _voidcall(SETPOSITION,ms); }

  void setVolume(int volume) { _voidcall(SETVOLUME,volume); }
  void setPan(int pan) { _voidcall(SETPAN,pan); }

  void abortCurrentSong() { _voidcall(ABORTCURRENTSONG); }
  void stop(int suppress_callback=FALSE) { _voidcall(STOP,suppress_callback); }

  int  getPlaying(void) { return _call(GETPLAYING,0); }
  int  getPosition(void) { return _call(GETPOSITION,0); }
	int	 getWritePosition(void) { return _call(GETWRITEPOSITION,0); }

  int  getTitle(char *title, int maxlen) { return _call(GETTITLE,0,title,maxlen); }
  void getInfo(char *info, int maxlen) { _voidcall(GETINFO,info,maxlen); }
  int  getLength(void) { return _call(GETLENGTH,0); }

  int  getVolume(void) { return _call(GETVOLUME,0); }
  int  getPan(void) { return _call(GETPAN,0); }

  void setEQStatus(int enable) { _voidcall(SETEQSTATUS,enable); }
  int  getEQStatus() { return _call(GETEQSTATUS,0); }
  void setEQPreamp(char pre) { _voidcall(SETEQPREAMP,pre); }
  char getEQPreamp() { return _call(GETEQPREAMP,0); }
  void setEQBand(int band, char val) { _voidcall(SETEQBAND,band,val); }
  char getEQBand(int band) { return _call(GETEQBAND,0,band); }
  void setEQBands(char tab[10]) { _voidcall(SETEQBANDS,tab); }
  void getEQBands(char *tab) { _voidcall(GETEQBANDS,tab); }

  void setEQ(int enable, char pre, char tab[10]) { _voidcall(SETEQ,enable,pre,tab); }
  void getEQ(int *enable, char *pre, char *tab) { _voidcall(GETEQ,enable,pre,tab); }

  int  getMetaData(const char *name, char *data, int data_len) { return _call(GETMETADATA,0,name,data,data_len); }

  int  getVisData(void *dataptr, int sizedataptr) { return _call(GETVISDATA,0,dataptr,sizedataptr); }
  
  enum {
    SETCALLBACK=10,
    SETNEXTFILE=20,
    START=30,
    PAUSE=40,
    SETPOSITION=50,
    SETVOLUME=60,
    SETPAN=70,
    ABORTCURRENTSONG=80,
    STOP=90,
    GETPLAYING=100,
    GETPOSITION=110,
    GETWRITEPOSITION=120,
    GETTITLE=130,
    GETINFO=140,
    GETLENGTH=150,
    GETVOLUME=160,
    GETPAN=170,
    SETEQSTATUS=180,
    GETEQSTATUS=190,
    SETEQPREAMP=200,
    GETEQPREAMP=210,
    SETEQBAND=220,
    GETEQBAND=230,
    SETEQBANDS=240,
    GETEQBANDS=250,
    SETEQ=260,
    GETEQ=270,
    GETMETADATA=280,
    GETVISDATA=290,
  };
};

// derive from this one
class svc_mediaCoreI : public svc_mediaCore {
public:
  virtual void setCallback(CoreCallback *callback)=0;

  virtual void setNextFile(const char *file, const char *to=NULL, int uniqueid=0)=0; 
  /* call to specify next file from WCM_NEEDNEXTFILE, or before calling start() */

  virtual void start(void)=0; /* start playback */
  virtual void pause(int pause)=0; /* set/unset paused state (nonzero is paused, zero is unpaused) */
  virtual void setPosition(int ms)=0; /* set position of current stream in ms */

  virtual void setVolume(int volume)=0; /* volume is 0 to 255 */
  virtual void setPan(int pan)=0; /* pan is -127 to 127 */
  virtual void abortCurrentSong()=0; /* abort decoding of current song and start playing the next one */
  virtual void stop(int suppress_callback=FALSE)=0;

  virtual int  getPlaying(void)=0;    /* 0 is not playing, 1 is playing, -1 is paused */
  virtual int  getPosition(void)=0;   /* get position of current stream in ms */
	virtual int	 getWritePosition(void)=0;	/* get written position of current stream in ms */

  virtual int  getTitle(char *title, int maxlen)=0; // returns uniqueid
  virtual void getInfo(char *info, int maxlen)=0;
  virtual int  getLength(void)=0;     /* get length of track in ms. -1 indicates infinite/unknown */

  virtual int  getVolume(void)=0;     /* get volume (0-255) */
  virtual int  getPan(void)=0;        /* get panning (-127 to 127) */

  virtual void setEQStatus(int enable)=0;   /* 0 if off, 1 if on */
  virtual int  getEQStatus()=0;             /* 0 if off, 1 if on */
  virtual void setEQPreamp(char pre)=0;     /* -127 to 127 (-20db to +20db) */
  virtual char getEQPreamp()=0;             /* -127 to 127 (-20db to +20db) */
  virtual void setEQBand(int band, char val)=0; /* band=0-9 */
  virtual char getEQBand(int band)=0;           /* band=0-9 */
  virtual void setEQBands(char tab[10])=0; /* eq values are -127 to 127 (-20db to +20db) */
  virtual void getEQBands(char *tab)=0;    /* eq values are -127 to 127 (-20db to +20db) */

  virtual void setEQ(int enable, char pre, char tab[10])=0;
  virtual void getEQ(int *enable, char *pre, char *tab)=0;

  virtual int  getMetaData(const char *name, char *data, int data_len)=0; // returns size of data

  virtual int  getVisData(void *dataptr, int sizedataptr)=0; // returns size of data it wanted to copy, if any.

protected:
  RECVS_DISPATCH;
};

#endif
