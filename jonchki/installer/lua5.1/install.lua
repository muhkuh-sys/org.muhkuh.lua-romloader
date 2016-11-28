local t = ...

local strPlatform = 'Windows_x64'

if strPlatform=='Windows_x86' then
	t:install('windows_x86/lua_plugins/romloader.dll', '${install_lua_cpath}')
	t:install('windows_x86/lua_plugins/romloader_eth.dll', '${install_lua_cpath}')
	t:install('windows_x86/lua_plugins/romloader_jtag.dll', '${install_lua_cpath}')
	t:install('windows_x86/lua_plugins/romloader_papa_schlumpf.dll', '${install_lua_cpath}')
	t:install('windows_x86/lua_plugins/romloader_uart.dll', '${install_lua_cpath}')
	t:install('windows_x86/lua_plugins/romloader_usb.dll', '${install_lua_cpath}')

elseif strPlatform=='Windows_x64' then
	t:install('windows_x64/lua_plugins/romloader.dll', '${install_lua_cpath}')
	t:install('windows_x64/lua_plugins/romloader_eth.dll', '${install_lua_cpath}')
	t:install('windows_x64/lua_plugins/romloader_jtag.dll', '${install_lua_cpath}')
	t:install('windows_x64/lua_plugins/romloader_papa_schlumpf.dll', '${install_lua_cpath}')
	t:install('windows_x64/lua_plugins/romloader_uart.dll', '${install_lua_cpath}')
	t:install('windows_x64/lua_plugins/romloader_usb.dll', '${install_lua_cpath}')

end

return true
