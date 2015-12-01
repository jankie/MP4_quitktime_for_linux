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

#ifndef _APIHELP_H
#define _APIHELP_H

inline int ComponentAPI::getVersion() {
  return _call(GETVERSION, 0);
}

inline const char *ComponentAPI::main_getVersionString() {
  return _call(GETVERSIONSTRING, "");
}

inline unsigned int ComponentAPI::main_getBuildNumber() {
  return _call(GETBUILDNUMBER, 0);
}

inline GUID ComponentAPI::main_getGUID() {
  return _call(GETGUID, INVALID_GUID);
}

inline HINSTANCE ComponentAPI::main_gethInstance() {
  return _call(GETHINSTANCE, (HINSTANCE)0);
}

inline HICON ComponentAPI::main_getIcon(int _small) {
  return _call(GETICON, (HICON)0, _small);
}

inline void ComponentAPI::setIntPrivate(const char *name, int val) {
  _voidcall(SETINTPRIVATE, name, val);
}

inline int ComponentAPI::getIntPrivate(const char *name, int def_val) {
  return _call(GETINTPRIVATE, def_val, name, def_val);	// hee hee
}

inline void ComponentAPI::setIntArrayPrivate(const char *name, const int *val, int nval) {
  _voidcall(SETINTARRAYPRIVATE, name, val, nval);
}

inline int ComponentAPI::getIntArrayPrivate(const char *name, int *val, int nval) {
  return _call(GETINTARRAYPRIVATE, 0, name, val, nval);
}

inline void ComponentAPI::setStringPrivate(const char *name, const char *str) {
  _voidcall(SETSTRINGPRIVATE, name, str, 0);
}

inline int ComponentAPI::getStringPrivate(const char *name, char *buf, int buf_len, const char *default_str) {
  return _call(GETSTRINGPRIVATE, 0, name, buf, buf_len, default_str);
}


inline
const char *ComponentAPI::core_getSupportedExtensions() {
  return _call(COREGETSUPPORTEDEXTENSIONS, (const char *)0);
}

inline
const char *ComponentAPI::core_getExtSupportedExtensions() {
  return _call(COREGETEXTSUPPORTEDEXTENSIONS, (const char *)0);
}

inline CoreToken ComponentAPI::core_create() {
  return _call(CORECREATE, 0);
}

inline
int ComponentAPI::core_free(CoreToken core) {
  return _call(COREFREE, 0);
}

inline int ComponentAPI::core_setNextFile(CoreToken core, const char *playstring) {
  return _call(CORESETNEXTFILE, 0, core, playstring);
}

inline int ComponentAPI::core_getStatus(CoreToken core) {
  return _call(COREGETSTATUS, 0, core);
}

inline const char *ComponentAPI::core_getCurrent(CoreToken core) {
  return _call(COREGETCURRENT, (const char*)NULL, core);
}

inline int ComponentAPI::core_getPosition(CoreToken core) {
  return _call(COREGETPOSITION, -1, core);
}

inline int ComponentAPI::core_setPosition(CoreToken core, int ms) {
  return _call(CORESETPOSITION, 0, core, ms);
}

inline int ComponentAPI::core_getLength(CoreToken core) {
  return _call(COREGETLENGTH, 0, core);
}

inline int ComponentAPI::core_getPluginData(const char *playstring,
  const char *name, char *data, int data_len, int data_type) {
  return _call(COREGETPLUGINDATA, 0, playstring, name, data, data_len, data_type);
}

inline unsigned int ComponentAPI::core_getVolume(CoreToken core) {
  return _call(COREGETVOLUME, 0, core);
}

inline void ComponentAPI::core_setVolume(CoreToken core, unsigned int vol) {
  _voidcall(CORESETVOLUME, core, vol);
}

inline void ComponentAPI::core_addCallback(CoreToken core, CoreCallback *cb) {
  _voidcall(COREADDCALLBACK, core, cb);
}

inline void ComponentAPI::core_delCallback(CoreToken core, CoreCallback *cb) {
  _voidcall(COREDELCALLBACK, core, cb);
}

inline int ComponentAPI::core_getVisData(CoreToken core, void *dataptr, int sizedataptr) {
  return _call(COREGETVISDATA, 0, core, dataptr, sizedataptr);
}

inline int ComponentAPI::core_getLeftVuMeter(CoreToken core) {
  return _call(COREGETLEFTVUMETER, 0, core);
}

