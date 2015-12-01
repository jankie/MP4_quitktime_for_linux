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

#ifndef _API_H
#define _API_H

#include "../common/dispatch.h"
#include "../common/platform/platform.h"	// basic type defs
#include "../common/vcputypes.h"

#define LATEST_API_VERSION	1

class CanvasBase;	// see ../common/canvas.h
class CfgItem;		// see ../attribs/cfgitem.h
class CoreCallback;	// see corecb.h
class DragInterface;	// see ../common/drag.h
class EmbedWnd;		// see ../common/embedwnd.h
class ItemSequencer;	// see ../common/sequence.h
class RootWnd;		// see ../common/rootwnd.h
class SkinFilter;	// see skinfilter.h
class SysCallback;	// see syscb.h
class XmlReaderCallback; // see xmlreader.h
class waService;	// see service.h
class MetricsCallback;	// see metricscb.h

typedef unsigned int CoreToken;

RootWnd *const MODALWND_NOWND = reinterpret_cast<RootWnd*>(-1);

class ComponentAPI : public Dispatchable {
public:
  int getVersion();	// the API version #

// studio.exe info
  // text string of version
  const char *main_getVersionString();
  // not-very-reliable auto-incremented build number... mostly just interesting
  unsigned int main_getBuildNumber();
  // the GUID of studio
  GUID main_getGUID();
  // the HINSTANCE (try not to abuse :)
  HINSTANCE main_gethInstance();
  // the icon
  HICON main_getIcon(int _small);


// config file stuff
  void setIntPrivate(const char *name, int val);
  int getIntPrivate(const char *name, int def_val);
  void setIntArrayPrivate(const char *name, const int *val, int nval);
  int getIntArrayPrivate(const char *name, int *val, int nval);
  void setStringPrivate(const char *name, const char *str);
  int getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str);


// playback core
  const char *core_getSupportedExtensions();//just the *.mp3 or whatever
  const char *core_getExtSupportedExtensions();// including names
// don't use these directly, use CoreHandle in common/corehandle.h
  CoreToken core_create(); // returns a token
  int core_free(CoreToken core);
  int core_setNextFile(CoreToken core, const char *playstring);
  int core_getStatus(CoreToken core);	// returns -1 if paused, 0 if stopped and 1 if playing
  const char *core_getCurrent(CoreToken core);
  int core_getPosition(CoreToken core);
  int core_setPosition(CoreToken core, int ms);
  int core_getLength(CoreToken core);
  // this method queries the core plugins directly, bypassing the db
  int core_getPluginData(const char *playstring, const char *name,
    char *data, int data_len, int data_type=0);	// returns size of data
  unsigned int core_getVolume(CoreToken core);
  void core_setVolume(CoreToken core, unsigned int vol);	// 0..255
  // register here for general callbacks in core status.
  void core_addCallback(CoreToken core, CoreCallback *cb);
  void core_delCallback(CoreToken core, CoreCallback *cb);
  // get visualization data, returns 0 if you should blank out
  int core_getVisData(CoreToken core, void *dataptr, int sizedataptr);
  int core_getLeftVuMeter(CoreToken core);
  int core_getRightVuMeter(CoreToken core);
  
  int core_registerSequencer(CoreToken core, ItemSequencer *seq);
  int core_deregisterSequencer(CoreToken core, ItemSequencer *seq);
  void core_userButton(CoreToken core, int button);	// see buttons.h

  int core_getEqStatus(CoreToken core); // returns 1 if on, 0 if off
  void core_setEqStatus(CoreToken core, int enable);  // 1=on, 0=off
  int core_getEqPreamp(CoreToken core);            // -127 to 127 (-20db to +20db)
  void core_setEqPreamp(CoreToken core, int pre);  // -127 to 127 (-20db to +20db)
  int core_getEqBand(CoreToken core, int band);            // band=0-9
  void core_setEqBand(CoreToken core, int band, int val);  // band=0-9,val=-127 to 127 (-20db to +20db)

  void core_registerExtension(const char *extensions, const char *extension_name);

