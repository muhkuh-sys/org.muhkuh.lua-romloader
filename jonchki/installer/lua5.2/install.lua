local t = ...
local strDistId, strDistVersion, strCpuArch = t:get_platform()
local tResult

if strDistId=='windows' then
  if strCpuArch=='x86' then
    t:install('windows_x86/lua_plugins/',                       '${install_lua_cpath}/')
    tResult = true
  elseif strCpuArch=='x86_64' then
    t:install('windows_x86_64/lua_plugins/',                    '${install_lua_cpath}/')
    tResult = true
  end

elseif strDistId=='ubuntu' then
  if strDistVersion=='14.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1404_x86/lua_plugins/',                 '${install_lua_cpath}/')
      tResult = true
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1404_x86_64/lua_plugins/',              '${install_lua_cpath}/')
      tResult = true
    end

  elseif strDistVersion=='16.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1604_x86/lua_plugins/',                 '${install_lua_cpath}/')
      tResult = true
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1604_x86_64/lua_plugins/',              '${install_lua_cpath}/')
      tResult = true
    end

  elseif strDistVersion=='17.04' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1704_x86/lua_plugins/',                 '${install_lua_cpath}/')
      tResult = true
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1704_x86_64/lua_plugins/',              '${install_lua_cpath}/')
      tResult = true
    end

  elseif strDistVersion=='17.10' then
    if strCpuArch=='x86' then
      t:install('ubuntu_1710_x86/lua_plugins/',                 '${install_lua_cpath}/')
      tResult = true
    elseif strCpuArch=='x86_64' then
      t:install('ubuntu_1710_x86_64/lua_plugins/',              '${install_lua_cpath}/')
      tResult = true
    end
  end
end

return tResult
