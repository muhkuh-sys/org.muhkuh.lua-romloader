
[Components]
Name: plugin_romloader_uart;    Description: "Romloader uart plugin";    Types: full


[Files]
Source: "bin\bootloader_uart.dll";                                DestDir: "{app}\application\plugins";   Components: plugin_romloader_uart
Source: "bin\bootloader_uart.xml";                                DestDir: "{app}\application\plugins";   Components: plugin_romloader_uart
Source: "plugins\romloader\uart\win\driver\netX_bootmonitor.inf"; DestDir: "{app}\driver\romloader_uart"; Components: plugin_romloader_uart
Source: "plugins\romloader\uart\win\driver\usbsernx.sys";         DestDir: "{app}\driver\romloader_uart"; Components: plugin_romloader_uart