// services
  int service_register(waService *);
  int service_deregister(waService *);
  int service_getNumServices(int svc_type);	// see common/svc_enum.h
  waService *service_enumService(int svc_type, int n);
  // service owner calls this when it issues a service *
  int service_lock(waService *owner, void *svcptr);
  // service client calls this when done w/ service *
  int service_release(void *svcptr);
  const char *service_getTypeName(int svc_type);

// sys callbacks
  int syscb_registerCallback(SysCallback *cb);
  int syscb_deregisterCallback(SysCallback *cb);

// component commands
  int cmdbar_createButton(char *label, char *command=NULL, int pos=0);
  int cmdbar_addCommand(int button, char *label, char *command);
  int cmd_sendCommand(GUID guid, char *command, int param1=0, int param2=0, void *ptr=NULL, int ptrlen=0);
  void cmd_broadcastCommand(char *command, int param1=0, int param2=0, void *ptr=NULL, int ptrlen=0);

  RECT compwnd_getClientRect();

  // component container status bar text
  void status_enable(int enabled);
  void status_setText(const char *text);
  void status_setOverlayText(const char *text, int interval);
  void status_setTrayTipText(const char *text);

  void main_enableInput(int enable);

  // language/locality support
  const char *locales_getTranslation(const char *str); // if not found, returns the str paramer
  void locales_addTranslation(const char *from, const char *to);
  const char *locales_getBindFromAction(int action);

  // formats milliseconds into time display based on local custom
  void timefmt_printMinSec(char *buf, int seconds);
  void timefmt_printTimeStamp(char *buf, int bufsize, int ts);


// configuration
  CfgItem *config_enumRootCfgItem(int n);

// message box
  int messageBox(const char *txt, const char *title, int flags, const char *not_anymore_identifier, RootWnd *parenwnt);
  RootWnd *getModalWnd();
  void pushModalWnd(RootWnd *w=MODALWND_NOWND);
  void popModalWnd(RootWnd *w=MODALWND_NOWND);

// size metrics
  void metrics_addCallback(MetricsCallback *mcb);
  void metrics_delCallback(MetricsCallback *mcb);
  int metrics_getDelta();
  int metrics_setDelta(int newdelta);

// open file dialog
// which: 0-> open new files    1-> open location   2-> add new files
  void main_displayOpenDialog(RootWnd *parentwnd, int which);
  void main_openFile(const char *filename);
  void main_navigateUrl(const char *url);

// database stuff
  int metadb_optimizePlayString(char *playstring);
  int metadb_addNewItem(const char *playstring, const char *initial_name=NULL);
  void metadb_sync();

  // called by class Playstring
  const char *metadb_dupPlaystring(const char *playstring);
  void metadb_releasePlaystring(const char *playstring);

// metadata assistance from metaDB
  void metadb_getInfo(const char *playstring, char *info, int maxlen);
  int metadb_getLength(const char *playstring);
  int metadb_getMetaData(const char *playstring, const char *name,
    char *data, int data_len, int data_type=0); // returns size of data
  int metadb_getMetaDataType(const char *name);
  int metadb_setMetaData(const char *playstring, const char *name,
    const char *data, int data_len, int data_type);

  // attempts to render data (from db or whatever) in given rect
  // if you use this you can show new datatypes your code doesn't understand
  int metadb_renderData(CanvasBase *cb, RECT &r, void *data, int datatype, int centered=FALSE);
  // attempts to convert a datatype to a text representation
  int metadb_convertToText(void *data, int datatype, char *buf, int buflen);

// item properties
  // builds the appropriate right-click menu for some item
  void contextMenu(void *thing, char *datatype, int broadcast=TRUE, LPARAM lparam=0);
  // application right-click context menu
  // returns the action value selected or -1 if nothing was selected
  int appContextMenu(RootWnd *parentwnd);
  // this version assumes you want a component callback
  void popup_addCommand(LPARAM lParam, const char *txt, int cmd, int checked=FALSE, int disabled=FALSE);

#if 1//REIMPLEMENT
// completeness graph
  void startCompleted(int max);
  void incCompleted();
  void endCompleted();

#endif
// skinning
  COLORREF skin_getElementColor(char *type);
  COLORREF *skin_getElementRef(char *type);
  int *skin_getIterator();	// an int that cycles on skin reloading

