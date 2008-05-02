; Muhkuh Inno Setup Cfg

#define AppName "Muhkuh"
#define AppVersion GetFileVersion("..\..\bin\muhkuh.exe")
#define AppVerName AppName+" "+AppVersion
#define InstallerName "muhkuh_"+AppVersion+"_setup"

#define SourceDir "..\.."

; include the common muhkuh settings
#include "muhkuh_common.iss"

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

