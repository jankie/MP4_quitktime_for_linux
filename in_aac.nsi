Name "FAAD Winamp AAC plugin"
OutFile in_aac.exe
CRCCheck on
LicenseText "You must read the following license before installing."
LicenseData COPYING
ComponentText "This will install the FAAD Winamp AAC plugin on your computer."
InstType Normal
DirText "Please select a location to install the FAAD Winamp AAC plugin (or use the default)."
AutoCloseWindow true
SetOverwrite on
SetDateSave on

InstallDir $PROGRAMFILES\Winamp\Plugins\
InstallDirRegKey HKEY_CURRENT_USER SOFTWARE\Winamp\FAAD ""

Section "FAAD Winamp AAC plugin"
SectionIn 1
SetOutPath $INSTDIR
File winamp\Release\in_aac.dll
SectionEnd
