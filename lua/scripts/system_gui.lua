aLuaModules = {
	"bit",
	"muhkuh",
	"muhkuh_system",
	"select_plugin_gui",
	"wx"
}

aCode = {}

strPath = string.gsub(muhkuh_application.get_lua_script_path(), "\\", "\\\\")
table.insert(aCode, "-- Add all include paths.")
table.insert(aCode, "package.path  = package.path  .. " .. "\";" .. strPath .. "\"")
table.insert(aCode, "")


strPath = string.gsub(muhkuh_application.get_lua_module_path(), "\\", "\\\\")
table.insert(aCode, "-- Add all search paths for plugins.")
table.insert(aCode, "package.cpath = package.cpath .. " .. "\";" .. strPath .. "\"")
table.insert(aCode, "")


table.insert(aCode, "-- Set the Muhkuh version.")
table.insert(aCode, "_G.__MUHKUH_VERSION = \"" .. muhkuh_application.get_version() .. "\"")
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
table.insert(aCode, "_G.__MUHKUH_PLUGINS = {}")
aLuaPlugins = muhkuh_application.get_plugins()
for iCnt,aAttr in ipairs(aLuaPlugins) do
	if aAttr.is_ok==true and aAttr.enabled==true then
		table.insert(aCode, string.format("require(\"%s\")", aAttr.module))
	end
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


table.insert(aCode, "muhkuh_system.boot_xml()")

strCode = table.concat(aCode, "\n")
print(strCode)
return strCode