#define RenderBaseTexture renderBaseTexture //CUT

  void skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT &r, RootWnd *destWnd);
  void skin_registerBaseTextureWindow(RootWnd *window, const char *bmp=NULL);
  void skin_unregisterBaseTextureWindow(RootWnd *window);
  
  void skin_switchSkin(char *skin_name);
  const char *getSkinName();
  const char *getSkinPath();
  const char *getDefaultSkinPath();

// image loading
  int *imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h);
  int *imgldr_makeBmp(HINSTANCE hInst, int id, int *has_alpha, int *w, int *h);
  int *imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached);
  void imgldr_releaseSkinBitmap(int *bmpbits);

  void registerSkinFilter(SkinFilter *filter);
  void unregisterSkinFilter(SkinFilter *filter);
  void reapplySkinFilters();

  void paintset_render(int set, CanvasBase *c, RECT *r, int alpha=255);
  void paintset_renderTitle(char *t, CanvasBase *c, RECT *r, int alpha=255);

// animated rects
  void drawAnimatedRects(RECT *r1, RECT *r2);

// Dde com
  void sendDdeCommand(char *application, char *command, DWORD minimumDelay);

// XML Reader
  void xmlreader_registerCallback(char *matchstr, XmlReaderCallback *callback);
  void xmlreader_unregisterCallback(XmlReaderCallback *callback);
  void xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object=NULL);

// Window Tracker
  void wndTrackAdd(RootWnd *wnd);
  void wndTrackRemove(RootWnd *wnd);
  BOOL wndTrackDock(RootWnd *wnd, RECT *r, int mask);
  BOOL wndTrackDock(RootWnd *wnd, RECT *r, RECT *or, int mask);
  void wndTrackStartCooperative(RootWnd *wnd);
  void wndTrackEndCooperative(void);
  int wndTrackWasCooperative(void);

// Components
  int getNumComponents();
  GUID getComponentGUID(int c);
  const char *getComponentName(GUID componentGuid);
  const char *getComponentInfo(GUID componentGuid, int infoid);
  int getComponentMetrics(GUID g, int m);
  int getDisplayComponent(GUID g);
  void toggleComponent(GUID g);
  int isComponentVisible(GUID g);
  int setComponentVisible(GUID g, int visible, RECT *r=NULL);
  DragInterface *comp_getDragInterface(GUID g);
  int comp_renderElement(GUID guid, int element, CanvasBase *dest, RECT *r, int alpha);
  int comp_notifyScripts(const char *s, int i1, int i2);
  void register_autoPopupGUID(GUID g, const char *desc);

