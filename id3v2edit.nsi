Name "ID3v2edit: universal ID3v2 editor"
OutFile id3v2edit_inst.exe
CRCCheck on
LicenseText "You must read the following license before installing."
LicenseData COPYING
ComponentText "This will install ID3v2edit on your computer."
InstType Normal
DirText "Please select a location to install ID3v2edit (or use the default)."
AutoCloseWindow true
SetOverwrite on
SetDateSave on

InstallDir $PROGRAMFILES\ID3v2edit\
#InstallDirRegKey HKEY_CURRENT_USER SOFTWARE\ID3v2edit ""

Section "ID3v2edit"
  SectionIn 1
  SetOutPath $INSTDIR
  File utils\id3v2edit\Release\id3v2edit.exe
SectionEnd

Section "Desktop Shortcut"
  SectionIn 1
  CreateShortCut "$DESKTOP\ID3v2edit.lnk" "$INSTDIR\id3v2edit.exe" "" "" 0
SectionEnd
