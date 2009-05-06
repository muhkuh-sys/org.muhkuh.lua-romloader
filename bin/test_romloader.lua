
require("romloader_baka")

t = romloader_baka.romloader_baka_provider()

-- print the description
local desc = t:GetDesc()
--print("name:          " .. desc.pcPluginName)
print("id:            " .. desc.pcPluginId)
print("version major: " .. desc.tVersion.uiVersionMajor)
print("version minor: " .. desc.tVersion.uiVersionMinor)
print("version sub:   " .. desc.tVersion.uiVersionSub)

-- detect the interfaces
l = romloader_baka.PluginVector()
i = t:DetectInterfaces(l)
print("i:    " .. i)
print("size: " .. l:size())

-- show all interfaces
for cnt=0,l:size()-1 do
	print("name: " .. l[cnt]:GetName())
end


-- create the first interface
p = t:ClaimInterface(l[0])
--p = romloader_baka.romloader_baka(l[0]:GetName(), l[0]:GetTyp(), t)
print( p:IsConnected() )
p:Connect()
print( p:IsConnected() )
p:write_data32(0, 0)
print( romloader_baka.ROMLOADER_CHIPTYP_NETX500 )
print( p:GetChiptypName(romloader_baka.ROMLOADER_CHIPTYP_NETX500) )
p:Disconnect()
print( p:IsConnected() )
p = nil
