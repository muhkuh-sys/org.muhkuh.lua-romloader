
require("bit")
require("tester")

--require("romloader_baka")
require("romloader_usb")
require("romloader_uart")

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

local function test32(tPlugin, address, value)
	local readback


	print(string.format("write data:    0x%08X = 0x%08X", address, value))
	tPlugin:write_data32(address, value)
	readback = tPlugin:read_data32(address)
	print(string.format("readback data: 0x%08X = 0x%08X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end

local function test16(tPlugin, address, value)
	local readback


	print(string.format("write data:    0x%08X = 0x%04X", address, value))
	tPlugin:write_data16(address, value)
	readback = tPlugin:read_data16(address)
	print(string.format("readback data: 0x%08X = 0x%04X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end

local function test08(tPlugin, address, value)
	local readback


	print(string.format("write data:    0x%08X = 0x%02X", address, value))
	tPlugin:write_data08(address, value)
	readback = tPlugin:read_data08(address)
	print(string.format("readback data: 0x%08X = 0x%02X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end

local function get_rnd_data(len)
	local data = ""
	for i=1,len do
		data = data .. string.char(math.random(0,255))
	end
	return data
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


local iInterfaceIdx
repeat do
	repeat do
		-- detect all interfaces
		aDetectedInterfaces = {}
		for i,v in ipairs(__MUHKUH_PLUGINS) do
			local iDetected
			print(string.format("Detecting interfaces with plugin %s", v:GetID()))
			iDetected = v:DetectInterfaces(aDetectedInterfaces)
			print(string.format("Found %d interfaces with plugin %s", iDetected, v:GetID()))
		end
		print(string.format("Found a total of %d interfaces with %d plugins", #aDetectedInterfaces, #__MUHKUH_PLUGINS))
		print("")
		-- Show all detected interfaces.
		print("Please select the interface:")
		for i,v in ipairs(aDetectedInterfaces) do
			print(string.format("%d: %s (%s) Used: %s, Valid: %s", i, v:GetName(), v:GetTyp(), tostring(v:IsUsed()), tostring(v:IsValid())))
		end
		print("R: rescan")
		-- Get the user input.
		io.write(">")
		strInterface = io.read():lower()
	end until strInterface~="r"
	iInterfaceIdx = tonumber(strInterface)
end until iInterfaceIdx~=nil and iInterfaceIdx>0 and iInterfaceIdx<=#aDetectedInterfaces

-- create the plugin
tPlugin = aDetectedInterfaces[iInterfaceIdx]:Create()
-- check if the plugin is connected (should be not)
print( tPlugin:IsConnected() )
-- connect the plugin
tPlugin:Connect()
-- check if the plugin is connected (should be connected now)
print( tPlugin:IsConnected() )


__MUHKUH_TEST_PARAMETER = { ["testarea"]=0x00010000, ["testsize"]=0x00000080, ["loops"]=1 }


-- get the test parameters
local testarea = tonumber(__MUHKUH_TEST_PARAMETER["testarea"])
local testsize = tonumber(__MUHKUH_TEST_PARAMETER["testsize"])
local parameter_loops = tonumber(__MUHKUH_TEST_PARAMETER["loops"])

local ulLoopCounter = 0
-- If the "loops" parameter is 0, loop forever (or until an error occurs).
local fLoopEndless = (parameter_loops==0)

while fLoopEndless==true or ulLoopCounter<parameter_loops do
	print("***************************************")
	print("***************************************")
	print("**                                   **")
	print(string.format("** Loop %08d                     **", ulLoopCounter))
	print("**                                   **")
	print("***************************************")
	print("***************************************")

	print("---------------------------------------")
	print(" test 32 bit access")
	test32(tPlugin, testarea, 0x00000000)
	test32(tPlugin, testarea, 0x000000FF)
	test32(tPlugin, testarea, 0x0000FF00)
	test32(tPlugin, testarea, 0x00FF0000)
	test32(tPlugin, testarea, 0xFF000000)
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" test 16 bit access")
	test16(tPlugin, testarea, 0x0000)
	test16(tPlugin, testarea, 0x00FF)
	test16(tPlugin, testarea, 0xFF00)
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" test 8 bit access")
	test08(tPlugin, testarea, 0x00)
	test08(tPlugin, testarea, 0xFF)
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, write32")
	data = get_rnd_data(testsize)
	print("Writing data in 32 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()/4-1 do
		local value
		local address
		value = data:byte(i*4+1) + 0x00000100*data:byte(i*4+2) + 0x00010000*data:byte(i*4+3) + 0x01000000*data:byte(i*4+4)
		address = testarea + i*4
		print(string.format("write data:    0x%08X = 0x%08X", address, value))
		tPlugin:write_data32(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, testarea, data:len())
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, write16")
	data = get_rnd_data(testsize)
	print("Writing data in 16 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()/2-1 do
		local value
		local address
		value = data:byte(i*2+1) + 0x00000100*data:byte(i*2+2)
		address = testarea + i*2
		print(string.format("write data:    0x%08X = 0x%04X", address, value))
		tPlugin:write_data16(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, testarea, data:len())
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, write08")
	data = get_rnd_data(testsize)
	print("Writing data in 8 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()-1 do
		local value
		local address
		value = data:byte(i+1)
		address = testarea + i
		print(string.format("write data:    0x%08X = 0x%02X", address, value))
		tPlugin:write_data08(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, testarea, data:len())
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, read32")
	data = get_rnd_data(testsize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, testarea, data)
	data_readback = ""
	for i=0,data:len()/4-1 do
		local value
		local address
		address = testarea + i*4
		value = tPlugin:read_data32(address)
		print(string.format("read data: 0x%08X = 0x%08X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value,  8), 0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value, 16), 0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value, 24), 0xff) )
	end
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, read16")
	data = get_rnd_data(testsize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, testarea, data)
	data_readback = ""
	for i=0,data:len()/2-1 do
		local value
		local address
		address = testarea + i*2
		value = tPlugin:read_data16(address)
		print(string.format("read data: 0x%08X = 0x%04X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value,  8), 0xff) )
	end
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" range test, read08")
	data = get_rnd_data(testsize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, testarea, data)
	data_readback = ""
	for i=0,data:len()-1 do
		local value
		local address
		address = testarea + i
		value = tPlugin:read_data08(address)
		print(string.format("read data: 0x%08X = 0x%02X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
	end
	print("Readback data:")
	tester.hexdump(data_readback,16)
	if data~=data_readback then
		error("Readback data does not match written values!")
	end
	print("Ok!")
	print(" ")

	ulLoopCounter = ulLoopCounter + 1
end


-- disconnect the plugin
tPlugin:Disconnect()
-- check if the plugin is disconnected
print( tPlugin:IsConnected() )
-- free the plugin
tPlugin = nil
