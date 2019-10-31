-----------------------------------------------------------------------------
--   Copyright (C) 2012 by Christoph Thelen                                --
--   doc_bacardi@users.sourceforge.net                                     --
--                                                                         --
--   This program is free software; you can redistribute it and/or modify  --
--   it under the terms of the GNU General Public License as published by  --
--   the Free Software Foundation; either version 2 of the License, or     --
--   (at your option) any later version.                                   --
--                                                                         --
--   This program is distributed in the hope that it will be useful,       --
--   but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--   GNU General Public License for more details.                          --
--                                                                         --
--   You should have received a copy of the GNU General Public License     --
--   along with this program; if not, write to the                         --
--   Free Software Foundation, Inc.,                                       --
--   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
-----------------------------------------------------------------------------
-- 
-- Description:
--   test_romloader.lua   Test plugin functions
--
--   - write random data with write_data8/16/32 and check with read_image
--   - write random data with write_image and check with read_data8/16/32
--   - call binary and check its output
-----------------------------------------------------------------------------

require("muhkuh_cli_init")
require("bit")

-- The usable intram when MI 2.0 is running
atTestAreas = {
	[romloader.ROMLOADER_CHIPTYP_NETX500]           = {strName = "netx500",    ulTestAreaStart = 0x00004000, ulTestAreaSize  = 0x0001c000},
	[romloader.ROMLOADER_CHIPTYP_NETX100]           = {strName = "netx500",    ulTestAreaStart = 0x00004000, ulTestAreaSize  = 0x0001c000},
	[romloader.ROMLOADER_CHIPTYP_NETX56]            = {strName = "netx56",     ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0009c000},
	[romloader.ROMLOADER_CHIPTYP_NETX56B]           = {strName = "netx56",     ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0009c000},
	[romloader.ROMLOADER_CHIPTYP_NETX50]            = {strName = "netx50",     ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x00014000},
	[romloader.ROMLOADER_CHIPTYP_NETX10]            = {strName = "netx10",     ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0002c000},
	[romloader.ROMLOADER_CHIPTYP_NETX4000_RELAXED]  = {strName = "netx4000",   ulTestAreaStart = 0x04010000, ulTestAreaSize  = 0x000f0000},
	[romloader.ROMLOADER_CHIPTYP_NETX4000_FULL]     = {strName = "netx4000",   ulTestAreaStart = 0x04010000, ulTestAreaSize  = 0x000f0000},
	[romloader.ROMLOADER_CHIPTYP_NETX4100_SMALL]    = {strName = "netx4000",   ulTestAreaStart = 0x04010000, ulTestAreaSize  = 0x000f0000},
	[romloader.ROMLOADER_CHIPTYP_NETX90_MPW]        = {strName = "netx90",     ulTestAreaStart = 0x00040000, ulTestAreaSize  = 0x00020000},
	[romloader.ROMLOADER_CHIPTYP_NETX90]            = {strName = "netx90",     ulTestAreaStart = 0x00040000, ulTestAreaSize  = 0x00020000},
	[romloader.ROMLOADER_CHIPTYP_NETX90B]           = {strName = "netx90",     ulTestAreaStart = 0x00040000, ulTestAreaSize  = 0x00020000},
	[romloader.ROMLOADER_CHIPTYP_NETIOLA]           = {strName = "netiol",     ulTestAreaStart = 0x00009000, ulTestAreaSize  = 0x00005000},
	[romloader.ROMLOADER_CHIPTYP_NETIOLB]           = {strName = "netiol",     ulTestAreaStart = 0x00009000, ulTestAreaSize  = 0x00005000},
}

-- print date/time in every line, useful when it stops in the middle of the night
local orig_print = print
function print(...)
	io.write(os.date(), " ")
	orig_print(...)
end

-- convert input parameter to boolean
function tobool(x, fDefault)
	if type(x)=="boolean" then
		return x
	elseif type(x)=="string" then
		return x=="true" or x=="TRUE"
	else
		return fDefault
	end
end

-- get the test parameters
local ulTestSize           = 0x1000 -- max. size of the memory area to test
local uiParameterLoops     = 1      -- 0 = endless
local fDoMemTest           = true   -- memory read/write test
local fDoCallTest          = true   -- call/output message test
local fCheckOutput         = true   -- check messages from call test program 
                                    -- Set to false for JTAG without DCC support (netX 4000, 90, netIOL)
