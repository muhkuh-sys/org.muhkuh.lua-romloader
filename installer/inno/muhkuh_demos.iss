
[Components]
Name: demos;                    Description: "Demos";                    Types: full


[Files]
Source: "plugins\romloader\_demos\memory_read_gui\read_write_gui.lua";       DestDir: "{app}\demos\memory_read_gui";     Components: demos
Source: "plugins\romloader\_demos\memory_read_gui\test_description.xml";     DestDir: "{app}\demos\memory_read_gui";     Components: demos
Source: "plugins\romloader\_demos\memory_read_simple\test_description.xml";  DestDir: "{app}\demos\memory_read_simple";  Components: demos
Source: "plugins\romloader\_demos\memory_read_tester\test_description.xml";  DestDir: "{app}\demos\memory_read_tester";  Components: demos
Source: "plugins\romloader\_test\test_description.xml";                      DestDir: "{app}\demos\romloader_test";      Components: demos
Source: "plugins\romloader\_test\netxtest_nx50.bin";                         DestDir: "{app}\demos\romloader_test";      Components: demos
Source: "plugins\romloader\_test\netxtest_nx500.bin";                        DestDir: "{app}\demos\romloader_test";      Components: demos
Source: "plugins\romloader\_demos\Muhkuh.cfg";                               DestDir: "{app}\application";               Components: demos

