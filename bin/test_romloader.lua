
--require("romloader_baka")
require("romloader_usb")

local function callback_progress(a,b)
	print(string.format("%d%% (%d/%d)", a*100/b, a, b))
	return true
end

local function callback(a,b)
	print(string.format("callback: a(%s)='%s', b(%s)='%s'", type(a), a, type(b), b))
	return true
end

local function get_rnd_data(len)
	local data = ""
	for i=1,len do
		data = data .. string.char(math.random(0,255))
	end
	return data
end

-- read binary file into string
-- returns the file or nil, message
function loadBin(strName)
	print("reading file " .. strName)
	local bin
	local f, msg = io.open(strName, "rb")
	if f then
		bin = f:read("*all")
		f:close()
		print(bin:len() .. " bytes read")
		return bin
	else
		return nil, msg
	end
end

local function hexdump(strData, iBytesPerRow)
	local iCnt
	local iByteCnt
	local strDump


	if not iBytesPerRow then
		iBytesPerRow = 16
	end

	iByteCnt = 0
	for iCnt=1,strData:len() do
		if iByteCnt==0 then
			strDump = string.format("%08X :", iCnt-1)
		end
		strDump = strDump .. string.format(" %02X", strData:byte(iCnt))
		iByteCnt = iByteCnt + 1
		if iByteCnt==iBytesPerRow then
			iByteCnt = 0
			print(strDump)
		end
	end
	if iByteCnt~=0 then
		print(strDump)
	end
end

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

	-- read a 32 bit value
	ulVal = tPlugin:read_data32(0x00000000)
	print( string.format("0x00000000: 0x%08x", ulVal) )

	-- write a 32 bit value
	tPlugin:write_data32(0x00008000, 0)

	-- read 128 bytes
	str = tPlugin:read_image(0x00008000, 128, callback_progress, 128)
	print("size: ", string.len(str))
	hexdump(str,16)

	--write 128 bytes
	strData = get_rnd_data(128)
	hexdump(strData,16)
	tPlugin:write_image(0x00008000, strData, callback_progress, string.len(strData))

	-- try to load netx500 binary
	strData, msg = loadBin("netxtest_nx500.bin")
	if not strData then
		print("failed to load binary: " .. msg)
		print("skipping call test.")
	else
		-- download binary to 0x8000
		tPlugin:write_image(0x00008000, strData, callback_progress, string.len(strData))

		-- set the parameter
		tPlugin:write_data32(0x00017000, 0xffffffff)
		tPlugin:write_data32(0x00017004, 0x12345678)
		tPlugin:write_data32(0x00017008, 0x00000000)
		tPlugin:call(0x00008000, 0x00017000, callback, 1)

		-- get the result
		ulValue = tPlugin:read_data32(0x08017000)
		print(string.format("call finished with result 0x%08x", ulValue))
	end

	-- access some plugin functions
	print( romloader_usb.ROMLOADER_CHIPTYP_NETX500 )
	print( tPlugin:GetChiptypName(romloader_usb.ROMLOADER_CHIPTYP_NETX500) )
	-- disconnect the plugin
	tPlugin:Disconnect()
	-- check if the plugin is disconnected
	print( tPlugin:IsConnected() )
	-- free the plugin
	tPlugin = nil
end
