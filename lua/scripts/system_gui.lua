aLuaModules = {
	"bit",
	"muhkuh",
	"muhkuh_system",
	"select_plugin_gui",
	"wx"
}

aLuaPlugins = {
	"romloader_uart"
	"romloader_usb"
}

print(muhkuh_app.plugin_count())

aCode = {}

table.insert(aCode, "-- Add all include paths.")
table.insert(aCode, "package.path  = package.path  .. \";/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/system_folder_is_somewhere_else/lua_modules/?.lua\"")
table.insert(aCode, "")


table.insert(aCode, "-- Add all search paths for plugins.")
table.insert(aCode, "package.cpath = package.cpath .. \";/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/system_folder_is_somewhere_else/lua_plugins/?.so\"")
table.insert(aCode, "")


table.insert(aCode, "-- Set the Muhkuh version.")
-- table.insert(aCode, "_G.__MUHKUH_VERSION = \"" .. muhkuh_app.get_version() .. "\"")
table.insert(aCode, "_G.__MUHKUH_VERSION = \"Muhkuh 2.0.1234\"")
table.insert(aCode, "")
table.insert(aCode, "")


table.insert(aCode, "_G.__MUHKUH_TEST_XML = \"file:/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/data/test_description.xml\"")
table.insert(aCode, "_G.__MUHKUH_TEST_INDEX = 0")
table.insert(aCode, "")
table.insert(aCode, "")


table.insert(aCode, "-- Load all system modules.")
for tKey,strName in pairs(aLuaModules) do
	table.insert(aCode, string.format("require(\"%s\")", strName))
end
table.insert(aCode, "")

table.insert(aCode, "-- Load all enabled plugins.")
for tKey,strName in pairs(aLuaPlugins) do
	table.insert(aCode, string.format("require(\"%s\")", strName))
end
table.insert(aCode, "")
table.insert(aCode, "")


table.insert(aCode, "-- Add support for old style bitlib.")
table.insert(aCode, "if not bit.bshl then")
table.insert(aCode, "\tbit.bshl = bit.lshift")
table.insert(aCode, "end")
table.insert(aCode, "if not bit.bshr then")
table.insert(aCode, "\tbit.bshr = bit.rshift")
table.insert(aCode, "end")
table.insert(aCode, "")


table.insert(aCode, "-- NOTE: move this to a better place.")
table.insert(aCode, "function muhkuh.TestHasFinished()")
table.insert(aCode, "\twx.wxGetApp():ExitMainLoop()")
table.insert(aCode, "end")
table.insert(aCode, "")


table.insert(aCode, "muhkuh_system.boot_xml()")

return table.concat(aCode, "\n")