local fCheckOutputEnd      = true   -- check the output at the return from a call   
local USB_BLOCK_SIZE       = 63     -- max. USB block size

if __MUHKUH_TEST_PARAMETER then
	ulTestSize           = tonumber(__MUHKUH_TEST_PARAMETER.testsize)             
	uiParameterLoops     = tonumber(__MUHKUH_TEST_PARAMETER.loops)                
	fCheckOutput         = tobool(  __MUHKUH_TEST_PARAMETER.checkoutput, true)    
	fCheckOutputEnd      = tobool(  __MUHKUH_TEST_PARAMETER.checkoutputend, true) 
end

local function printf(...) print(string.format(...)) end
printf("ulTestSize           0x%08x", ulTestSize)
printf("uiParameterLoops     %d",     uiParameterLoops)
printf("fDoMemTest           %s",     tostring(fDoMemTest))
printf("fDoCallTest          %s",     tostring(fDoCallTest))
printf("fCheckOutput         %s",     tostring(fCheckOutput))
printf("fCheckOutputEnd      %s",     tostring(fCheckOutputEnd))
printf("USB_BLOCK_SIZE       %d",     USB_BLOCK_SIZE)


--------------------------------------------------------------------------
--  Get plugin
--------------------------------------------------------------------------

function getPluginByName(strName)
	for iPluginClass, tPluginClass in ipairs(__MUHKUH_PLUGINS) do
		local iDetected
		local aDetectedInterfaces = {}
		print(string.format("Detecting interfaces with plugin %s", tPluginClass:GetID()))
		iDetected = tPluginClass:DetectInterfaces(aDetectedInterfaces)
		print(string.format("Found %d interfaces with plugin %s", iDetected, tPluginClass:GetID()))
		
		for i,v in ipairs(aDetectedInterfaces) do
			print(string.format("%d: %s (%s) Used: %s, Valid: %s", i, v:GetName(), v:GetTyp(), tostring(v:IsUsed()), tostring(v:IsValid())))
			if strName == v:GetName() then
				if not v:IsValid() then
					return nil, "Plugin is not valid"
				elseif v:IsUsed() then
					return nil, "Plugin is in use"
				else
					print("found plugin")
					local tPlugin = v:Create()
					if tPlugin then 
						return tPlugin
					else
						return nil, "Error creating plugin instance"
					end
				end
			end
		end
	end
	return nil, "plugin not found"
end

function getPlugin(strPluginName)
	local tPlugin, strError
	if strPluginName then
		-- get the plugin by name
		tPlugin, strError = getPluginByName(strPluginName)
	else
		-- Ask the user to pick a plugin.
		tPlugin = select_plugin.SelectPlugin()
		if tPlugin == nil then
			strError = "No plugin selected!"
		end
	end
	
	return tPlugin, strError
end



--------------------------------------------------------------------------
--  short memory access test
--------------------------------------------------------------------------


