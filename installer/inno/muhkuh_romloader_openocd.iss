
[Components]
Name: plugin_romloader_openocd; Description: "Romloader openocd plugin"; Types: full


[Files]
Source: "bin\openocd.dll";                     DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
Source: "bin\openocd_netx50_jtagkey_win.xml";  DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
Source: "bin\openocd_netx500_jtagkey_win.xml"; DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd
Source: "bin\openocd_nxhx50_win.xml";          DestDir: "{app}\application\plugins"; Components: plugin_romloader_openocd

