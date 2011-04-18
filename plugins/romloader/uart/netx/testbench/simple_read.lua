
-- NOTE: This script uses the "bit" and "romloader_usb" lua libraries. Both
-- are part of the "Muhkuh" experimental distribution.

require("romloader_uart")

require("select_plugin")


local function progress(cnt,max)
	print(string.format("%d%% (%d/%d)", cnt*100/max, cnt, max))
	return true
end

local function callback(a,b)
	print(string.format("'%s'", a))
	return true
end


tPlugin = select_plugin.SelectPlugin()
if not tPlugin then
	error("No plugin selected. Nothing to do.")
end

-- connect the plugin
tPlugin:Connect()

ulAddress = 0
print(string.format("Read address 0x%08x", ulAddress))
ulValue = tPlugin:read_data08(ulAddress)
print(string.format("0x%08x = 0x%02x", ulAddress, ulValue))
ulValue = tPlugin:read_data16(ulAddress)
print(string.format("0x%08x = 0x%04x", ulAddress, ulValue))
ulValue = tPlugin:read_data32(ulAddress)
print(string.format("0x%08x = 0x%08x", ulAddress, ulValue))
ulValue = tPlugin:read_data08(ulAddress)
print(string.format("0x%08x = 0x%02x", ulAddress, ulValue))
ulValue = tPlugin:read_data16(ulAddress)
print(string.format("0x%08x = 0x%04x", ulAddress, ulValue))
ulValue = tPlugin:read_data32(ulAddress)
print(string.format("0x%08x = 0x%08x", ulAddress, ulValue))
ulValue = tPlugin:read_data08(ulAddress)
print(string.format("0x%08x = 0x%02x", ulAddress, ulValue))
ulValue = tPlugin:read_data16(ulAddress)
print(string.format("0x%08x = 0x%04x", ulAddress, ulValue))
ulValue = tPlugin:read_data32(ulAddress)
print(string.format("0x%08x = 0x%08x", ulAddress, ulValue))

-- disconnect the plugin
tPlugin:Disconnect()

