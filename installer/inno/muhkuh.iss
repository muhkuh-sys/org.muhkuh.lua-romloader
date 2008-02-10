; Muhkuh Inno Setup Cfg

[Setup]
AppName=Muhkuh
AppVersion="1.0.0"
AppVerName="Muhkuh 1.0.0"
OutputBaseFilename=muhkuh_1.0.0_setup
VersionInfoTextVersion="1.0.0"
VersionInfoVersion="1.0.0"

DefaultDirName={pf}\Muhkuh
DefaultGroupName=Muhkuh
Compression=lzma/max
SolidCompression=yes

OutputDir=.
SourceDir=..\..

AllowNoIcons=yes
;ChangesAssociations=yes
LicenseFile=docs\gpl-2.0.txt

; misc info
AppPublisher=Muhkuh team
AppPublisherURL=http://muhkuh.sourceforge.net/
AppSupportURL=http://www.sourceforge.net/projects/muhkuh
AppUpdatesURL=http://www.sourceforge.net/projects/muhkuh
VersionInfoCopyright=Copyright (C) 2008, Muhkuh team
AppCopyright=Copyright (C) 2008, Muhkuh team

; icon stuff
UninstallDisplayIcon={src}\muhkuh.ico
SetupIconFile=application\muhkuh.ico
;WizardImageFile=application\muhkuh.ico


[Types]
Name: "full"; Description: "Full installation"
Name: "compact"; Description: "Only install the base system without plugins"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: muhkuh; Description: "The muhkuh base application"; Types: full compact custom; Flags: fixed
Name: plugin_romloader_baka; Description: "The romloader baka plugin"; Types: full
Name: plugin_romloader_usb; Description: "The romloader usb plugin"; Types: full
Name: plugin_romloader_openocd; Description: "The romloader openocd plugin"; Types: full
Name: lua_scripts; Description: "The lua scripts"; Types: full compact

[Tasks]
Name: startmenu; Description: "Create icons in Start menu"; GroupDescription: "Additional icons:"; Components: muhkuh
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Components: muhkuh
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Components: muhkuh; Flags: unchecked

[Files]
Source: "bin\muhkuh.exe"; DestDir: "{app}"; Components: muhkuh
Source: "bin\muhkuh_tips.txt"; DestDir: "{app}"; Components: muhkuh
; the wxwidgets dlls
Source: "bin\wxbase28_net_vc.dll" ; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxbase28_odbc_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxbase28_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxbase28_xml_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_adv_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_aui_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_core_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_dbgrid_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_gl_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_html_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_media_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_qa_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_richtext_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_stc_vc_custom.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_vc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxmsw28_xrc_vc.dll"; DestDir: "{app}"; Components: muhkuh
; the wxLua dlls
Source: "bin\lua5.1.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindadv.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindaui.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindbase.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindcore.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindgl.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindhtml.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindmedia.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindnet.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindstc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindxml.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindxrc.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxlua.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxluadebug.dll"; DestDir: "{app}"; Components: muhkuh
Source: "bin\wxlua_msw28_wxluasocket.dll"; DestDir: "{app}"; Components: muhkuh

Source: "bin\baka.dll"; DestDir: "{app}"; Components: plugin_romloader_baka
Source: "bin\baka.xml"; DestDir: "{app}"; Components: plugin_romloader_baka

Source: "bin\bootloader_usb.dll"; DestDir: "{app}"; Components: plugin_romloader_usb
Source: "bin\bootloader_usb.xml"; DestDir: "{app}"; Components: plugin_romloader_usb

Source: "bin\openocd.dll"; DestDir: "{app}"; Components: plugin_romloader_openocd
Source: "bin\openocd_netx500_jtagkey_win.xml"; DestDir: "{app}"; Components: plugin_romloader_openocd
Source: "bin\openocd_nxhx50_win.xml"; DestDir: "{app}"; Components: plugin_romloader_openocd

Source: "bin\lua\mmio.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\muhkuh_system.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\secmem.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\select_plugin.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\serialnr.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\tester.lua"; DestDir: "{app}\lua"; Components: lua_scripts
Source: "bin\lua\utils.lua"; DestDir: "{app}\lua"; Components: lua_scripts


[Icons]
Name: {group}\Muhkuh; Filename: {app}\muhkuh.exe; WorkingDir: {app}; Components: muhkuh; Tasks: startmenu
Name: {group}\Uninstall; Filename: {uninstallexe}; Components: muhkuh; Tasks: startmenu

Name: {userdesktop}\Muhkuh; Filename: {app}\muhkuh.exe; WorkingDir: {app}; Components: muhkuh; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Muhkuh; Filename: {app}\muhkuh.exe; Tasks: quicklaunchicon


[Messages]
BeveledLabel=Muhkuh


