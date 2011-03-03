
package.path  = package.path  .. ";/tmp/lib/lua/5.1/?.lua"
package.cpath = package.cpath .. ";/tmp/lib/lua/5.1/?.so"


require("bit")
require("romloader_usb")
require("select_plugin")
require("muhkuh")
require("tester_legacy")

strTestFile = "file:/somewhere/test_romloader.lua"
strTestCode = muhkuh.load(strTestFile)

__MUHKUH_TEST_PARAMETER = { ["testarea"]=0x00010000, ["testsize"]=0x00000080, ["loops"]=1 }

assert(loadstring(strTestCode, strTestFile))()
