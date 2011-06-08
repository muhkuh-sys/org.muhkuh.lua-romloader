-- Load all system modules.
require("bit")
require("muhkuh")
require("muhkuh_system")
require("wx")

-- Load all enabled plugins.
require("romloader_usb")

-- Add all include paths.
package.path  = package.path  .. ";/home/cthelen/Compile/muhkuh_experimental/bin/lua/?.lua"
-- package.cpath = package.cpath + "";

-- Set the Muhkuh version.
_G.__MUHKUH_VERSION = "Muhkuh V2.0.0"


_G.__MUHKUH_TEST_XML = "file:/home/cthelen/Compile/muhkuh_experimental/build/build/romloader_test.mtd#zip:test_description.xml"
_G.__MUHKUH_TEST_INDEX = 0

-- add support for old style bitlib
if not bit.bshl then
	bit.bshl = bit.lshift
end
if not bit.bshr then
	bit.bshr = bit.rshift
end

function muhkuh.TestHasFinished()
--	wx.wxGetApp():GetTopWindow():Close()
	wx.wxGetApp():ExitMainLoop()
end

muhkuh_system.boot_xml()

