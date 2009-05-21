
require("romloader_baka")


-- show all providers
print("Available plugins:")
for i,v in ipairs(__MUHKUH_PLUGINS) do
	local strID
	local tVer
	strID = v:GetID()
	tVer = v:GetVersion()
	print(string.format("%d: %s, v%d.%d.%d", i, strID, tVer.uiVersionMajor, tVer.uiVersionMinor, tVer.uiVersionSub))
end


-- detect all interfaces
aDetectedInterfaces = {}
for i,v in ipairs(__MUHKUH_PLUGINS) do
	local iDetected
	print(string.format("Detecting interfaces with plugin %s", v:GetID()))
	iDetected = v:DetectInterfaces(aDetectedInterfaces)
	print(string.format("Found %d interfaces with plugin %s", iDetected, v:GetID()))
end
print(string.format("Found a total of %d interfaces with %d plugins", #aDetectedInterfaces, #__MUHKUH_PLUGINS))


if #aDetectedInterfaces==0 then
	print("No interfaces detected, nothing to do...")
else
	-- select a list entry
	iIndex = 1
	
	-- create the plugin
	tPlugin = aDetectedInterfaces[iIndex]:Create()
	-- check if the plugin is connected (should be not)
	print( tPlugin:IsConnected() )
	-- connect the plugin
	tPlugin:Connect()
	-- check if the plugin is connected (should be connected now)
	print( tPlugin:IsConnected() )
	-- write a 32 bit value
	tPlugin:write_data32(0, 0)
	-- access some plugin functions
	print( romloader_baka.ROMLOADER_CHIPTYP_NETX500 )
	print( tPlugin:GetChiptypName(romloader_baka.ROMLOADER_CHIPTYP_NETX500) )
	-- disconnect the plugin
	tPlugin:Disconnect()
	-- check if the plugin is disconnected
	print( tPlugin:IsConnected() )
	-- free the plugin
	tPlugin = nil
end
