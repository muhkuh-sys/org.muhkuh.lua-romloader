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
	[romloader.ROMLOADER_CHIPTYP_NETX500]           = {ulTestAreaStart = 0x00004000, ulTestAreaSize  = 0x0001c000},
	[romloader.ROMLOADER_CHIPTYP_NETX100]           = {ulTestAreaStart = 0x00004000, ulTestAreaSize  = 0x0001c000},
	[romloader.ROMLOADER_CHIPTYP_NETX56]            = {ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0009c000},
	[romloader.ROMLOADER_CHIPTYP_NETX56B]           = {ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0009c000},
	[romloader.ROMLOADER_CHIPTYP_NETX50]            = {ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x00014000},
	[romloader.ROMLOADER_CHIPTYP_NETX10]            = {ulTestAreaStart = 0x08004000, ulTestAreaSize  = 0x0002c000},
	[romloader.ROMLOADER_CHIPTYP_NETX4000_RELAXED]  = {ulTestAreaStart = 0x04000000, ulTestAreaSize  = 0x00010000},
	[romloader.ROMLOADER_CHIPTYP_NETX90_MPW]        = {ulTestAreaStart = 0x00040000, ulTestAreaSize  = 0x00020000},
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
local ulTestSize           -- max. size of the memory area to test
local uiParameterLoops     -- 0 = endless
local fCheckExpectedOutput -- enable output checking
local fCheckOutputEnd      -- enable checking of the output at the return from a call   

if __MUHKUH_TEST_PARAMETER then
	ulTestSize           = tonumber(__MUHKUH_TEST_PARAMETER.testsize)             
	uiParameterLoops     = tonumber(__MUHKUH_TEST_PARAMETER.loops)                
	fCheckExpectedOutput = tobool(  __MUHKUH_TEST_PARAMETER.checkoutput, true)    
	fCheckOutputEnd      = tobool(  __MUHKUH_TEST_PARAMETER.checkoutputend, true) 
else
	ulTestSize           = 0x100
	uiParameterLoops     = 1     
	fCheckExpectedOutput = true 
	fCheckOutputEnd      = true 
end

local function printf(...) print(string.format(...)) end
printf("ulTestSize           0x%08x", ulTestSize)
printf("uiParameterLoops     %d",     uiParameterLoops)
printf("fCheckExpectedOutput %s",     tostring(fCheckExpectedOutput))
printf("fCheckOutputEnd      %s",     tostring(fCheckOutputEnd))


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
USB_BLOCK_SIZE = 63

function getExpectedOutput()
	local strLine = astrExpectedOutput[iExpectedOutputLine]
	assert((not fCheckExpectedOutput) or strLine, "Expected output exhausted")
	local strExpected
	
	local tPluginTyp = tPlugin:GetTyp()
	local tAsicTyp = tPlugin:GetChiptyp()
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

fOutputMissing = false

function fnCallbackCheckOutput(a,b)
	print(string.format("[netX %d] %s", b, a))
	
	-- error if a is not a string
	if type(a)~="string" then
		print("Message callback called with non-string:", type(a))
		
	-- allow a to be an empty string (due to timeout?)
	elseif a == "" then
		print("Message callback called with empty string")
		
	-- check if a is the expected string
	else
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
			if fCheckExpectedOutput then
				error("Unexpected output")
			end
		end
	end
	return true
end


function mbin_simple_run(tParentWindow, tPlugin, strFilename, aParameter)
	local aAttr
	aAttr = tester.mbin_open(strFilename)
	tester.mbin_debug(aAttr)
	tester.mbin_write(tParentWindow, tPlugin, aAttr)
	tester.mbin_set_parameter(tPlugin, aAttr, aParameter)
	iExpectedOutputLine = 1
	iLinePos = 1

	-- function mbin_execute(tParentWindow, tPlugin, aAttr, aParameter, fnCallback, ulUserData)
	local result = tester.mbin_execute(tParentWindow, tPlugin, aAttr, aParameter, fnCallbackCheckOutput, 0)
	if fCheckOutputEnd and not isOutputComplete() then
		error("did not receive all expected output")
	end
	
	if fOutputMissing then
		print("** Missing output **")
	end
	
	return result
end

--------------------------------------------------------------------------
--  test main
--------------------------------------------------------------------------


tPlugin = select_plugin.SelectPlugin()
if not tPlugin then
	error("Test canceled!")
end

tPlugin:Connect()


-- Get the maximum test area for the current chip.
local tAsicTyp = tPlugin:GetChiptyp()
local tTestArea = atTestAreas[tAsicTyp]
if tTestArea then
	ulTestAreaStart = tTestArea.ulTestAreaStart
	ulTestAreaSize  = tTestArea.ulTestAreaSize
else
	error("Unknown chiptyp! " .. tostring(tAsicTyp))
end
 

-- Limit the test size to the size of the test area.
if ulTestSize>ulTestAreaSize then
	ulTestSize = ulTestAreaSize
end






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

	print("---------------------------------------")
	print(string.format("** Loop %d **", uiLoopCounter))
	print(" call test")
	-- Get the chiptyp.
	local tAsicTyp = tPlugin:GetChiptyp()
	local ulSerialVectorAddr
	-- Get the binary for the ASIC.
	print(string.format("detected chip type: %d %s", tAsicTyp, tPlugin:GetChiptypName(tAsicTyp)))
	local uiAsicType = nil
	if tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX50 then
		uiAsicType = 50
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX100 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX500 then
		uiAsicType = 500
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX10 then
		uiAsicType = 10
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56 or tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX56B then
		uiAsicType = 56
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX4000_RELAXED then
		uiAsicType = 4000
	elseif tAsicTyp==romloader.ROMLOADER_CHIPTYP_NETX90_MPW then
		uiAsicType = 90
	end

	if uiAsicType==nil then
		error("Unknown chiptyp! " .. tostring(tAsicTyp))
	end

	local strBinaryName = string.format("netx/montest_netx%d.bin", uiAsicType)

	ulMagic = 0x12345678
	ulResult = mbin_simple_run(nil, tPlugin, strBinaryName, ulMagic)
	if ulResult~=0 then
		error(string.format("The code returned an error code: 0x%08x", ulResult))
	end


	uiLoopCounter = uiLoopCounter + 1
end

print("")
print(" #######  ##    ## ")
print("##     ## ##   ##  ")
print("##     ## ##  ##   ")
print("##     ## #####    ")
print("##     ## ##  ##   ")
print("##     ## ##   ##  ")
print(" #######  ##    ## ")
print("")

-- disconnect the plugin
tPlugin:Disconnect()