function test32(tPlugin, address, value)
	local readback
	
	print(string.format("write data:    0x%08X = 0x%08X", address, value))
	tPlugin:write_data32(address, value)
	readback = tPlugin:read_data32(address)
	print(string.format("readback data: 0x%08X = 0x%08X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end

function test16(tPlugin, address, value)
	local readback
	
	print(string.format("write data:    0x%08X = 0x%04X", address, value))
	tPlugin:write_data16(address, value)
	readback = tPlugin:read_data16(address)
	print(string.format("readback data: 0x%08X = 0x%04X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end

function test08(tPlugin, address, value)
	local readback
	
	print(string.format("write data:    0x%08X = 0x%02X", address, value))
	tPlugin:write_data08(address, value)
	readback = tPlugin:read_data08(address)
	print(string.format("readback data: 0x%08X = 0x%02X", address, readback))
	if value~=readback then
		error("*** Error: Readback does not match written value ***")
	end
end


function basic_access_test(args)
	local tPlugin = args.tPlugin 
	local ulTestAreaStart = args.ulTestAreaStart
	
	print("---------------------------------------")
	print(" test 32 bit access")
	test32(tPlugin, ulTestAreaStart, 0x00000000)
	test32(tPlugin, ulTestAreaStart, 0x000000FF)
	test32(tPlugin, ulTestAreaStart, 0x0000FF00)
	test32(tPlugin, ulTestAreaStart, 0x00FF0000)
	test32(tPlugin, ulTestAreaStart, 0xFF000000)
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" test 16 bit access")
	test16(tPlugin, ulTestAreaStart, 0x0000)
	test16(tPlugin, ulTestAreaStart, 0x00FF)
	test16(tPlugin, ulTestAreaStart, 0xFF00)
	print("Ok!")
	print(" ")

	print("---------------------------------------")
	print(" test 8 bit access")
	test08(tPlugin, ulTestAreaStart, 0x00)
	test08(tPlugin, ulTestAreaStart, 0xFF)
	print("Ok!")
	print(" ")
	
	return true
end 



--------------------------------------------------------------------------
-- test memory access
--------------------------------------------------------------------------

local function get_rnd_data(len)
	local data = ""
	for i=1,len do
		data = data .. string.char(math.random(0,255))
	end
	return data
end

COMPARE_MAX_ERRORS = 10
local function compare_readback(strData, strData_readback)
	print("Readback data:")
	tester.hexdump(data_readback,16)
	
	local iLenData = strData:len()
	local iLenData_readback = strData_readback:len()
	if iLenData~=iLenData_readback then
		error(string.format("Data and readback data differ in length: %d <-> %d", iLenData, iLenData_readback))
	end
	
	local iCompErrs = 0
	for i=1, iLenData do
		local bData = strData:byte(i)
		local bData_readback = strData_readback:byte(i)
		if bData~=bData_readback then
			print(string.format("Readback data does not match - offset 0x%08x, data:0x%02x, readback: 0x%02x", i-1, bData, bData_readback))
			iCompErrs = iCompErrs + 1
			if iCompErrs >= COMPARE_MAX_ERRORS then
				print("Too many errors, stopping")
				break
			end
		end
	end
	
	if iCompErrs>0 then
		error("Readback data does not match written values!")
	else
		print("Ok!")
		print(" ")
		return true
	end
end

function mem_test(tArgs)
	local tPlugin = tArgs.tPlugin
	local ulTestAreaStart = tArgs.ulTestAreaStart
	local ulTestSize = tArgs.ulTestSize
	local uiLoopCounter = tArgs.uiLoopCounter
	
	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, write32")
	data = get_rnd_data(ulTestSize)
	print("Writing data in 32 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()/4-1 do
		local value
		local address
		value = data:byte(i*4+1) + 0x00000100*data:byte(i*4+2) + 0x00010000*data:byte(i*4+3) + 0x01000000*data:byte(i*4+4)
		address = ulTestAreaStart + i*4
		print(string.format("write data:    0x%08X = 0x%08X", address, value))
		tPlugin:write_data32(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, ulTestAreaStart, data:len())
	assert(compare_readback(data, data_readback))

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, write16")
	data = get_rnd_data(ulTestSize)
	print("Writing data in 16 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()/2-1 do
		local value
		local address
		value = data:byte(i*2+1) + 0x00000100*data:byte(i*2+2)
		address = ulTestAreaStart + i*2
		print(string.format("write data:    0x%08X = 0x%04X", address, value))
		tPlugin:write_data16(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, ulTestAreaStart, data:len())
	assert(compare_readback(data, data_readback))

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, write08")
	data = get_rnd_data(ulTestSize)
	print("Writing data in 8 bit chunks:")
	tester.hexdump(data,16)
	for i=0,data:len()-1 do
		local value
		local address
		value = data:byte(i+1)
		address = ulTestAreaStart + i
		print(string.format("write data:    0x%08X = 0x%02X", address, value))
		tPlugin:write_data08(address, value)
	end
	data_readback = tester.stdRead(none, tPlugin, ulTestAreaStart, data:len())
	assert(compare_readback(data, data_readback))

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, read32")
	data = get_rnd_data(ulTestSize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, ulTestAreaStart, data)
	data_readback = ""
	for i=0,data:len()/4-1 do
		local value
		local address
		address = ulTestAreaStart + i*4
		value = tPlugin:read_data32(address)
		print(string.format("read data: 0x%08X = 0x%08X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value,  8), 0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value, 16), 0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value, 24), 0xff) )
	end
	assert(compare_readback(data, data_readback))

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, read16")
	data = get_rnd_data(ulTestSize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, ulTestAreaStart, data)
	data_readback = ""
	for i=0,data:len()/2-1 do
		local value
		local address
		address = ulTestAreaStart + i*2
		value = tPlugin:read_data16(address)
		print(string.format("read data: 0x%08X = 0x%04X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
		data_readback = data_readback .. string.char( bit.band( bit.rshift(value,  8), 0xff) )
	end
	assert(compare_readback(data, data_readback))

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" range test, read08")
	data = get_rnd_data(ulTestSize)
	print("Writing image:")
	tester.hexdump(data,16)
	tester.stdWrite(none, tPlugin, ulTestAreaStart, data)
	data_readback = ""
	for i=0,data:len()-1 do
		local value
		local address
		address = ulTestAreaStart + i
		value = tPlugin:read_data08(address)
		print(string.format("read data: 0x%08X = 0x%02X", address, value))
		data_readback = data_readback .. string.char( bit.band(            value,      0xff) )
	end
	assert(compare_readback(data, data_readback))
end




--------------------------------------------------------------------------
-- execute binary, check output in message callback against expected output
--------------------------------------------------------------------------

-- [netX 0] . *** test skeleton start ***
-- 
-- [netX 0] . Parameter Address: 0x08018880
-- 
-- [netX 0] . Parameter: 0x12345678
-- 
-- [netX 0] 012345678901234567890123456789012345678901234567890123456789012
-- [netX 0] 345678901234567
-- 
-- [netX 0] 000000000011111111112222222222333333333344444444445555555555666
-- [netX 0] 666666677777777
-- 
-- [netX 0] . counting from 0 to 100 and 0ms delay.
-- ...
-- [netX 0] % 97/100
-- [netX 0] % 98/100
-- [netX 0] % 99/100
-- [netX 0] . counting from 0 to 8 and 500ms delay.
-- [netX 0] % 0/8
-- [netX 0] % 1/8
-- [netX 0] % 2/8
-- [netX 0] % 3/8
-- [netX 0] % 4/8
-- [netX 0] % 5/8
-- [netX 0] % 6/8
-- [netX 0] % 7/8
-- [netX 0] . counting from 0 to 4 and 1000ms delay.
-- [netX 0] % 0/4
-- [netX 0] % 1/4
-- [netX 0] % 2/4
-- [netX 0] % 3/4
-- [netX 0] . counting from 0 to 2 and 2000ms delay.
-- [netX 0] % 0/2

astrExpectedOutput = {}

function addExpectedOutput(str)
	table.insert(astrExpectedOutput, str)
end

function addExpectedOutputLoop(n, iMsDelay)
	addExpectedOutput(string.format(". counting from 0 to %d and %dms delay.%s", n, iMsDelay, strNetxCR))
	for i=0, n-1 do
		addExpectedOutput(string.format("%% %d/%d%s", i, n, strNetxCR))
	end
end

-- Construct the expected output
strNetxCR = "\r\n" 
addExpectedOutput(". *** test skeleton start ***" .. strNetxCR)
--addExpectedOutput(". Parameter Address: 0x08018880" .. strNetxCR)
addExpectedOutput("skip")
addExpectedOutput(". Parameter: 0x12345678" .. strNetxCR)
addExpectedOutput("012345678901234567890123456789012345678901234567890123456789012345678901234567" .. strNetxCR)
addExpectedOutput("000000000011111111112222222222333333333344444444445555555555666666666677777777" .. strNetxCR)
addExpectedOutputLoop(100, 0)
addExpectedOutputLoop(8, 500)
addExpectedOutputLoop(4, 1000)
addExpectedOutputLoop(2, 2000)


-- Get the next piece of expected output.
-- If the Plugin is USB and the chip type is not netx 100/500, 
-- split the current line into 63 byte segments
-- Uses:
-- astrExpectedOutput    array of expected output lines
-- iExpectedOutputLine   index into astrExpectedOutput
-- iLinePos              1-based position in iExpectedOutputLine
-- tPlugin               plugin, used to check we're using USB
-- tPluginTyp            the interface used, e.g. UART, USB, JTAG
-- tAsicTyp              the netx type, e.g. netx 500.

function getExpectedOutput()
	local strLine = astrExpectedOutput[iExpectedOutputLine]
	assert((not fCheckOutput) or strLine, "Expected output exhausted")
	local strExpected
	
	if tPluginTyp=="romloader_usb" 
	and tAsicTyp~=romloader.ROMLOADER_CHIPTYP_NETX100 and tAsicTyp~=romloader.ROMLOADER_CHIPTYP_NETX500 then
		local iLen = strLine:len()
		strExpected = strLine:sub(iLinePos, iLinePos + USB_BLOCK_SIZE - 1)
		iLinePos = iLinePos + USB_BLOCK_SIZE
		if iLinePos > iLen then
			iLinePos = 1
			iExpectedOutputLine = iExpectedOutputLine + 1
		end
	elseif tPluginTyp=="romloader_jtag" then
		strExpected = strLine:gsub(string.char(13, 10), string.char(13, 10, 10))
		iExpectedOutputLine = iExpectedOutputLine + 1
		
	else
		strExpected = strLine
		iExpectedOutputLine = iExpectedOutputLine + 1
	end

		return strExpected
end

function isOutputComplete()
	--return true 
	return iExpectedOutputLine == #astrExpectedOutput + 1
end

function showOutputHex(str)
		print(str)
		if str then
			print(string.format("%d bytes", str:len()))
			tester.hexdump(str,16)
		end
end

function fnCallbackCheckOutput(a,b)
	print(string.format("[netX %d] %s", b, a))
	
	-- error if a is not a string
	if type(a)~="string" then
		print("Message callback called with non-string:", type(a))
		
	-- allow a to be an empty string (due to timeout?)
	elseif a == "" then
		print("Message callback called with empty string")
		
	-- check if a is the expected string
	elseif fCheckOutput then
		local strExpected = getExpectedOutput()
		if a == strExpected or strExpected=="skip" then
			-- ok
		else
			print("Unexpected output")
			print("Expected:")
			print(strExpected)
			print("Received:")
			print(a)
			if a then
				print(string.format("%d bytes", a:len()))
				tester.hexdump(a,16)
			end
			fOutputMissing = true
			error("Unexpected output")
		end
	else 
		print("Received:")
		print(a)
		if a then
			print(string.format("%d bytes", a:len()))
			tester.hexdump(a,16)
		end
	end
	
	iCallbackCount = iCallbackCount + 1
	if iCallbackCount == iMaxCallbackCount then 
		print("Lua callback: stopping call")
		return false
	else 
		return true
	end
end


function mbin_simple_run(tParentWindow, tPlugin, strFilename, aParameter)
	local aAttr
	aAttr = tester.mbin_open(strFilename)
	tester.mbin_debug(aAttr)
	tester.mbin_write(tParentWindow, tPlugin, aAttr)
	tester.mbin_set_parameter(tPlugin, aAttr, aParameter)
	
	-- function mbin_execute(tParentWindow, tPlugin, aAttr, aParameter, fnCallback, ulUserData)
	local ulResult = tester.mbin_execute(tParentWindow, tPlugin, aAttr, aParameter, fnCallbackCheckOutput, 0)
	local ulInitParam = tPlugin:read_data32(aAttr.ulParameterStartAddress+4)
	local ulReturnMessage = tPlugin:read_data32(aAttr.ulParameterStartAddress+8)
	
	return ulResult, ulInitParam, ulReturnMessage
end




function call_test(tArgs)
	local tPlugin = tArgs.tPlugin
	local strAsicName = tArgs.strAsicName
	local uiLoopCounter = tArgs.uiLoopCounter
	
	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" call test")
	
	iExpectedOutputLine = 1
	iLinePos = 1
	
	iMaxCallbackCount = 0
	iCallbackCount = 0
	
	local strBinaryName = string.format("netx/montest_%s.bin", strAsicName)
	ulMagic = 0x12345678
	local ulResult, ulInitParam, ulReturnMessage = mbin_simple_run(nil, tPlugin, strBinaryName, ulMagic)
	
	if ulResult~=0 then
		error(string.format("The code returned an error code: 0x%08x", ulResult))
	end
	
	if ulReturnMessage ~= ulMagic then
		error(string.format("The return message value is not correct: 0x%08x, expected: 0x%086x", ulReturnMessage, ulMagic))
	end
	
	if fCheckOutput then
		if fCheckOutputEnd and not isOutputComplete() then
			error("did not receive all expected output")
		end
		
		if fOutputMissing then
			print("** Missing output **")
		end
	end
end


--------------------------------------------------------------------------
--  test main
--------------------------------------------------------------------------


function test_main(tPlugin)
	-- set global variables, because they're needed by the output checker
	tPluginTyp = tPlugin:GetTyp()
	tAsicTyp = tPlugin:GetChiptyp()
				
	-- Get the maximum test area for the current chip.
	local tTestArea = atTestAreas[tAsicTyp]
	assert(tTestArea, "Unknown chiptyp! " .. tostring(tAsicTyp))
	
	-- Limit the test size to the size of the test area.
	if ulTestSize>tTestArea.ulTestAreaSize then
		ulTestSize = ulTestAreaSize
	end
	
	-- Disable output checks when using a JTAG connection without DCC message support.
	local fNoDCCSupport = 
		tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX4000_RELAXED or
		tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX4000_FULL or
		tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX4100_SMALL or
		-- tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX90_MPW or
		-- tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX90 or
		-- tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETX90B or
		tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETIOLA or
		tAsicTyp == romloader.ROMLOADER_CHIPTYP_NETIOLB
	
	if tPluginTyp=="romloader_jtag" and fNoDCCSupport then
		print("No DCC support - disabling output checks")
		fCheckOutput = false 
	end
	
	local tArgs = {
		tPlugin         = tPlugin,
		tAsicTyp        = tAsicTyp,
		strAsicName     = tTestArea.strName,
		ulTestAreaStart = tTestArea.ulTestAreaStart,
		ulTestAreaSize  = tTestArea.ulTestAreaSize,
		uiLoopCounter   = 0,
		ulTestSize      = ulTestSize,
	}
	
	
	local uiLoopCounter = 0
	-- If the "loops" parameter is 0, loop forever (or until an error occurs).
	local fLoopEndless = (uiParameterLoops==0)
	
	while fLoopEndless==true or uiLoopCounter<uiParameterLoops do
		print("***************************************")
		print("***************************************")
		print("**                                   **")
		print(string.format("** Loop %08d                     **", uiLoopCounter))
		print("**                                   **")
		print("***************************************")
		print("***************************************")
	
		tArgs.uiLoopCounter = uiLoopCounter
		basic_access_test(tArgs)
		if fDoMemTest then
			mem_test(tArgs)
		end 
		if fDoCallTest then
			call_test(tArgs)
		end
		uiLoopCounter = uiLoopCounter + 1
	end
	
	tPlugin:Disconnect()
	
end 




E_OK             = 0
E_ERROR          = 1
E_INCORRECT_ARGS = 2
E_NO_PLUGIN      = 3

function error_handler(e)
	 print(debug.traceback(e))
end


local iRet

if #arg == 0 then
	strPluginName = nil
	iRet = E_OK
elseif #arg == 2 and arg[1]=="-p" and type(arg[2])=="string" then
	strPluginName = arg[2]
	iRet = E_OK
else
	print("Usage: lua test_romloader.lua [-p plugin_name]")
	iRet = E_INCORRECT_ARGS
end


if iRet == E_OK then
	local fOk, tPlugin, strError = xpcall(
		function () 
			local tPlugin, strError = getPlugin(strPluginName)
			if tPlugin ~= nil then
				tPlugin:Connect()
				
				strPluginName = tPlugin:GetName()
				strPluginType = tPlugin:GetTyp()
				iChipType = tPlugin:GetChiptyp()
				strChipType = tPlugin:GetChiptypName(iChipType)
				
			end
			return tPlugin, strError
		end,
		error_handler
		)
		
	if fOk~=true or tPlugin==nil then
		iRet = E_NO_PLUGIN
		print(strError or "Failed to open plugin.")
		
	else
		fOk = xpcall(
			function () return test_main(tPlugin) end, 
			error_handler
			)
		
		tPlugin:Disconnect()
		
		if fOk~=true then
			iRet = E_ERROR 
		else 
			iRet = E_OK
		end
	end
end

print()
printf("Plugin: %s (%s)", strPluginName or "None", strPluginType or "None")
printf("Chip type: %s (%d)", strChipType or "None", iChipType or 0)

if iRet == E_OK then

print("")
print(" #######  ##    ##")
print("##     ## ##   ## ")
print("##     ## ##  ##  ")
print("##     ## #####   ")
print("##     ## ##  ##  ")
print("##     ## ##   ## ")
print(" #######  ##    ##")
print("")

else

print("")
print("######## #######  #######   ######  ####### ")
print("##       ##    ## ##    ## ##    ## ##    ##")
print("##       ##    ## ##    ## ##    ## ##    ##")
print("#######  #######  #######  ##    ## ####### ")
print("##       ## ##    ## ##    ##    ## ## ##   ")
print("##       ##  ##   ##  ##   ##    ## ##  ##  ")
print("######## ##   ##  ##   ##   ######  ##   ## ")
print("")

end

os.exit(iRet)
