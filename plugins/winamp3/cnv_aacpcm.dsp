# Microsoft Developer Studio Project File - Name="cnv_aacpcm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cnv_aacpcm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cnv_aacpcm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cnv_aacpcm.mak" CFG="cnv_aacpcm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cnv_aacpcm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cnv_aacpcm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cnv_aacpcm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "../../include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_MEAN_AND_LEAN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/cnv_aacpcm.wac"

!ELSEIF  "$(CFG)" == "cnv_aacpcm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /I "../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN32_MEAN_AND_LEAN" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/cnv_aacpcm.wac" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "cnv_aacpcm - Win32 Release"
# Name "cnv_aacpcm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "sdk source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sdk\studio\assert.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\attribute.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\attrint.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\blending.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\canvas.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\cfgitemi.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\corecb.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\depend.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\nsGUID.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\pathparse.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\playstring.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\ptrlist.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\region.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\servicei.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\std.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\common\string.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\svc_mediaconverter.cpp
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\waclient.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\aacpcm.cpp
# End Source File
# Begin Source File

SOURCE=.\cnv_aacpcm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "sdk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sdk\studio\api.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\apihelp.h
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\attrcb.h
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\attribute.h
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\attrint.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\basewnd.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\blending.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\canvas.h
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\cfgitem.h
# End Source File
# Begin Source File

SOURCE=.\sdk\attribs\cfgitemi.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\chunklist.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\common.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\compdb.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\corecb.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\depend.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\drag.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\fontdef.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\guid.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\map.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\mediainfo.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\multimap.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\named.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\nsGUID.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\pair.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\pathparse.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\platform\platform.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\playstring.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\ptrlist.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\region.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\rootcomp.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\rootwnd.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\scriptvar.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\service.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\servicei.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\services.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\stack.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\std.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\svc_fileread.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\services\svc_mediaconverter.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\tlist.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\vcputypes.h
# End Source File
# Begin Source File

SOURCE=.\sdk\common\virtualwnd.h
# End Source File
# Begin Source File

SOURCE=.\sdk\studio\wac.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\aacpcm.h
# End Source File
# Begin Source File

SOURCE="E:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=.\cnv_aacpcm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\faad.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
