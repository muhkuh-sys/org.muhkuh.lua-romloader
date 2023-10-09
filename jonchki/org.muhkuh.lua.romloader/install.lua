local t = ...

t:install('netx/', '${install_base}/netx/')
t:install('test_romloader.lua', '${install_base}/')
t:install('test_romloader_lua54.lua', '${install_base}/')

return true
