
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


-- select a provider
iPluginIdx = 1
tProvider = __MUHKUH_PLUGINS[iPluginIdx]
print(string.format("Using plugin %d: %s", iPluginIdx, tProvider:GetID()))


-- detect the interfaces
l = romloader_baka.PluginVector()
i = tProvider:DetectInterfaces(l)
print("i:    " .. i)
print("size: " .. l:size())

tPlugin = l[0]:Create()
print( tPlugin:IsConnected() )
tPlugin:Connect()
print( tPlugin:IsConnected() )
tPlugin:write_data32(0, 0)
print( romloader_baka.ROMLOADER_CHIPTYP_NETX500 )
print( tPlugin:GetChiptypName(romloader_baka.ROMLOADER_CHIPTYP_NETX500) )
tPlugin:Disconnect()
print( tPlugin:IsConnected() )
tPlugin = nil
