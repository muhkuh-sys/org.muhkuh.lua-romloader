local t = ...
local strDistId, strDistVersion, strCpuArch = t:get_platform()

if strDistId=='windows' then
  if strCpuArch=='x86' then
    t:install('windows_x86/lua_plugins/romloader.dll',                       '${install_lua_cpath}/')
    t:install('windows_x86/lua_plugins/romloader_eth.dll',                   '${install_lua_cpath}/')
    t:install('windows_x86/lua_plugins/romloader_jtag.dll',                  '${install_lua_cpath}/')
    t:install('windows_x86/lua_plugins/romloader_papa_schlumpf.dll',         '${install_lua_cpath}/')
    t:install('windows_x86/lua_plugins/romloader_uart.dll',                  '${install_lua_cpath}/')
    t:install('windows_x86/lua_plugins/romloader_usb.dll',                   '${install_lua_cpath}/')
  elseif strCpuArch=='x86_64' then
    t:install('windows_x86_64/lua_plugins/romloader.dll',                    '${install_lua_cpath}/')
    t:install('windows_x86_64/lua_plugins/romloader_eth.dll',                '${install_lua_cpath}/')
    t:install('windows_x86_64/lua_plugins/romloader_jtag.dll',               '${install_lua_cpath}/')
    t:install('windows_x86_64/lua_plugins/romloader_papa_schlumpf.dll',      '${install_lua_cpath}/')
    t:install('windows_x86_64/lua_plugins/romloader_uart.dll',               '${install_lua_cpath}/')
    t:install('windows_x86_64/lua_plugins/romloader_usb.dll',                '${install_lua_cpath}/')
  end

elseif strDistId=='ubuntu' then
  if strDistVersion=='12.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1204_x86/lua_plugins/romloader.so',                  '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86/lua_plugins/romloader_eth.so',              '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86/lua_plugins/romloader_jtag.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86/lua_plugins/romloader_papa_schlumpf.so',    '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86/lua_plugins/romloader_uart.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86/lua_plugins/romloader_usb.so',              '${install_lua_cpath}/')
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader.so',               '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader_eth.so',           '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader_jtag.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader_papa_schlumpf.so', '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader_uart.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1204_x86_64/lua_plugins/romloader_usb.so',           '${install_lua_cpath}/')
    end

  elseif strDistVersion=='14.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1404_x86/lua_plugins/romloader.so',                  '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86/lua_plugins/romloader_eth.so',              '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86/lua_plugins/romloader_jtag.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86/lua_plugins/romloader_papa_schlumpf.so',    '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86/lua_plugins/romloader_uart.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86/lua_plugins/romloader_usb.so',              '${install_lua_cpath}/')
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader.so',               '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader_eth.so',           '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader_jtag.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader_papa_schlumpf.so', '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader_uart.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1404_x86_64/lua_plugins/romloader_usb.so',           '${install_lua_cpath}/')
    end

  elseif strDistVersion=='16.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1604_x86/lua_plugins/romloader.so',                  '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86/lua_plugins/romloader_eth.so',              '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86/lua_plugins/romloader_jtag.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86/lua_plugins/romloader_papa_schlumpf.so',    '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86/lua_plugins/romloader_uart.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86/lua_plugins/romloader_usb.so',              '${install_lua_cpath}/')
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader.so',               '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader_eth.so',           '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader_jtag.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader_papa_schlumpf.so', '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader_uart.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1604_x86_64/lua_plugins/romloader_usb.so',           '${install_lua_cpath}/')
    end

  elseif strDistVersion=='16.10' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1610_x86/lua_plugins/romloader.so',                  '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86/lua_plugins/romloader_eth.so',              '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86/lua_plugins/romloader_jtag.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86/lua_plugins/romloader_papa_schlumpf.so',    '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86/lua_plugins/romloader_uart.so',             '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86/lua_plugins/romloader_usb.so',              '${install_lua_cpath}/')
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader.so',               '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader_eth.so',           '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader_jtag.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader_papa_schlumpf.so', '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader_uart.so',          '${install_lua_cpath}/')
      t:install('ubuntu_1610_x86_64/lua_plugins/romloader_usb.so',           '${install_lua_cpath}/')
    end
  end
end

return true
