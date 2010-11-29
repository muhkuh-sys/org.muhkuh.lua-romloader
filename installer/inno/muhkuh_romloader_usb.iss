
[Setup]
; This installation requires admin priveledges.  This is needed to install
; drivers on windows vista and later.
PrivilegesRequired = admin

; "ArchitecturesInstallIn64BitMode=x64 ia64" requests that the install
; be done in "64-bit mode" on x64 & Itanium, meaning it should use the
; native 64-bit Program Files directory and the 64-bit view of the
; registry. On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64 ia64


; Inno pascal functions for determining the processor type.
; you can use these to use (in an inno "check" parameter for example) to
; customize the installation depending on the architecture. 
[Code]
function IsX64: Boolean;
begin
  Result := Is64BitInstallMode and (ProcessorArchitecture = paX64);
end;

function IsI64: Boolean;
begin
  Result := Is64BitInstallMode and (ProcessorArchitecture = paIA64);
end;

function IsX86: Boolean;
begin
  Result := not IsX64 and not IsI64;
end;

function Is64: Boolean;
begin
  Result := IsX64 or IsI64;
end;


[Components]
Name: plugin_romloader_usb;     Description: "Romloader usb plugin";     Types: full


[Files]
; Copy some supporting dlls. TODO: handle bit, lua mhash and muhkuh in another iss file.
Source: "build\build\bit.dll";                                DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\lua.dll";                                DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\lua.exe";                                DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\mhash.dll";                              DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\muhkuh.dll";                             DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\muhkuh_plugin_interface.dll";            DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\romloader.dll";                          DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\romloader_usb.dll";                      DestDir: "{app}\application";          Components: plugin_romloader_usb

; Copy some demo scripts. TODO: handle tester in another iss file. 
Source: "build\build\test_romloader.lua";                     DestDir: "{app}\application";          Components: plugin_romloader_usb
Source: "build\build\tester.lua";                             DestDir: "{app}\application";          Components: plugin_romloader_usb

; Copy the plugin and the plugin descriptor.
;Source: "bin\bootloader_usb.dll";                            DestDir: "{app}\application\plugins";  Components: plugin_romloader_usb
;Source: "bin\bootloader_usb.xml";                            DestDir: "{app}\application\plugins";  Components: plugin_romloader_usb
;Source: "plugins\romloader\usb\win\driver\libusb0.dll";      DestDir: "{app}\application";          Components: plugin_romloader_usb

; Copy the driver to the application folder.
Source: "plugins\romloader\usb\win\driver\amd64\libusb0.dll"; DestDir: "{app}\driver\romloader_usb\amd64"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\amd64\libusb0.sys"; DestDir: "{app}\driver\romloader_usb\amd64"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\ia64\libusb0.dll";  DestDir: "{app}\driver\romloader_usb\ia64"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\ia64\libusb0.sys";  DestDir: "{app}\driver\romloader_usb\ia64"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\x86\libusb0_x86.dll";   DestDir: "{app}\driver\romloader_usb\x86"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\x86\libusb0.sys";   DestDir: "{app}\driver\romloader_usb\x86"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\netX_Romcode.cat";  DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\netX_Romcode.inf";  DestDir: "{app}\driver\romloader_usb"; Components: plugin_romloader_usb

; Copy the driver to the system folder.
Source: "plugins\romloader\usb\win\driver\x86\libusb0_x86.dll"; DestName: "libusb0.dll"; DestDir: "{sys}"; Flags: uninsneveruninstall replacesameversion restartreplace promptifolder; Check: IsX86; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\amd64\libusb0.dll"; DestDir: "{sys}"; Flags: uninsneveruninstall replacesameversion restartreplace promptifolder; Check: IsX64; Components: plugin_romloader_usb
Source: "plugins\romloader\usb\win\driver\ia64\libusb0.dll"; DestDir: "{sys}"; Flags: uninsneveruninstall replacesameversion restartreplace promptifolder; Check: IsI64; Components: plugin_romloader_usb


[Run]
Filename: "rundll32"; Parameters: "libusb0.dll,usb_install_driver_np_rundll {app}\driver\romloader_usb\netX_Romcode.inf"; StatusMsg: "Installing driver (this may take a few seconds) ..."