inline int ComponentAPI::core_getRightVuMeter(CoreToken core) {
  return _call(COREGETRIGHTVUMETER, 0, core);
}

inline int ComponentAPI::core_registerSequencer(CoreToken core, ItemSequencer *seq) {
  return _call(COREREGISTERSEQUENCER, 0, core, seq);
}

inline int ComponentAPI::core_deregisterSequencer(CoreToken core, ItemSequencer *seq) {
  return _call(COREDEREGISTERSEQUENCER, 0, core, seq);
}

inline void ComponentAPI::core_userButton(CoreToken core, int button) {
  _voidcall(COREUSERBUTTON, core, button);
}

inline int ComponentAPI::core_getEqStatus(CoreToken core) {
  return _call(COREGETEQSTATUS, 0, core);
}

inline void ComponentAPI::core_setEqStatus(CoreToken core, int enable) {
  _voidcall(CORESETEQSTATUS, core, enable);
}

inline int ComponentAPI::core_getEqPreamp(CoreToken core) {
  return _call(COREGETEQPREAMP, 0, core);
}

inline void ComponentAPI::core_setEqPreamp(CoreToken core, int pre) {
  _voidcall(CORESETEQPREAMP, core, pre);
}

inline int ComponentAPI::core_getEqBand(CoreToken core, int band) {
  return _call(COREGETEQBAND, 0, core, band);
}

inline void ComponentAPI::core_setEqBand(CoreToken core, int band, int val) {
  _voidcall(CORESETEQBAND, core, band, val);
}

inline void ComponentAPI::core_registerExtension(const char *extensions, const char *extension_name) {
  _voidcall(COREREGISTEREXTENSION, extensions, extension_name);
}

inline int ComponentAPI::service_register(waService *svc) {
  return _call(SERVICEREGISTER, 0, svc);
}

inline int ComponentAPI::service_deregister(waService *svc) {
  return _call(SERVICEDEREGISTER, 0, svc);
}

inline void ComponentAPI::hint_garbageCollect() {
  _voidcall(GARBAGECOLLECT);
}

inline int ComponentAPI::service_getNumServices(int svc_type) {
  return _call(SERVICEGETNUMSERVICES, 0, svc_type);
}

inline waService *ComponentAPI::service_enumService(int svc_type, int n) {
  return _call(SERVICEENUMSERVICE, (waService*)0, svc_type, n);
}

inline int ComponentAPI::service_lock(waService *owner, void *svcptr) {
  return _call(SERVICELOCK, 0, owner, svcptr);
}

inline int ComponentAPI::service_release(void *svcptr) {
  return _call(SERVICERELEASE, 0, svcptr);
}

inline const char *ComponentAPI::service_getTypeName(int svc_type) {
  return _call(SERVICEGETTYPENAME, "", svc_type);
}

inline void ComponentAPI::maki_pushObject(void *o) {
  _voidcall(PUSHOBJECT, o);
}

inline void ComponentAPI::maki_pushInt(int i) {
  _voidcall(PUSHINT, i);
}

inline void ComponentAPI::maki_pushBoolean(int b) {
  _voidcall(PUSHBOOLEAN, b);
}

inline void ComponentAPI::maki_pushFloat(float f) {
  _voidcall(PUSHFLOAT, f);
}

inline void ComponentAPI::maki_pushDouble(double d) {
  _voidcall(PUSHDOUBLE, d);
}

inline void ComponentAPI::maki_pushString(const char *s) {
  _voidcall(PUSHSTRING, s);
}

inline void ComponentAPI::maki_pushVoid() {
  _voidcall(PUSHVOID);
}

inline void *ComponentAPI::maki_popObject() {
  return _call(POPOBJECT, (void *)0);
}                       

inline int ComponentAPI::maki_popInt() {
  return _call(POPINT, 0);
}

inline int ComponentAPI::maki_popBoolean() {
  return _call(POPBOOLEAN, 0);
}

inline float ComponentAPI::maki_popFloat() {
  return _call(POPFLOAT, (float)0);
}

inline double ComponentAPI::maki_popDouble() {
  return _call(POPDOUBLE, (double)0);
}

inline const char *ComponentAPI::maki_popString() {
  return _call(POPSTRING, (const char *)NULL);
}

inline void ComponentAPI::maki_popDiscard() {
  _voidcall(POPDISCARD);
}

inline scriptVar ComponentAPI::maki_triggerEvent(ScriptObject *o, int dlfid, int np, int scriptid) {
  return _call(TRIGGEREVENT, scriptVar(), o, dlfid, np, scriptid);
}

