
require("romloader_uart")
require("select_plugin")
require("tester_cli")


local tPlugin = select_plugin.SelectPlugin()
if not tPlugin then
	error("Test canceled!")
end

tPlugin:Connect()

local tNetxTyp = tPlugin:GetChiptyp()
print("Found " .. tPlugin:GetChiptypName(tNetxTyp) .. " with " .. tPlugin:GetRomcodeName(tPlugin:GetRomcode()))

if tNetxTyp==romloader.ROMLOADER_CHIPTYP_NETX50 then
	strFilename = "montest_netx50.bin"
elseif tNetxTyp==romloader.ROMLOADER_CHIPTYP_NETX500 or tNetxTyp==romloader.ROMLOADER_CHIPTYP_NETX100 then
	strFilename = "montest_netx500.bin"
elseif tNetxTyp==romloader.ROMLOADER_CHIPTYP_NETX10 then
	strFilename = "montest_netx10.bin"
else
	error("Unknown chiptyp:", tPlugin)
end

ulParameter = 0x12345678

aAttr = tester.mbin_open(strFilename)
tester.mbin_debug(aAttr)
tester.mbin_write(nil, tPlugin, aAttr)
tester.mbin_set_parameter(tPlugin, aAttr, ulParameter)
ulValue = tester.mbin_execute(nil, tPlugin, aAttr)
print(string.format("Returncode: 0x%08x", ulValue))
if ulValue~=0 then
	error(string.format("The code returned an error: 0x%08x", ulValue))
end

ulValue = tPlugin:read_data32(aAttr.ulParameterStartAddress+8)
print(string.format("Return Parameter: 0x%08x", ulValue))
if ulValue~=ulParameter then
	error(string.format("The code did not copy the input parameter. Expected 0x%08x, got 0x%08x", ulParameter, ulValue))
end
