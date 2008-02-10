
module("secmem", package.seeall)


function init_zones()
	local zones = {}

	zones.zone1 = {}
	zones.zone1.abMacAddress = string.char( 0, 0, 0, 0, 0, 0 )
	zones.zone1.ulSdramGeneralCtrl = 0
	zones.zone1.ulSdramTimingCtrl = 0
	zones.zone1.bSdramSizeExp = 0
	zones.zone1.usHwOptions0 = 0
	zones.zone1.usHwOptions1 = 0
	zones.zone1.usHwOptions2 = 0
	zones.zone1.usHwOptions3 = 0
	zones.zone1.bBootMode = 0
	zones.zone1.bZone1Revision = 0

	zones.zone2 = {}
	zones.zone2.usPciVendorID = 0
	zones.zone2.usPciDeviceID = 0
	zones.zone2.bPciSubClassCode = 0
	zones.zone2.bPciClassCode = 0
	zones.zone2.usPciSubSystemVendorID = 0
	zones.zone2.usPciSubSystemDeviceID = 0
	zones.zone2.ulPciSizeTarget = 0
	zones.zone2.bPciSizeIO = 0
	zones.zone2.ulPciSizeROM = 0
	zones.zone2.abOsSettings = string.char( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )
	zones.zone2.bZone2Revision = 0

	zones.zone3 = {}
	zones.zone3.abUserdata = string.char( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

	return zones
end


function bin2zones(bin)
end


function zones2bin(zones)
	local zone
	local zone1
	local zone2
	local zone3


	zone = string.sub( zones.zone1.abMacAddress..string.char( 0, 0, 0, 0, 0, 0 ), 1, 6 )

	zone = zone .. string.char( bit.band(zones.zone1.ulSdramGeneralCtrl, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramGeneralCtrl, 8), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramGeneralCtrl, 16), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramGeneralCtrl, 24), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone1.ulSdramTimingCtrl, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramTimingCtrl, 8), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramTimingCtrl, 16), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.ulSdramTimingCtrl, 24), 0xff) )

	zone = zone .. string.char( zones.zone1.bSdramSizeExp )

	zone = zone .. string.char( bit.band(zones.zone1.usHwOptions0, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.usHwOptions0, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone1.usHwOptions1, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.usHwOptions1, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone1.usHwOptions2, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.usHwOptions2, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone1.usHwOptions3, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone1.usHwOptions3, 8), 0xff) )

	zone = zone .. string.char( zones.zone1.bBootMode )

	zone = zone .. string.char( 0, 0, 0, 0, 0, 0 )

	zone = zone .. string.char( zones.zone1.bZone1Revision )

	zone = zone .. string.char( crc(zone) )

	zone1 = ""
	for cnt=1,32 do
		zone1 = zone1 .. string.char( bit.bxor(zone:byte(cnt), 0xff) )
	end


	zone = string.char( bit.band(zones.zone2.usPciVendorID, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.usPciVendorID, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone2.usPciDeviceID, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.usPciDeviceID, 8), 0xff) )

	zone = zone .. string.char( zones.zone2.bPciSubClassCode )

	zone = zone .. string.char( zones.zone2.bPciClassCode )

	zone = zone .. string.char( bit.band(zones.zone2.usPciSubSystemVendorID, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.usPciSubSystemVendorID, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone2.usPciSubSystemDeviceID, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.usPciSubSystemDeviceID, 8), 0xff) )

	zone = zone .. string.char( bit.band(zones.zone2.ulPciSizeTarget, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.ulPciSizeTarget, 8), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.ulPciSizeTarget, 16), 0xff) )

	zone = zone .. string.char( zones.zone2.bPciSizeIO )

	zone = zone .. string.char( bit.band(zones.zone2.ulPciSizeROM, 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.ulPciSizeROM, 8), 0xff) )
	zone = zone .. string.char( bit.band(bit.bshr(zones.zone2.ulPciSizeROM, 16), 0xff) )

	zone = zone .. string.char( 0 )

	zone = zone .. string.sub(zones.zone2.abOsSettings..string.char( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ), 1, 12 )

	zone = zone .. string.char( zones.zone2.bZone2Revision )

	zone = zone .. string.char( crc(zone) )

	zone2 = ""
	for cnt=1,32 do
		zone2 = zone2 .. string.char( bit.bxor(zone:byte(cnt), 0xff) )
	end


	zone = zones.zone3.abUserdata:sub(1,32)

	zone3 = ""
	for cnt=1,32 do
		zone3 = zone3 .. string.char( bit.bxor(zone:byte(cnt), 0xff) )
	end


	return zone1..zone2..zone3
end


function crc(strArea)
	local uiCrc
	local uiByteCnt
	local uiByte
	local uiBitCnt
	local uiBit


	uiCrc = 0
	for uiByteCnt=1,31 do
		uiByte = strArea:byte(uiByteCnt)
		for uiBitCnt=1,8 do
			uiBit = bit.bxor(uiCrc, uiByte)
			uiBit = bit.band(uiBit, 0x80)
			uiCrc = bit.bshl(uiCrc, 1)
			uiByte = bit.bshl(uiByte, 1)
			if uiBit~=0 then
				uiCrc = bit.bxor(uiCrc, 0x07)
			end
		end
		uiCrc = bit.band(uiCrc, 0xff)
	end

	return uiCrc
end


