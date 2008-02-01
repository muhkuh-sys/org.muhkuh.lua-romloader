-----------------------------------------------------------------------------
--   Copyright (C) 2008 by Christoph Thelen                                --
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

module("mmio", package.seeall)

XM0_IO0            = 0x00
XM0_IO1            = 0x01
XM0_IO2            = 0x02
XM0_IO3            = 0x03
XM0_IO4            = 0x04
XM0_IO5            = 0x05
XM0_RX             = 0x06
XM0_TX_OE          = 0x07
XM0_TX_OUT         = 0x08
XM1_IO0            = 0x09
XM1_IO1            = 0x0a
XM1_IO2            = 0x0b
XM1_IO3            = 0x0c
XM1_IO4            = 0x0d
XM1_IO5            = 0x0e
XM1_RX             = 0x0f
XM1_TX_OE          = 0x10
XM1_TX_OUT         = 0x11
GPIO0              = 0x12
GPIO1              = 0x13
GPIO2              = 0x14
GPIO3              = 0x15
GPIO4              = 0x16
GPIO5              = 0x17
GPIO6              = 0x18
GPIO7              = 0x19
GPIO8              = 0x1a
GPIO9              = 0x1b
GPIO10             = 0x1c
GPIO11             = 0x1d
GPIO12             = 0x1e
GPIO13             = 0x1f
GPIO14             = 0x20
GPIO15             = 0x21
GPIO16             = 0x22
GPIO17             = 0x23
GPIO18             = 0x24
GPIO19             = 0x25
GPIO20             = 0x26
GPIO21             = 0x27
GPIO22             = 0x28
GPIO23             = 0x29
GPIO24             = 0x2a
GPIO25             = 0x2b
GPIO26             = 0x2c
GPIO27             = 0x2d
GPIO28             = 0x2e
GPIO29             = 0x2f
GPIO30             = 0x30
GPIO31             = 0x31
PHY0_LED0          = 0x32
PHY0_LED1          = 0x33
PHY0_LED2          = 0x34
PHY0_LED3          = 0x35
PHY1_LED0          = 0x36
PHY1_LED1          = 0x37
PHY1_LED2          = 0x38
PHY1_LED3          = 0x39
MII_MDC            = 0x3a
MII_MDIO           = 0x3b
MII0_COL           = 0x3c
MII0_CRS           = 0x3d
MII0_LED0          = 0x3e
MII0_LED1          = 0x3f
MII0_LED2          = 0x40
MII0_LED3          = 0x41
MII0_RXCLK         = 0x42
MII0_RXD0          = 0x43
MII0_RXD1          = 0x44
MII0_RXD2          = 0x45
MII0_RXD3          = 0x46
MII0_RXDV          = 0x47
MII0_RXER          = 0x48
MII0_TXCLK         = 0x49
MII0_TXD0          = 0x4a
MII0_TXD1          = 0x4b
MII0_TXD2          = 0x4c
MII0_TXD3          = 0x4d
MII0_TXEN          = 0x4e
MII0_TXER          = 0x4f
MII1_COL           = 0x50
MII1_CRS           = 0x51
MII1_LED0          = 0x52
MII1_LED1          = 0x53
MII1_LED2          = 0x54
MII1_LED3          = 0x55
MII1_RXCLK         = 0x56
MII1_RXD0          = 0x57
MII1_RXD1          = 0x58
MII1_RXD2          = 0x59
MII1_RXD3          = 0x5a
MII1_RXDV          = 0x5b
MII1_RXER          = 0x5c
MII1_TXCLK         = 0x5d
MII1_TXD0          = 0x5e
MII1_TXD1          = 0x5f
MII1_TXD2          = 0x60
MII1_TXD3          = 0x61
MII1_TXEN          = 0x62
MII1_TXER          = 0x63
PIO0               = 0x64
PIO1               = 0x65
PIO2               = 0x66
PIO3               = 0x67
PIO4               = 0x68
PIO5               = 0x69
PIO6               = 0x6a
PIO7               = 0x6b
SPI0_CS2N          = 0x6c
SPI1_CLK           = 0x6d
SPI1_CS0N          = 0x6e
SPI1_CS1N          = 0x6f
SPI1_CS2N          = 0x70
SPI1_MISO          = 0x71
SPI1_MOSI          = 0x72
I2C_SCL_MMIO       = 0x73
I2C_SDA_MMIO       = 0x74
XC_SAMPLE0         = 0x75
XC_SAMPLE1         = 0x76
XC_TRIGGER0        = 0x77
XC_TRIGGER1        = 0x78
UART0_CTS          = 0x79
UART0_RTS          = 0x7a
UART0_RXD          = 0x7b
UART0_TXD          = 0x7c
UART1_CTS          = 0x7d
UART1_RTS          = 0x7e
UART1_RXD          = 0x7f
UART1_TXD          = 0x80
UART2_CTS          = 0x81
UART2_RTS          = 0x82
UART2_RXD          = 0x83
UART2_TXD          = 0x84
USB_ID_DIG         = 0x85
USB_ID_PULLUP_CTRL = 0x86
USB_RPD_ENA        = 0x87
USB_RPU_ENA        = 0x88
CCD_DATA0          = 0x89
CCD_DATA1          = 0x8a
CCD_DATA2          = 0x8b
CCD_DATA3          = 0x8c
CCD_DATA4          = 0x8d
CCD_DATA5          = 0x8e
CCD_DATA6          = 0x8f
CCD_DATA7          = 0x90
CCD_PIXCLK         = 0x91
CCD_LINE_VALID     = 0x92
CCD_FRAME_VALID    = 0x93
INPUT              = 0xff

