-- List all system modules.
require("bit")
require("select_plugin")
require("tester_cli")

-- List all plugins.
require("romloader_usb")
require("romloader_uart")

--require("wx")
--require("muhkuh")

__MUHKUH_TEST_XML = "file:/home/christoph/Compile/muhkuh_tests_ng/trunk/mtd/nxdm10/targets/nxdm10.mtd#zip:test_description.xml"

-- add support for old style bitlib
if not bit.bshl then
	bit.bshl = bit.lshift
end
if not bit.bshr then
	bit.bshr = bit.rshift
end


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



__MUHKUH_TEST_PARAMETER = {}
__MUHKUH_TEST_PARAMETER["testarea"] = "0x08010000"
__MUHKUH_TEST_PARAMETER["testsize"] = "0x40"
__MUHKUH_TEST_PARAMETER["loops"] = "0"



-- muhkuh_system.boot_xml()
strTestFileName = "test_romloader.lua"
strTest = muhkuh.load(strTestFileName)

fnTest,luaresult = loadstring(strTest, strTestFileName)
if not fnTest then
	error("failed to compile test code:", luaresult)
else
	print("Running test...")
	luaresult,testresult = pcall(fnTest)
	print(luaresult,testresult)
end