inline int ComponentAPI::maki_getScriptInt(scriptVar v) {
  return _call(GETSCRIPTINT, 0, v);
}

inline int ComponentAPI::maki_getScriptBoolean(scriptVar v) {
  return _call(GETSCRIPTBOOLEAN, 0, v);
}

inline float ComponentAPI::maki_getScriptFloat(scriptVar v) {
  return _call(GETSCRIPTFLOAT, (float)0, v);
}

inline double ComponentAPI::maki_getScriptDouble(scriptVar v) {
  return _call(GETSCRIPTDOUBLE, (double)0, v);
}

inline const char *ComponentAPI::maki_getScriptString(scriptVar v) {
  return _call(GETSCRIPTSTRING, (const char *)0, v);
}

inline void *ComponentAPI::maki_getScriptObject(scriptVar v) {
  return _call(GETSCRIPTOBJECT, (void *)0, v);
}

inline int ComponentAPI::vcpu_getCacheCount() {  return _call(GETCACHECOUNT, 0);
}

inline int ComponentAPI::vcpu_isValidScriptId(int id) {
  return _call(ISVALIDSCRIPTID, 0, id);
}

inline int ComponentAPI::vcpu_mapVarId(int varid, int scriptid) {
  return _call(MAPVARID, 0, varid, scriptid);
}

inline int ComponentAPI::vcpu_getUserAncestorId(int varid, int scriptid) {
  return _call(GETUSERANCESTORID, 0, varid, scriptid);
}

inline int ComponentAPI::vcpu_getNumEvents() {
  return _call(GETNUMEVENTS, 0);
}

inline int ComponentAPI::vcpu_getEvent(int event, int *dlf, int *script, int *var) {
  return _call(GETEVENT, 0, event, dlf, script, var);
}

inline RootWnd *ComponentAPI::group_create(const char *groupid) {
  return _call(GROUPCREATE, (RootWnd *)0, groupid);
}

inline int ComponentAPI::group_destroy(RootWnd *group) {
  return _call(GROUPDESTROY, 0, group);
}

inline RootWnd *ComponentAPI::group_create_cfg(const char *groupid, CfgItem *cfgitem, const char *attributename) {
  return _call(GROUPCREATECFG, (RootWnd *)0, groupid, cfgitem, attributename);
}

inline void ComponentAPI::register_autoPopupGUID(GUID g, const char *desc) {
  _voidcall(REGAUTOPOPUPGUID, g, desc);
}

//
// mig spit out the following:
//

