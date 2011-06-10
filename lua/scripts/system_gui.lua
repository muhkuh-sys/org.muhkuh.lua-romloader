-- Add all include paths.
package.path  = package.path  .. ";/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/system_folder_is_somewhere_else/lua_modules/?.lua"

-- Add all search paths for plugins.
package.cpath = package.cpath .. ";/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/system_folder_is_somewhere_else/lua_plugins/?.so"

-- Set the Muhkuh version.
_G.__MUHKUH_VERSION = "Muhkuh V2.0.0"


_G.__MUHKUH_TEST_XML = "file:/home/cthelen/Compile/muhkuh_experimental/build/build/demo_layout/data/test_description.xml"
_G.__MUHKUH_TEST_INDEX = 0


-- Load all system modules.
require("bit")
require("muhkuh")
require("muhkuh_system")
require("select_plugin_gui")
require("wx")

-- Load all enabled plugins.
require("romloader_usb")


-- Add support for old style bitlib.
if not bit.bshl then
	bit.bshl = bit.lshift
end
if not bit.bshr then
	bit.bshr = bit.rshift
end

-- NOTE: move this to a better place.
function muhkuh.TestHasFinished()
	wx.wxGetApp():ExitMainLoop()
end

muhkuh_system.boot_xml()

