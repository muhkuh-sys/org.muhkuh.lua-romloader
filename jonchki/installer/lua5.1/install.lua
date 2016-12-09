local t = ...
local strPlatform = t:get_platform()


if strPlatform=='windows_32' then
	t:install('windows_32/lua_plugins/romloader.dll',               '${install_lua_cpath}/')
	t:install('windows_32/lua_plugins/romloader_eth.dll',           '${install_lua_cpath}/')
	t:install('windows_32/lua_plugins/romloader_jtag.dll',          '${install_lua_cpath}/')
	t:install('windows_32/lua_plugins/romloader_papa_schlumpf.dll', '${install_lua_cpath}/')
	t:install('windows_32/lua_plugins/romloader_uart.dll',          '${install_lua_cpath}/')
	t:install('windows_32/lua_plugins/romloader_usb.dll',           '${install_lua_cpath}/')

elseif strPlatform=='windows_64' then
	t:install('windows_64/lua_plugins/romloader.dll',               '${install_lua_cpath}/')
	t:install('windows_64/lua_plugins/romloader_eth.dll',           '${install_lua_cpath}/')
	t:install('windows_64/lua_plugins/romloader_jtag.dll',          '${install_lua_cpath}/')
	t:install('windows_64/lua_plugins/romloader_papa_schlumpf.dll', '${install_lua_cpath}/')
	t:install('windows_64/lua_plugins/romloader_uart.dll',          '${install_lua_cpath}/')
	t:install('windows_64/lua_plugins/romloader_usb.dll',           '${install_lua_cpath}/')

end

return true
