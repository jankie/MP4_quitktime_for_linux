# Microsoft Developer Studio Project File - Name="libfaad_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libfaad_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libfaad_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libfaad_dll.mak" CFG="libfaad_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libfaad_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libfaad_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libfaad_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libfaad_dll___Win32_Release"
# PROP BASE Intermediate_Dir "libfaad_dll___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBFAAD_DLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBFAAD_DLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386 /out:"ReleaseDLL/libfaad.dll"

!ELSEIF  "$(CFG)" == "libfaad_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libfaad_dll___Win32_Debug"
# PROP BASE Intermediate_Dir "libfaad_dll___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBFAAD_DLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBFAAD_DLL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"DebugDLL/libfaad.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "libfaad_dll - Win32 Release"
# Name "libfaad_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bits.c
# End Source File
# Begin Source File

SOURCE=.\block.c
# End Source File
# Begin Source File

SOURCE=.\config.c
# End Source File
# Begin Source File

SOURCE=.\decdata.c
# End Source File
# Begin Source File

SOURCE=.\decoder.c
# End Source File
# Begin Source File

SOURCE=.\dolby_adapt.c
# End Source File
# Begin Source File

SOURCE=.\fastfft.c
# End Source File
# Begin Source File

SOURCE=.\huffdec.c
# End Source File
# Begin Source File

SOURCE=.\huffinit.c
# End Source File
# Begin Source File

SOURCE=.\hufftables.c
# End Source File
# Begin Source File

SOURCE=.\intensity.c
# End Source File
# Begin Source File

SOURCE=.\monopred.c
# End Source File
# Begin Source File

SOURCE=.\nok_lt_prediction.c
# End Source File
# Begin Source File

SOURCE=.\pns.c
# End Source File
# Begin Source File

SOURCE=.\stereo.c
# End Source File
# Begin Source File

SOURCE=.\tns.c
# End Source File
# Begin Source File

SOURCE=.\transfo.c
# End Source File
# Begin Source File

SOURCE=.\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\All.h
# End Source File
# Begin Source File

SOURCE="E:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=.\bits.h
# End Source File
# Begin Source File

SOURCE=.\Block.h
# End Source File
# Begin Source File

SOURCE=.\Fastfft.h
# End Source File
# Begin Source File

SOURCE=.\interface.h
# End Source File
# Begin Source File

SOURCE=.\kbd_win.h
# End Source File
# Begin Source File

SOURCE=.\monopred.h
# End Source File
# Begin Source File

SOURCE=.\nok_lt_prediction.h
# End Source File
# Begin Source File

SOURCE=.\nok_ltp_common.h
# End Source File
# Begin Source File

SOURCE=.\nok_ltp_common_internal.h
# End Source File
# Begin Source File

SOURCE=.\Port.h
# End Source File
# Begin Source File

SOURCE=.\Tns.h
# End Source File
# Begin Source File

SOURCE=.\Transfo.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\libfaad.def
# End Source File
# End Target
# End Project