// db
  int db_enumComponentFields(GUID componentGuid, int fieldN, char *field_name, int field_name_max, int *field_type, int *indexed, int *unique_indexed);
  int db_getGUIDFromField(GUID g, GUID last, char *field_name, int data_type); // last=NULL to start from beginning, last=lastresult to get nt

  // basic db stuff
  int db_getPrivateData(const char *playstring, char *field_name, void *data, int data_len, int data_type);
  int db_setPrivateData(const char *playstring, char *field_name, void *data, int data_len, int data_type);
  int db_getPublicData(GUID owner, const char *playstring, char *field_name, void *data, int data_len, int data_type);
  int db_setPublicData(GUID owner, const char *playstring, char *field_name, void *data, int data_len, int data_type);
  int db_deletePrivateData(const char *playstring);
  int db_deletePublicData(GUID owner, const char *playstring);

  // advanced db stuff
  int db_createScanner(GUID owner, BOOL notifyUpdates);
  void db_deleteScanner(int scanner);
  void db_first(int scanner);
  void db_next(int scanner);
  void db_previous(int scanner);
  BOOL db_eof(int scanner);
  BOOL db_bof(int scanner);
  const char *db_getPlaystring(int scanner);
  int db_getData(int scanner, const char *field_name, void *data, int data_len, int data_type);
  int db_getNumRecords(int scanner);
  void db_gotoRecord(int scanner, int record_num);
  int db_getRecordNum(int scanner);
  BOOL db_locate(int scanner, const char *field_name, int from, const char *data, int data_len, int data_type);
  void db_push(int scanner);
  void db_pop(int scanner);
  int db_getConsistency(int scanner);
  float db_getFragmentationLevel(int scanner);
  BOOL db_isIndexCompromized(int scanner);
  void db_ackNewIndex(int scanner);
  void db_setWorkingIndex(int scanner, char *field_name);
  int db_dropIndex(int scanner, char *field_name);
  int db_createUniqueScanner(int scanner, const char *field_name, BOOL notify_updates);
  int db_runQuery(int scanner, const char *query);
  void db_removeFilters(int scanner);

  // allows a rootWndFromPoint that won't get other instances RootWnds
  // also, this will find VirtualWnds while the WIN32 version won't
  // and... this is portable :)
  RootWnd *rootWndFromPoint(POINT *pt);
  void registerRootWnd(RootWnd *wnd);
  void unregisterRootWnd(RootWnd *wnd);
  int rootwndIsValid(RootWnd *wnd);

  //keyboard support
  int forwardOnChar(char c);
  int forwardOnKeyDown(int k);
  int forwardOnKeyUp(int k);
  int forwardOnKillFocus();

  // file readers support
  void *fileOpen(const char *filename, const char *mode);
  void fileClose(void *fileHandle);
  int fileRead(void *buffer, int size, void *fileHandle);
  int fileWrite(const void *buffer, int size, void *fileHandle);
  int fileSeek(int offset, int origin, void *fileHandle);
  int fileTell(void *fileHandle);
  int fileGetFileSize(void *fileHandle);
  int fileExists(const char *filename);
  int fileRemove(const char *filename);
  
// text output functions. don't call these directly, use the Canvas stuff
  void font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt);
  int font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h);

// call this after deallocating a lot of RAM
  void hint_garbageCollect();
// outputs a message to the console, severity 0 to 9 (critical, before terminating operations)
  void console_outputString(int severity, const char *string);

// load a script on top of skin
  void xml_loadSkinFile(const char *xmlfile);

  void maki_pushObject(void *o);
  void maki_pushInt(int v);
  void maki_pushBoolean(int b);
  void maki_pushFloat(float f);
  void maki_pushDouble(double d);
  void maki_pushString(const char *s);
  void maki_pushVoid();
  void *maki_popObject();
  int maki_popInt();
  int maki_popBoolean();
  float maki_popFloat();
  double maki_popDouble();
  const char *maki_popString();
  void maki_popDiscard();
  scriptVar maki_triggerEvent(ScriptObject *o, int dlfid, int np, int scriptid=-1);
  int maki_getScriptInt(scriptVar v);
  int maki_getScriptBoolean(scriptVar v);
  float maki_getScriptFloat(scriptVar v);
  double maki_getScriptDouble(scriptVar v);
  const char *maki_getScriptString(scriptVar v);
  void *maki_getScriptObject(scriptVar v);
  int vcpu_getCacheCount();
  int vcpu_isValidScriptId(int id);
  int vcpu_mapVarId(int varid, int scriptid);
  int vcpu_getUserAncestorId(int varid, int scriptid);
  int vcpu_getNumEvents();
  int vcpu_getEvent(int event, int *dlf, int *script, int *var);

// groups creation / destruction

  RootWnd *group_create(const char *groupid);
  RootWnd *group_create_cfg(const char *groupid, CfgItem *cfgitem, const char *attributename);
  int group_destroy(RootWnd *group);



