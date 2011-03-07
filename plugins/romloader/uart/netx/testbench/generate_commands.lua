
require("bit")
require("mhash")


MONITOR_STREAM_PACKET_START = 0x2a


MONITOR_COMMAND_Read                    = 0x00
MONITOR_COMMAND_Write                   = 0x01
MONITOR_COMMAND_Execute                 = 0x02

MONITOR_ACCESSSIZE_Byte                 = 0x00
MONITOR_ACCESSSIZE_Word                 = 0x40
MONITOR_ACCESSSIZE_Long                 = 0x80

MONITOR_STATUS_Ok                       = 0x00
MONITOR_STATUS_CallMessage              = 0x01
MONITOR_STATUS_CallFinished             = 0x02
MONITOR_STATUS_InvalidCommand           = 0x03
MONITOR_STATUS_InvalidPacketSize        = 0x04
MONITOR_STATUS_InvalidSizeParameter     = 0x05


local function write_data(strFilename, aData)
	-- Get the user data for the packet.
	strUserData = table.concat(aData)
	-- The size of the user data.
	sizPacket = strUserData:len()
	-- Prepend the size.
	table.insert(aData, 1, string.char(bit.band(sizPacket,0xff), bit.band(bit.rshift(sizPacket, 8), 0xff)))

	-- Generate the CRC16.
	local mh = mhash.mhash_state()
	mh:init(mhash.MHASH_CRC16)
	mh:hash(table.concat(aData))
	local hash = mh:hash_end()
	table.insert(aData, hash:sub(2,2) .. hash:sub(1,1))

	-- Test the CRC16.
	local mh = mhash.mhash_state()
	mh:init(mhash.MHASH_CRC16)
	mh:hash(table.concat(aData))
	local hash = mh:hash_end()
	mh = nil
	print(string.format("%02x %02x", hash:byte(2), hash:byte(1)))

	-- Prepend the start character.
	table.insert(aData, 1, string.char(MONITOR_STREAM_PACKET_START))

	local tFileOut = io.open(strFilename, "wb")
	tFileOut:write(table.concat(aData))
	tFileOut:close()
end





aData = {}
-- Read Data with access size = byte.
table.insert(aData, string.char(MONITOR_COMMAND_Read + MONITOR_ACCESSSIZE_Byte))
-- Read 8 bytes.
table.insert(aData, string.char(8))
-- Read from address 0x00200000
ulAddress = 0x00200000
table.insert(aData, string.char(bit.band(ulAddress, 0xff), bit.band(bit.rshift(ulAddress, 8), 0xff), bit.band(bit.rshift(ulAddress, 16), 0xff), bit.band(bit.rshift(ulAddress, 24), 0xff)))

write_data("read_8bytes_00200000.bin", aData)
