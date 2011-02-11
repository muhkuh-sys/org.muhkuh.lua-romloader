
require("bit")
require("muhkuh_system")
require("romloader_usb")
require("wx")
require("muhkuh")

__MUHKUH_TEST_XML = "file:/home/christoph/Compile/muhkuh_tests_ng/trunk/mtd/nxdm10/targets/nxdm10.mtd#zip:test_description.xml"

-- add support for old style bitlib
if not bit.bshl then
	bit.bshl = bit.lshift
end
if not bit.bshr then
	bit.bshr = bit.rshift
end

--[[
muhkuh = {}
function muhkuh.load(strName)
	local bin
	local f, msg = io.open(strName, "rb")
	if f then
		bin = f:read("*all")
		f:close()
		return bin
	else
		return nil, msg
	end
end
]]--

muhkuh_system.boot_xml()