local pin_names =
{
	[XM0_IO0]		= "XM0_IO0",
	[XM0_IO1]		= "XM0_IO1",
	[XM0_IO2]		= "XM0_IO2",
	[XM0_IO3]		= "XM0_IO3",
	[XM0_IO4]		= "XM0_IO4",
	[XM0_IO5]		= "XM0_IO5",
	[XM0_RX]		= "XM0_RX",
	[XM0_TX_OE]		= "XM0_TX_OE",
	[XM0_TX_OUT]		= "XM0_TX_OUT",
	[XM1_IO0]		= "XM1_IO0",
	[XM1_IO1]		= "XM1_IO1",
	[XM1_IO2]		= "XM1_IO2",
	[XM1_IO3]		= "XM1_IO3",
	[XM1_IO4]		= "XM1_IO4",
	[XM1_IO5]		= "XM1_IO5",
	[XM1_RX]		= "XM1_RX",
	[XM1_TX_OE]		= "XM1_TX_OE",
	[XM1_TX_OUT]		= "XM1_TX_OUT",
	[GPIO0]			= "GPIO0",
	[GPIO1]			= "GPIO1",
	[GPIO2]			= "GPIO2",
	[GPIO3]			= "GPIO3",
	[GPIO4]			= "GPIO4",
	[GPIO5]			= "GPIO5",
	[GPIO6]			= "GPIO6",
	[GPIO7]			= "GPIO7",
	[GPIO8]			= "GPIO8",
	[GPIO9]			= "GPIO9",
	[GPIO10]		= "GPIO10",
	[GPIO11]		= "GPIO11",
	[GPIO12]		= "GPIO12",
	[GPIO13]		= "GPIO13",
	[GPIO14]		= "GPIO14",
	[GPIO15]		= "GPIO15",
	[GPIO16]		= "GPIO16",
	[GPIO17]		= "GPIO17",
	[GPIO18]		= "GPIO18",
	[GPIO19]		= "GPIO19",
	[GPIO20]		= "GPIO20",
	[GPIO21]		= "GPIO21",
	[GPIO22]		= "GPIO22",
	[GPIO23]		= "GPIO23",
	[GPIO24]		= "GPIO24",
	[GPIO25]		= "GPIO25",
	[GPIO26]		= "GPIO26",
	[GPIO27]		= "GPIO27",
	[GPIO28]		= "GPIO28",
	[GPIO29]		= "GPIO29",
	[GPIO30]		= "GPIO30",
	[GPIO31]		= "GPIO31",
	[PHY0_LED0]		= "PHY0_LED0",
	[PHY0_LED1]		= "PHY0_LED1",
	[PHY0_LED2]		= "PHY0_LED2",
	[PHY0_LED3]		= "PHY0_LED3",
	[PHY1_LED0]		= "PHY1_LED0",
	[PHY1_LED1]		= "PHY1_LED1",
	[PHY1_LED2]		= "PHY1_LED2",
	[PHY1_LED3]		= "PHY1_LED3",
	[MII_MDC]		= "MII_MDC",
	[MII_MDIO]		= "MII_MDIO",
	[MII0_COL]		= "MII0_COL",
	[MII0_CRS]		= "MII0_CRS",
	[MII0_LED0]		= "MII0_LED0",
	[MII0_LED1]		= "MII0_LED1",
	[MII0_LED2]		= "MII0_LED2",
	[MII0_LED3]		= "MII0_LED3",
	[MII0_RXCLK]		= "MII0_RXCLK",
	[MII0_RXD0]		= "MII0_RXD0",
	[MII0_RXD1]		= "MII0_RXD1",
	[MII0_RXD2]		= "MII0_RXD2",
	[MII0_RXD3]		= "MII0_RXD3",
	[MII0_RXDV]		= "MII0_RXDV",
	[MII0_RXER]		= "MII0_RXER",
	[MII0_TXCLK]		= "MII0_TXCLK",
	[MII0_TXD0]		= "MII0_TXD0",
	[MII0_TXD1]		= "MII0_TXD1",
	[MII0_TXD2]		= "MII0_TXD2",
	[MII0_TXD3]		= "MII0_TXD3",
	[MII0_TXEN]		= "MII0_TXEN",
	[MII0_TXER]		= "MII0_TXER",
	[MII1_COL]		= "MII1_COL",
	[MII1_CRS]		= "MII1_CRS",
	[MII1_LED0]		= "MII1_LED0",
	[MII1_LED1]		= "MII1_LED1",
	[MII1_LED2]		= "MII1_LED2",
	[MII1_LED3]		= "MII1_LED3",
	[MII1_RXCLK]		= "MII1_RXCLK",
	[MII1_RXD0]		= "MII1_RXD0",
	[MII1_RXD1]		= "MII1_RXD1",
	[MII1_RXD2]		= "MII1_RXD2",
	[MII1_RXD3]		= "MII1_RXD3",
	[MII1_RXDV]		= "MII1_RXDV",
	[MII1_RXER]		= "MII1_RXER",
	[MII1_TXCLK]		= "MII1_TXCLK",
	[MII1_TXD0]		= "MII1_TXD0",
	[MII1_TXD1]		= "MII1_TXD1",
	[MII1_TXD2]		= "MII1_TXD2",
	[MII1_TXD3]		= "MII1_TXD3",
	[MII1_TXEN]		= "MII1_TXEN",
	[MII1_TXER]		= "MII1_TXER",
	[PIO0]			= "PIO0",
	[PIO1]			= "PIO1",
	[PIO2]			= "PIO2",
	[PIO3]			= "PIO3",
	[PIO4]			= "PIO4",
	[PIO5]			= "PIO5",
	[PIO6]			= "PIO6",
	[PIO7]			= "PIO7",
	[SPI0_CS2N]		= "SPI0_CS2N",
	[SPI1_CLK]		= "SPI1_CLK",
	[SPI1_CS0N]		= "SPI1_CS0N",
	[SPI1_CS1N]		= "SPI1_CS1N",
	[SPI1_CS2N]		= "SPI1_CS2N",
	[SPI1_MISO]		= "SPI1_MISO",
	[SPI1_MOSI]		= "SPI1_MOSI",
	[I2C_SCL_MMIO]		= "I2C_SCL_MMIO",
	[I2C_SDA_MMIO]		= "I2C_SDA_MMIO",
	[XC_SAMPLE0]		= "XC_SAMPLE0",
	[XC_SAMPLE1]		= "XC_SAMPLE1",
	[XC_TRIGGER0]		= "XC_TRIGGER0",
	[XC_TRIGGER1]		= "XC_TRIGGER1",
	[UART0_CTS]		= "UART0_CTS",
	[UART0_RTS]		= "UART0_RTS",
	[UART0_RXD]		= "UART0_RXD",
	[UART0_TXD]		= "UART0_TXD",
	[UART1_CTS]		= "UART1_CTS",
	[UART1_RTS]		= "UART1_RTS",
	[UART1_RXD]		= "UART1_RXD",
	[UART1_TXD]		= "UART1_TXD",
	[UART2_CTS]		= "UART2_CTS",
	[UART2_RTS]		= "UART2_RTS",
	[UART2_RXD]		= "UART2_RXD",
	[UART2_TXD]		= "UART2_TXD",
	[USB_ID_DIG]		= "USB_ID_DIG",
	[USB_ID_PULLUP_CTRL]	= "USB_ID_PULLUP_CTRL",
	[USB_RPD_ENA]		= "USB_RPD_ENA",
	[USB_RPU_ENA]		= "USB_RPU_ENA",
	[CCD_DATA0]		= "CCD_DATA0",
	[CCD_DATA1]		= "CCD_DATA1",
	[CCD_DATA2]		= "CCD_DATA2",
	[CCD_DATA3]		= "CCD_DATA3",
	[CCD_DATA4]		= "CCD_DATA4",
	[CCD_DATA5]		= "CCD_DATA5",
	[CCD_DATA6]		= "CCD_DATA6",
	[CCD_DATA7]		= "CCD_DATA7",
	[CCD_PIXCLK]		= "CCD_PIXCLK",
	[CCD_LINE_VALID]	= "CCD_LINE_VALID",
	[CCD_FRAME_VALID]	= "CCD_FRAME_VALID",
	[INPUT]			= "INPUT"
}