#define hintGarbageCollect hint_garbageCollect	//FUCKO CUT

  // Dispatchable codes
  enum {
    GETVERSION		= 100,
    GETVERSIONSTRING	= 101,
    GETBUILDNUMBER	= 102,
    GETGUID		= 103,
    GETHINSTANCE	= 104,
    GETICON		= 105,
    SETINTPRIVATE	= 200,
    GETINTPRIVATE	= 201,
    SETINTARRAYPRIVATE	= 202,
    GETINTARRAYPRIVATE	= 203,
    SETSTRINGPRIVATE	= 204,
    GETSTRINGPRIVATE	= 205,

    COREGETSUPPORTEDEXTENSIONS		= 300,
    COREGETEXTSUPPORTEDEXTENSIONS	= 301,
    CORECREATE				= 302,
    COREFREE				= 303,
    CORESETNEXTFILE			= 306,
    COREGETSTATUS			= 307,
    COREGETCURRENT			= 308,
    COREGETPOSITION			= 309,
    CORESETPOSITION			= 310,
    COREGETLENGTH			= 311,
    COREGETPLUGINDATA			= 312,
    COREGETVOLUME			= 313,
    CORESETVOLUME			= 314,
    COREADDCALLBACK			= 315,
    COREDELCALLBACK			= 316,
    COREGETVISDATA			= 317,
    COREGETLEFTVUMETER			= 318,
    COREGETRIGHTVUMETER			= 319,
    COREREGISTERSEQUENCER		= 320,
    COREDEREGISTERSEQUENCER		= 321,
    COREUSERBUTTON			= 322,
    COREGETEQSTATUS			= 323,
    CORESETEQSTATUS			= 324,
    COREGETEQPREAMP			= 325,
    CORESETEQPREAMP			= 326,
    COREGETEQBAND			= 327,
    CORESETEQBAND			= 328,
    COREREGISTEREXTENSION = 329,

    SERVICEREGISTER			= 400,
    SERVICEDEREGISTER			= 401,
    SERVICEGETNUMSERVICES		= 402,
    SERVICEENUMSERVICE			= 403,
    SERVICELOCK				= 404,
    SERVICERELEASE			= 405,
    SERVICEGETTYPENAME			= 406,

    GARBAGECOLLECT	= 600,

    OUTPUTSTRING	= 800,

    LOADXMLSKINFILE	= 1000,

    PUSHOBJECT = 1100,
    PUSHINT = 1101,
    PUSHBOOLEAN = 1102,
    PUSHFLOAT = 1103,
    PUSHDOUBLE = 1104,
    PUSHSTRING = 1105,
    PUSHVOID = 1106,

    POPOBJECT = 1120,
    POPINT = 1121,
    POPBOOLEAN = 1122,
    POPFLOAT = 1123,
    POPDOUBLE = 1124,
    POPSTRING = 1125,
    POPDISCARD = 1126,

    TRIGGEREVENT = 1140,

    GETSCRIPTOBJECT = 1160,
    GETSCRIPTINT = 1161,
    GETSCRIPTBOOLEAN = 1162,
    GETSCRIPTFLOAT = 1163,
    GETSCRIPTDOUBLE = 1164,
    GETSCRIPTSTRING = 1165,

    GETCACHECOUNT = 1166,
    ISVALIDSCRIPTID = 1167,
    MAPVARID = 1168,
    GETUSERANCESTORID = 1169,
    GETNUMEVENTS = 1170,
    GETEVENT = 1171,

    GROUPCREATE = 1200,
    GROUPDESTROY = 1201,
    GROUPCREATECFG = 1202,
    REGAUTOPOPUPGUID = 1300,

// All in one big fat load:

    SYSCB_REGISTERCALLBACK,
    SYSCB_DEREGISTERCALLBACK,
    CMDBAR_CREATEBUTTON,
    CMDBAR_ADDCOMMAND,
    CMD_SENDCOMMAND,
    CMD_BROADCASTCOMMAND,
    COMPWND_GETCLIENTRECT,
    STATUS_ENABLE,
    STATUS_SETTEXT,
    STATUS_SETOVERLAYTEXT,
    STATUS_SETTRAYTIPTEXT,
    MAIN_ENABLEINPUT,
    LOCALES_GETTRANSLATION,
    LOCALES_ADDTRANSLATION,
    LOCALES_GETBINDFROMACTION,
    TIMEFMT_PRINTMINSEC,
    TIMEFMT_PRINTTIMESTAMP,
    CONFIG_ENUMROOTCFGITEM,
    MESSAGEBOX,
    GETMODALWND,
    PUSHMODALWND,
    POPMODALWND,
    METRICS_ADDCALLBACK,
    METRICS_DELCALLBACK,
    METRICS_GETDELTA,
    METRICS_SETDELTA,
    MAIN_DISPLAYOPENDIALOG,
    MAIN_OPENFILE,
    MAIN_NAVIGATEURL,
    METADB_OPTIMIZEPLAYSTRING,
    METADB_ADDNEWITEM,
    METADB_SYNC,
    METADB_DUPPLAYSTRING,
    METADB_RELEASEPLAYSTRING,
    METADB_GETINFO,
    METADB_GETLENGTH,
    METADB_GETMETADATA,
    METADB_GETMETADATATYPE,
    METADB_SETMETADATA,
    METADB_RENDERDATA,
    METADB_CONVERTTOTEXT,
    CONTEXTMENU,
    APPCONTEXTMENU,
    POPUP_ADDCOMMAND,
    STARTCOMPLETED,
    INCCOMPLETED,    
    ENDCOMPLETED,    
    SKIN_GETELEMENTCOLOR,
    SKIN_GETELEMENTREF,
    SKIN_GETITERATOR,
    SKIN_RENDERBASETEXTURE,
    SKIN_REGISTERBASETEXTUREWINDOW,
    SKIN_UNREGISTERBASETEXTUREWINDOW,
    SKIN_SWITCHSKIN,
    GETSKINNAME,
    GETSKINPATH,
    GETDEFAULTSKINPATH,
    IMGLDR_MAKEBMP,
    IMGLDR_MAKEBMP2,
    IMGLDR_REQUESTSKINBITMAP,
    IMGLDR_RELEASESKINBITMAP,
    REGISTERSKINFILTER,
    UNREGISTERSKINFILTER,
    REAPPLYSKINFILTERS,
    PAINTSET_RENDER,
    PAINTSET_RENDERTITLE,
    DRAWANIMATEDRECTS,
    SENDDDECOMMAND,
    XMLREADER_REGISTERCALLBACK,
    XMLREADER_UNREGISTERCALLBACK,
    XMLREADER_LOADFILE,
    WNDTRACKADD,
    WNDTRACKREMOVE,
    WNDTRACKDOCK,
    WNDTRACKDOCK2,
    WNDTRACKSTARTCOOPERATIVE,
    WNDTRACKENDCOOPERATIVE,
    WNDTRACKWASCOOPERATIVE,
    GETNUMCOMPONENTS,
    GETCOMPONENTGUID,
    GETCOMPONENTNAME,
    GETCOMPONENTINFO,
    GETCOMPONENTMETRICS,
    GETDISPLAYCOMPONENT,
    TOGGLECOMPONENT,
    ISCOMPONENTVISIBLE,
    SETCOMPONENTVISIBLE,
    COMP_GETDRAGINTERFACE,
    COMP_RENDERELEMENT,
    COMP_NOTIFYSCRIPTS,
    DB_ENUMCOMPONENTFIELDS,
    DB_GETGUIDFROMFIELD,
    DB_GETPRIVATEDATA,
    DB_SETPRIVATEDATA,
    DB_GETPUBLICDATA,
    DB_SETPUBLICDATA,
    DB_DELETEPRIVATEDATA,
    DB_DELETEPUBLICDATA,
    DB_CREATESCANNER,
    DB_DELETESCANNER,
    DB_FIRST,
    DB_NEXT,
    DB_PREVIOUS,
    DB_EOF,
    DB_BOF,
    DB_GETPLAYSTRING,
    DB_GETDATA,
    DB_GETNUMRECORDS,
    DB_GOTORECORD,
    DB_GETRECORDNUM,
    DB_LOCATE,
    DB_PUSH,
    DB_POP,
    DB_GETCONSISTENCY,
    DB_GETFRAGMENTATIONLEVEL,
    DB_ISINDEXCOMPROMIZED,
    DB_ACKNEWINDEX,
    DB_SETWORKINGINDEX,
    DB_DROPINDEX2,
    DB_CREATEUNIQUESCANNER,
    DB_RUNQUERY,
    DB_REMOVEFILTERS,
    ROOTWNDFROMPOINT,
    REGISTERROOTWND,
    UNREGISTERROOTWND,
    ROOTWNDISVALID,
    FORWARDONCHAR,
    FORWARDONKEYDOWN,
    FORWARDONKEYUP,
    FORWARDONKILLFOCUS,
    FILEOPEN,
    FILECLOSE,
    FILEREAD,
    FILEWRITE,
    FILESEEK,
    FILETELL,
    FILEGETFILESIZE,
    FILEEXISTS,
    FILEREMOVE,
    FONT_TEXTOUT,
    FONT_GETINFO,
    HINT_GARBAGECOLLECT,
    CONSOLE_OUTPUTSTRING,
    XML_LOADSKINFILE,
    //
    END_OF_DISPATCH
  };
};