inline int ComponentAPI::syscb_registerCallback(SysCallback *cb) {
  return _call( SYSCB_REGISTERCALLBACK, (int)0, cb );
}
inline int ComponentAPI::syscb_deregisterCallback(SysCallback *cb) {
  return _call( SYSCB_DEREGISTERCALLBACK, (int)0, cb );
}
inline int ComponentAPI::cmdbar_createButton(char *label, char *command, int pos) {
  return _call( CMDBAR_CREATEBUTTON, (int)0, label, command, pos);
}
inline int ComponentAPI::cmdbar_addCommand(int button, char *label, char *command) {
  return _call( CMDBAR_ADDCOMMAND, (int)0, button, label, command );
}
inline int ComponentAPI::cmd_sendCommand(GUID guid, char *command, int param1, int param2, void *ptr, int ptrlen) {
  return _call( CMD_SENDCOMMAND, (int)0, guid, command, param1, param2, ptr, ptrlen );
}
inline void ComponentAPI::cmd_broadcastCommand(char *command, int param1, int param2, void *ptr, int ptrlen) {
  _voidcall( CMD_BROADCASTCOMMAND, command, param1, param2, ptr, ptrlen );
}
inline RECT ComponentAPI::compwnd_getClientRect() {
  return _call( COMPWND_GETCLIENTRECT, RECT() );
}
inline void ComponentAPI::status_enable(int enabled) {
  _voidcall( STATUS_ENABLE, enabled );
}
inline void ComponentAPI::status_setText(const char *text) {
  _voidcall( STATUS_SETTEXT, text );
}
inline void ComponentAPI::status_setOverlayText(const char *text, int interval) {
  _voidcall( STATUS_SETOVERLAYTEXT, text, interval );
}
inline void ComponentAPI::status_setTrayTipText(const char *text) {
  _voidcall( STATUS_SETTRAYTIPTEXT, text );
}
inline void ComponentAPI::main_enableInput(int enable) {
  _voidcall( MAIN_ENABLEINPUT, enable );
}
inline const char *ComponentAPI::locales_getTranslation(const char *str) {
  return _call( LOCALES_GETTRANSLATION, "", str );
}
inline void ComponentAPI::locales_addTranslation(const char *from, const char *to) {
  _voidcall( LOCALES_ADDTRANSLATION, from, to );
}
inline const char *ComponentAPI::locales_getBindFromAction(int action) {
  return _call( LOCALES_GETBINDFROMACTION, "", action );
}
inline void ComponentAPI::timefmt_printMinSec(char *buf, int seconds) {
  _voidcall( TIMEFMT_PRINTMINSEC, buf, seconds );
}
inline void ComponentAPI::timefmt_printTimeStamp(char *buf, int bufsize, int ts) {
  _voidcall( TIMEFMT_PRINTTIMESTAMP, buf, bufsize, ts );
}
inline CfgItem *ComponentAPI::config_enumRootCfgItem(int n) {
  return _call( CONFIG_ENUMROOTCFGITEM, (CfgItem *)0, n );
}
inline int ComponentAPI::messageBox(const char *txt, const char *title, int flags, const char *not_anymore_identifier, RootWnd *parenwnt) {
  return _call( MESSAGEBOX, (int)0, txt, title, flags, not_anymore_identifier, parenwnt );
}
inline RootWnd *ComponentAPI::getModalWnd() {
  return _call( GETMODALWND, (RootWnd *)0  );
}
inline void ComponentAPI::pushModalWnd(RootWnd *w) {
  _voidcall( PUSHMODALWND, w);
}
inline void ComponentAPI::popModalWnd(RootWnd *w) {
  _voidcall( POPMODALWND, w);
}
inline void ComponentAPI::metrics_addCallback(MetricsCallback *mcb) {
  _voidcall( METRICS_ADDCALLBACK, mcb );
}
inline void ComponentAPI::metrics_delCallback(MetricsCallback *mcb) {
  _voidcall( METRICS_DELCALLBACK, mcb );
}
inline int ComponentAPI::metrics_getDelta() {
  return _call( METRICS_GETDELTA, (int)0  );
}
inline int ComponentAPI::metrics_setDelta(int newdelta) {
  return _call( METRICS_SETDELTA, (int)0, newdelta );
}
inline void ComponentAPI::main_displayOpenDialog(RootWnd *parentwnd, int which) {
  _voidcall( MAIN_DISPLAYOPENDIALOG, parentwnd, which );
}
inline void ComponentAPI::main_openFile(const char *filename) {
  _voidcall( MAIN_OPENFILE, filename );
}
inline void ComponentAPI::main_navigateUrl(const char *url) {
  _voidcall( MAIN_NAVIGATEURL, url );
}
inline int ComponentAPI::metadb_optimizePlayString(char *playstring) {
  return _call( METADB_OPTIMIZEPLAYSTRING, (int)0, playstring );
}
inline int ComponentAPI::metadb_addNewItem(const char *playstring, const char *initial_name) {
  return _call( METADB_ADDNEWITEM, (int)0, playstring, initial_name);
}
inline void ComponentAPI::metadb_sync() {
  _voidcall( METADB_SYNC  );
}
inline const char *ComponentAPI::metadb_dupPlaystring(const char *playstring) {
  return _call( METADB_DUPPLAYSTRING, "", playstring );
}
inline void ComponentAPI::metadb_releasePlaystring(const char *playstring) {
  _voidcall( METADB_RELEASEPLAYSTRING, playstring );
}
inline void ComponentAPI::metadb_getInfo(const char *playstring, char *info, int maxlen) {
  _voidcall( METADB_GETINFO, playstring, info, maxlen );
}
inline int ComponentAPI::metadb_getLength(const char *playstring) {
  return _call( METADB_GETLENGTH, (int)0, playstring );
}
inline int ComponentAPI::metadb_getMetaData(const char *playstring, const char *name, char *data, int data_len, int data_type ) {
  return _call( METADB_GETMETADATA, (int)0, playstring, name, data, data_len, data_type  );
}
inline int ComponentAPI::metadb_getMetaDataType(const char *name) {
  return _call( METADB_GETMETADATATYPE, (int)0, name );
}
inline int ComponentAPI::metadb_setMetaData(const char *playstring, const char *name, const char *data, int data_len, int data_type ) {
  return _call( METADB_SETMETADATA, (int)0, playstring, name, data, data_len, data_type  );
}
inline int ComponentAPI::metadb_renderData(CanvasBase *cb, RECT &r, void *data, int datatype, int centered) {
  return _call( METADB_RENDERDATA, (int)0, cb, &r, data, datatype, centered);
}
inline int ComponentAPI::metadb_convertToText(void *data, int datatype, char *buf, int buflen) {
  return _call( METADB_CONVERTTOTEXT, (int)0, data, datatype, buf, buflen );
}
inline void ComponentAPI::contextMenu(void *thing, char *datatype, int broadcast, LPARAM lparam) {
  _voidcall( CONTEXTMENU, thing, datatype, broadcast, lparam);
}
inline int ComponentAPI::appContextMenu(RootWnd *parentwnd) {
  return _call( APPCONTEXTMENU, (int)0, parentwnd );
}
inline void ComponentAPI::popup_addCommand(LPARAM lParam, const char *txt, int cmd, int checked, int disabled) {
  _voidcall( POPUP_ADDCOMMAND, lParam, txt, cmd, checked, disabled);
}
inline void ComponentAPI::startCompleted(int max) {
  _voidcall( STARTCOMPLETED, max );
}
inline void ComponentAPI::incCompleted() {
  _voidcall( INCCOMPLETED );
}
inline void ComponentAPI::endCompleted() {
  _voidcall( ENDCOMPLETED );
}
inline COLORREF ComponentAPI::skin_getElementColor(char *type) {
  return _call( SKIN_GETELEMENTCOLOR, (COLORREF)0, type );
}
inline COLORREF *ComponentAPI::skin_getElementRef(char *type) {
  return _call( SKIN_GETELEMENTREF, (COLORREF *)0, type );
}
inline int *ComponentAPI::skin_getIterator() {
  return _call( SKIN_GETITERATOR, (int *)0);
}
inline void ComponentAPI::skin_renderBaseTexture(RootWnd *base, CanvasBase *c, RECT &r, RootWnd *destWnd) {
  _voidcall( SKIN_RENDERBASETEXTURE, base, c, &r, destWnd );
}
inline void ComponentAPI::skin_registerBaseTextureWindow(RootWnd *window, const char *bmp) {
  _voidcall( SKIN_REGISTERBASETEXTUREWINDOW, window, bmp);
}
inline void ComponentAPI::skin_unregisterBaseTextureWindow(RootWnd *window) {
  _voidcall( SKIN_UNREGISTERBASETEXTUREWINDOW, window );
}
inline void ComponentAPI::skin_switchSkin(char *skin_name) {
  _voidcall( SKIN_SWITCHSKIN, skin_name );
}
inline const char *ComponentAPI::getSkinName() {
  return _call( GETSKINNAME, ""  );
}
inline const char *ComponentAPI::getSkinPath() {
  return _call( GETSKINPATH, ""  );
}
inline const char *ComponentAPI::getDefaultSkinPath() {
  return _call( GETDEFAULTSKINPATH, ""  );
}
inline int *ComponentAPI::imgldr_makeBmp(const char *filename, int *has_alpha, int *w, int *h) {
  return _call( IMGLDR_MAKEBMP, (int *)0, filename, has_alpha, w, h );
}
inline int *ComponentAPI::imgldr_makeBmp(HINSTANCE hInst, int id, int *has_alpha, int *w, int *h) {
  return _call( IMGLDR_MAKEBMP2, (int *)0, hInst, id, has_alpha, w, h );
}
inline int *ComponentAPI::imgldr_requestSkinBitmap(const char *file, int *has_alpha, int *x, int *y, int *subw, int *subh, int *w, int *h, int cached) {
  return _call( IMGLDR_REQUESTSKINBITMAP, (int *)0, file, has_alpha, x, y, subw, subh, w, h, cached );
}
inline void ComponentAPI::imgldr_releaseSkinBitmap(int *bmpbits) {
  _voidcall( IMGLDR_RELEASESKINBITMAP, bmpbits );
}
inline void ComponentAPI::registerSkinFilter(SkinFilter *filter) {
  _voidcall( REGISTERSKINFILTER, filter );
}
inline void ComponentAPI::unregisterSkinFilter(SkinFilter *filter) {
  _voidcall( UNREGISTERSKINFILTER, filter );
}
inline void ComponentAPI::reapplySkinFilters() {
  _voidcall( REAPPLYSKINFILTERS  );
}
inline void ComponentAPI::paintset_render(int set, CanvasBase *c, RECT *r, int alpha) {
  _voidcall( PAINTSET_RENDER, set, c,  r, alpha);
}
inline void ComponentAPI::paintset_renderTitle(char *t, CanvasBase *c, RECT *r, int alpha) {
  _voidcall( PAINTSET_RENDERTITLE, t, c,  r, alpha);
}
inline void ComponentAPI::drawAnimatedRects(RECT *r1, RECT *r2) {
  _voidcall( DRAWANIMATEDRECTS,  r1,  r2 );
}
inline void ComponentAPI::sendDdeCommand(char *application, char *command, DWORD minimumDelay) {
  _voidcall( SENDDDECOMMAND, application, command, minimumDelay );
}
inline void ComponentAPI::xmlreader_registerCallback(char *matchstr, XmlReaderCallback *callback) {
  _voidcall( XMLREADER_REGISTERCALLBACK, matchstr, callback );
}
inline void ComponentAPI::xmlreader_unregisterCallback(XmlReaderCallback *callback) {
  _voidcall( XMLREADER_UNREGISTERCALLBACK, callback );
}
inline void ComponentAPI::xmlreader_loadFile(const char *filename, XmlReaderCallback *only_this_object) {
  _voidcall( XMLREADER_LOADFILE, filename, only_this_object);
}
inline void ComponentAPI::wndTrackAdd(RootWnd *wnd) {
  _voidcall( WNDTRACKADD, wnd );
}
inline void ComponentAPI::wndTrackRemove(RootWnd *wnd) {
  _voidcall( WNDTRACKREMOVE, wnd );
}
inline BOOL ComponentAPI::wndTrackDock(RootWnd *wnd, RECT *r, int mask) {
  return _call( WNDTRACKDOCK, (BOOL)0, wnd,  r, mask );
}
inline BOOL ComponentAPI::wndTrackDock(RootWnd *wnd, RECT *r, RECT *or, int mask) {
  return _call( WNDTRACKDOCK2, (BOOL)0, wnd,  r,  or, mask );
}
inline void ComponentAPI::wndTrackStartCooperative(RootWnd * wnd) {
  _voidcall( WNDTRACKSTARTCOOPERATIVE, wnd );
}
inline void ComponentAPI::wndTrackEndCooperative() {
  _voidcall( WNDTRACKENDCOOPERATIVE  );
}
inline int ComponentAPI::wndTrackWasCooperative() {
  return _call( WNDTRACKWASCOOPERATIVE, (int)0  );
}
inline int ComponentAPI::getNumComponents() {
  return _call( GETNUMCOMPONENTS, (int)0  );
}
inline GUID ComponentAPI::getComponentGUID(int c) {
  return _call( GETCOMPONENTGUID, GUID(), c );
}
inline const char *ComponentAPI::getComponentName(GUID componentGuid) {
  return _call( GETCOMPONENTNAME, "", componentGuid );
}
inline const char *ComponentAPI::getComponentInfo(GUID componentGuid, int infoid) {
  return _call( GETCOMPONENTINFO, "", componentGuid, infoid );
}
inline int ComponentAPI::getComponentMetrics(GUID g, int m) {
  return _call( GETCOMPONENTMETRICS, (int)0, g, m );
}
inline int ComponentAPI::getDisplayComponent(GUID g) {
  return _call( GETDISPLAYCOMPONENT, (int)0, g );
}
inline void ComponentAPI::toggleComponent(GUID g) {
  _voidcall( TOGGLECOMPONENT, g );
}
inline int ComponentAPI::isComponentVisible(GUID g) {
  return _call( ISCOMPONENTVISIBLE, (int)0, g );
}
inline int ComponentAPI::setComponentVisible(GUID g, int visible, RECT *r) {
  return _call( SETCOMPONENTVISIBLE, (int)0, g, visible,  r);
}
inline DragInterface *ComponentAPI::comp_getDragInterface(GUID g) {
  return _call( COMP_GETDRAGINTERFACE, (DragInterface *)0, g );
}
inline int ComponentAPI::comp_renderElement(GUID guid, int element, CanvasBase *dest, RECT *r, int alpha) {
  return _call( COMP_RENDERELEMENT, (int)0, guid, element, dest,  r, alpha );
}
inline int ComponentAPI::comp_notifyScripts(const char *s, int i1, int i2) {
  return _call( COMP_NOTIFYSCRIPTS, (int)0, s, i1, i2 );
}
inline int ComponentAPI::db_enumComponentFields(GUID componentGuid, int fieldN, char *field_name, int field_name_max, int *field_type, int *indexed, int *unique_indexed) {
  return _call( DB_ENUMCOMPONENTFIELDS, (int)0, componentGuid, fieldN, field_name, field_name_max, field_type, indexed, unique_indexed );
}
inline int ComponentAPI::db_getGUIDFromField(GUID g, GUID last, char *field_name, int data_type) {
  return _call( DB_GETGUIDFROMFIELD, (int)0, g, last, field_name, data_type);
}
inline int ComponentAPI::db_getPrivateData(const char *playstring, char *field_name, void *data, int data_len, int data_type) {
  return _call( DB_GETPRIVATEDATA, (int)0, playstring, field_name, data, data_len, data_type );
}
inline int ComponentAPI::db_setPrivateData(const char *playstring, char *field_name, void *data, int data_len, int data_type) {
  return _call( DB_SETPRIVATEDATA, (int)0, playstring, field_name, data, data_len, data_type );
}
inline int ComponentAPI::db_getPublicData(GUID owner, const char *playstring, char *field_name, void *data, int data_len, int data_type) {
  return _call( DB_GETPUBLICDATA, (int)0, owner, playstring, field_name, data, data_len, data_type );
}
inline int ComponentAPI::db_setPublicData(GUID owner, const char *playstring, char *field_name, void *data, int data_len, int data_type) {
  return _call( DB_SETPUBLICDATA, (int)0, owner, playstring, field_name, data, data_len, data_type );
}
inline int ComponentAPI::db_deletePrivateData(const char *playstring) {
  return _call( DB_DELETEPRIVATEDATA, (int)0, playstring );
}
inline int ComponentAPI::db_deletePublicData(GUID owner, const char *playstring) {
  return _call( DB_DELETEPUBLICDATA, (int)0, owner, playstring );
}
inline int ComponentAPI::db_createScanner(GUID owner, BOOL notifyUpdates) {
  return _call( DB_CREATESCANNER, (int)0, owner, notifyUpdates );
}
inline void ComponentAPI::db_deleteScanner(int scanner) {
  _voidcall( DB_DELETESCANNER, scanner );
}
inline void ComponentAPI::db_first(int scanner) {
  _voidcall( DB_FIRST, scanner );
}
inline void ComponentAPI::db_next(int scanner) {
  _voidcall( DB_NEXT, scanner );
}
inline void ComponentAPI::db_previous(int scanner) {
  _voidcall( DB_PREVIOUS, scanner );
}
inline BOOL ComponentAPI::db_eof(int scanner) {
  return _call( DB_EOF, (BOOL)0, scanner );
}
inline BOOL ComponentAPI::db_bof(int scanner) {
  return _call( DB_BOF, (BOOL)0, scanner );
}
inline const char *ComponentAPI::db_getPlaystring(int scanner) {
  return _call( DB_GETPLAYSTRING, "", scanner );
}
inline int ComponentAPI::db_getData(int scanner, const char *field_name, void *data, int data_len, int data_type) {
  return _call( DB_GETDATA, (int)0, scanner, field_name, data, data_len, data_type );
}
inline int ComponentAPI::db_getNumRecords(int scanner) {
  return _call( DB_GETNUMRECORDS, (int)0, scanner );
}
inline void ComponentAPI::db_gotoRecord(int scanner, int record_num) {
  _voidcall( DB_GOTORECORD, scanner, record_num );
}
inline int ComponentAPI::db_getRecordNum(int scanner) {
  return _call( DB_GETRECORDNUM, (int)0, scanner );
}
inline BOOL ComponentAPI::db_locate(int scanner, const char *field_name, int from, const char *data, int data_len, int data_type) {
  return _call( DB_LOCATE, (BOOL)0, scanner, field_name, from, data, data_len, data_type );
}
inline void ComponentAPI::db_push(int scanner) {
  _voidcall( DB_PUSH, scanner );
}
inline void ComponentAPI::db_pop(int scanner) {
  _voidcall( DB_POP, scanner );
}
inline int ComponentAPI::db_getConsistency(int scanner) {
  return _call( DB_GETCONSISTENCY, (int)0, scanner );
}
inline float ComponentAPI::db_getFragmentationLevel(int scanner) {
  return _call( DB_GETFRAGMENTATIONLEVEL, (float)0, scanner );
}
inline BOOL ComponentAPI::db_isIndexCompromized(int scanner) {
  return _call( DB_ISINDEXCOMPROMIZED, (BOOL)0, scanner );
}
inline void ComponentAPI::db_ackNewIndex(int scanner) {
  _voidcall( DB_ACKNEWINDEX, scanner );
}
inline void ComponentAPI::db_setWorkingIndex(int scanner, char *field_name) {
  _voidcall( DB_SETWORKINGINDEX, scanner, field_name );
}
inline int ComponentAPI::db_dropIndex(int scanner, char *field_name) {
  return _call( DB_DROPINDEX2, (int)0, scanner, field_name );
}
inline int ComponentAPI::db_createUniqueScanner(int scanner, const char *field_name, BOOL notify_updates) {
  return _call( DB_CREATEUNIQUESCANNER, (int)0, scanner, field_name, notify_updates );
}
inline int ComponentAPI::db_runQuery(int scanner, const char *query) {
  return _call( DB_RUNQUERY, (int)0, scanner, query );
}
inline void ComponentAPI::db_removeFilters(int scanner) {
  _voidcall( DB_REMOVEFILTERS, scanner );
}
inline RootWnd *ComponentAPI::rootWndFromPoint(POINT *pt) {
  return _call( ROOTWNDFROMPOINT, (RootWnd *)0, pt );
}
inline void ComponentAPI::registerRootWnd(RootWnd *wnd) {
  _voidcall( REGISTERROOTWND, wnd );
}
inline void ComponentAPI::unregisterRootWnd(RootWnd *wnd) {
  _voidcall( UNREGISTERROOTWND, wnd );
}
inline int ComponentAPI::rootwndIsValid(RootWnd *wnd) {
  return _call( ROOTWNDISVALID, (int)0, wnd );
}
inline int ComponentAPI::forwardOnChar(char c) {
  return _call( FORWARDONCHAR, (int)0, c );
}
inline int ComponentAPI::forwardOnKeyDown(int k) {
  return _call( FORWARDONKEYDOWN, (int)0, k );
}
inline int ComponentAPI::forwardOnKeyUp(int k) {
  return _call( FORWARDONKEYUP, (int)0, k );
}
inline int ComponentAPI::forwardOnKillFocus() {
  return _call( FORWARDONKILLFOCUS, (int)0  );
}
inline void *ComponentAPI::fileOpen(const char *filename, const char *mode) {
  return _call( FILEOPEN, (void *)0, filename, mode );
}
inline void ComponentAPI::fileClose(void *fileHandle) {
  _voidcall( FILECLOSE, fileHandle );
}
inline int ComponentAPI::fileRead(void *buffer, int size, void *fileHandle) {
  return _call( FILEREAD, (int)0, buffer, size, fileHandle );
}
inline int ComponentAPI::fileWrite(const void *buffer, int size, void *fileHandle) {
  return _call( FILEWRITE, (int)0, buffer, size, fileHandle );
}
inline int ComponentAPI::fileSeek(int offset, int origin, void *fileHandle) {
  return _call( FILESEEK, (int)0, offset, origin, fileHandle );
}
inline int ComponentAPI::fileTell(void *fileHandle) {
  return _call( FILETELL, (int)0, fileHandle );
}
inline int ComponentAPI::fileGetFileSize(void *fileHandle) {
  return _call( FILEGETFILESIZE, (int)0, fileHandle );
}
inline int ComponentAPI::fileExists(const char *filename) {
  return _call( FILEEXISTS, (int)0, filename );
}
inline int ComponentAPI::fileRemove(const char *filename) {
  return _call( FILEREMOVE, (int)0, filename );
}
inline void ComponentAPI::font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt) {
  _voidcall( FONT_TEXTOUT, c, style, x, y, w, h, txt );
}
inline int ComponentAPI::font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h) {
  return _call( FONT_GETINFO, (int)0, c, font, infoid, txt, w, h );
}
/*
inline void ComponentAPI::hint_garbageCollect() {
  _voidcall( HINT_GARBAGECOLLECT  );
}
*/
inline void ComponentAPI::console_outputString(int severity, const char *string) {
  _voidcall( CONSOLE_OUTPUTSTRING, severity, string );
}
inline void ComponentAPI::xml_loadSkinFile(const char *xmlfile) {
  _voidcall( XML_LOADSKINFILE, xmlfile );
}
//
//  ... and then passed out.
//

#endif
