#! /bin/bash
echo Generating wxWidgets Binding
lua -e"rulesFilename=\"bit_rules.lua\"" genwxbind.lua 
lua -e"rulesFilename=\"muhkuh_rules.lua\"" genwxbind.lua 
cd romloader
cd baka
cpp -o romloader_baka_lua.i -P -CC romloader_baka_lua.in
lua -e"rulesFilename=\"romloader_baka_rules.lua\"" ../../genwxbind.lua 
cd ..
cd usb
cpp -o romloader_usb_lua.i -P -CC romloader_usb_lua.in
lua -e"rulesFilename=\"romloader_usb_rules.lua\"" ../../genwxbind.lua 
cd ..
cd openocd
cpp -o romloader_openocd_lua.i -P -CC romloader_openocd_lua.in
lua -e"rulesFilename=\"romloader_openocd_rules.lua\"" ../../genwxbind.lua 
cd ..
cd ..
echo Done muhkuh wraps


