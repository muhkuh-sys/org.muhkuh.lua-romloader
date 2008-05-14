; Muhkuh Inno Setup Cfg

#define AppName "Muhkuh"
#define AppVersion GetFileVersion("..\..\bin\muhkuh.exe")
#define AppVerName AppName+" "+AppVersion
#define InstallerName "muhkuh_"+AppVersion+"_setup"

#define SourceDir "..\.."
#define OutputDir "."

; include the common muhkuh settings
#include "muhkuh_common.iss"


[Setup]
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}

; misc info
AppPublisher=Muhkuh team
AppPublisherURL=http://muhkuh.sourceforge.net/
AppSupportURL=http://www.sourceforge.net/projects/muhkuh
AppUpdatesURL=http://www.sourceforge.net/projects/muhkuh
VersionInfoCopyright=Copyright (C) 2008, Muhkuh team
AppCopyright=Copyright (C) 2008, Muhkuh team

InfoBeforeFile=changelog.txt

; icon stuff
SetupIconFile=icons\custom\muhkuh.ico

[Messages]
BeveledLabel=Muhkuh


; include the components
#include "muhkuh_app.iss"
#include "muhkuh_romloader_baka.iss"
#include "muhkuh_romloader_uart.iss"
#include "muhkuh_romloader_usb.iss"
#include "muhkuh_romloader_openocd.iss"
#include "lua_scripts.iss"
#include "muhkuh_demos.iss"

; create muhkuh icons
#include "muhkuh_icons.iss"