// these are a bunch of helper implementations of the above methods
#include "apihelp.h"

extern ComponentAPI *api;

#endif
/*

 syscb_registerCallback
 syscb_deregisterCallback
 cmdbar_createButton
 cmdbar_addCommand
 cmd_sendCommand
v cmd_broadcastCommand
 compwnd_getClientRect
v status_enable
v status_setText
v status_setOverlayText
v status_setTrayTipText
v main_enableInput
 locales_getTranslation
v locales_addTranslation
 locales_getBindFromAction
v timefmt_printMinSec
v timefmt_printTimeStamp
 config_enumRootCfgItem
 messageBox
 getModalWnd
v pushModalWnd
v popModalWnd
v metrics_addCallback
v metrics_delCallback
 metrics_getDelta
 metrics_setDelta
v main_displayOpenDialog
v main_openFile
v main_navigateUrl
 metadb_optimizePlayString
 metadb_addNewItem
v metadb_sync
 metadb_dupPlaystring
v metadb_releasePlaystring
v metadb_getInfo
 metadb_getLength
 metadb_getMetaData
 metadb_getMetaDataType
 metadb_setMetaData
 metadb_renderData
 metadb_convertToText
v contextMenu
 appContextMenu
v popup_addCommand
v startCompleted
v incCompleted
v endCompleted
 skin_getElementColor
 skin_getElementRef
 skin_getIterator
v skin_renderBaseTexture
v skin_registerBaseTextureWindow
v skin_unregisterBaseTextureWindow
v skin_switchSkin
 getSkinName
 getSkinPath
 getDefaultSkinPath
 imgldr_makeBmp
 imgldr_makeBmp
 imgldr_requestSkinBitmap
v imgldr_releaseSkinBitmap
v registerSkinFilter
v unregisterSkinFilter
v reapplySkinFilters
v paintset_render
v paintset_renderTitle
v drawAnimatedRects
v sendDdeCommand
v xmlreader_registerCallback
v xmlreader_unregisterCallback
v xmlreader_loadFile
v wndTrackAdd
v wndTrackRemove
 wndTrackDock
 wndTrackDock
v wndTrackStartCooperative
v wndTrackEndCooperative
 wndTrackWasCooperative
 getNumComponents
 getComponentGUID
 getComponentName
 getComponentInfo
 getComponentMetrics
 getDisplayComponent
v toggleComponent
 isComponentVisible
 setComponentVisible
 comp_getDragInterface
 comp_renderElement
 comp_notifyScripts
 db_enumComponentFields
 db_getGUIDFromField
 db_getPrivateData
 db_setPrivateData
 db_getPublicData
 db_setPublicData
 db_deletePrivateData
 db_deletePublicData
 db_createScanner
v db_deleteScanner
v db_first
v db_next
v db_previous
 db_eof
 db_bof
 db_getPlaystring
 db_getData
 db_getNumRecords
v db_gotoRecord
 db_getRecordNum
 db_locate
v db_push
v db_pop
 db_getConsistency
 db_getFragmentationLevel
 db_isIndexCompromized
v db_ackNewIndex
v db_setWorkingIndex
 db_dropIndex
 db_createUniqueScanner
 db_runQuery
v db_removeFilters
 rootWndFromPoint
v registerRootWnd
v unregisterRootWnd
 rootwndIsValid
 forwardOnChar
 forwardOnKeyDown
 forwardOnKeyUp
 forwardOnKillFocus
 fileOpen
v fileClose
 fileRead
 fileWrite
 fileSeek
 fileTell
 fileGetFileSize
 fileExists
 fileRemove
v font_textOut
 font_getInfo
v hint_garbageCollect
v console_outputString
v xml_loadSkinFile

*/