---------------------------------------
-- init
--   Init a variable with a valid mmio table.
--
-- Parameters:
--   none
--
-- Return:
--   mmio table
--
function init()
	local t

	-- create an empty array
	t = {}
	-- init all mmio entries with INPUT
	for i=0,39 do
		t[i] = { func=INPUT, fInvertIn=false, fInvertOut=false }
	end

	-- return the array
	return t
end


---------------------------------------
-- show
--   Prints the contents of a mmio table.
--
-- Parameters:
--   t: mmio table
--
-- Return:
--   nothing
--
-- Notes:
--   The 't' parameter must be an initialized mmio table (see function 'init')
--
function show(t)
	local name
	local func


	for i=0,39 do
		func = t[i].func
		name = pin_names[func]
		if not name then
			name = "??? unknown setting: " .. func
		end
		print(string.format("%02d",i) .. " = " .. name .. ", InvIn=" .. tostring(t[i].fInvertIn) .. ", InvOut=" .. tostring(t[i].fInvertOut))
	end
end


---------------------------------------
-- setConfig
--   Write a mmio table to a netX device.
--
-- Parameters:
--   plugin: romloader plugin to use for netX communication
--   t : mmio table, write the values from this table to the netX
--
-- Return:
--   nothing
--
-- Notes:
--   1) The 'plugin' parameter must be a romloader plugin. The connection
--      must be open.
--   2) The 't' parameter must be an initialized mmio table (see function 'init')
--
function setConfig(plugin, t)
	local key
	local val


	for i=0,39 do
		-- unlock access key (that's at address 0x1c000070)
		key = plugin:read_data32(0x1c000070)
		plugin:write_data32(0x1c000070, key)
		-- set the config for the pin
		val = t[i].func + (t[i].fInvertOut and 0x0100 or 0) + (t[i].fInvertIn and 0x0200 or 0)
--		print("Write " .. string.format("0x%08X", val) .. " to pin " .. i)
		plugin:write_data32(0x1c001300 + i*4, val)
	end
end


---------------------------------------
-- getConfig
--   Read the mmio table from a netX device.
--
-- Parameters:
--   plugin: romloader plugin to use for netX communication
--   t : mmio table, write the mmio table from the netX to this table
--
-- Return:
--   nothing
--
-- Notes:
--   1) The 'plugin' parameter must be a romloader plugin. The connection
--      must be open.
--   2) The 't' parameter must be an initialized mmio table (see function 'init')
--
function getConfig(plugin, t)
	local val


	for i=0,39 do
		val = plugin:read_data32(0x1c001300 + i*4)
		t[i].func = bit.band(val, 0xff)
		t[i].fInvertOut = (bit.band(val,0x0100) ~= 0)
		t[i].fInvertIn = (bit.band(val,0x0200) ~= 0)
	end
end

