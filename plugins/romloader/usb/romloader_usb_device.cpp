/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "romloader_usb_device.h"

#include <errno.h>


#if defined(WIN32)
#       define strncasecmp _strnicmp
#endif

const romloader_usb_device::NETX_USB_DEVICE_T romloader_usb_device::atNetxUsbDevices[8] =
{
	{
		"netX500 V1",
		0x0cc4,
		0x0815,
		0x0100,
		ROMLOADER_CHIPTYP_NETX500,
		ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1,
		1,
		0,
		0x81,
		0x01,
		true
	},
	{
		"netX500 HBoot Emulation",
		0x1939,
		0x0015,
		0x0001,
		ROMLOADER_CHIPTYP_NETX500,
		ROMLOADER_COMMANDSET_MI2,
		1,
		0,
		0x81,
		0x01,
		true
	},
	{
		"netX10 V1",
		0x1939,
		0x000c,
		0x0001,
		ROMLOADER_CHIPTYP_NETX10,
		ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1,
		1,
		0,
		0x83,
		0x04,
		false
	},
	{
		"netX10 HBoot Emulation",
		0x1939,
		0x000c,
		0x0002,
		ROMLOADER_CHIPTYP_NETX10,
		ROMLOADER_COMMANDSET_MI2,
		1,
		0,
		0x83,
		0x04,
		false
	},
	{
		"netX51",
		0x1939,
		0x0018,
		0x0001,
		ROMLOADER_CHIPTYP_NETX56,
		ROMLOADER_COMMANDSET_MI1,
		0,
		1,
		0x85,
		0x04,
		true
	},
	{
		"netX51 HBoot Emulation",
		0x1939,
		0x0018,
		0x0002,
		ROMLOADER_CHIPTYP_NETX56,
		ROMLOADER_COMMANDSET_MI2,
		0,
		1,
		0x85,
		0x04,
		true
	},
	{
		"netX52",
		0x1939,
		0x0019,
		0x0001,
		ROMLOADER_CHIPTYP_NETX56,
		ROMLOADER_COMMANDSET_MI1,
		0,
		1,
		0x85,
		0x04,
		true
	},
	{
		"netX52 HBoot Emulation",
		0x1939,
		0x0019,
		0x0002,
		ROMLOADER_CHIPTYP_NETX56,
		ROMLOADER_COMMANDSET_MI2,
		0,
		1,
		0x85,
		0x04,
		true
	}
};



romloader_usb_device::romloader_usb_device(const char *pcPluginId)
 : m_pcPluginId(NULL)
{
	m_pcPluginId = strdup(pcPluginId);
}


romloader_usb_device::~romloader_usb_device(void)
{
	if( m_pcPluginId!=NULL )
	{
		free(m_pcPluginId);
	}
}


void romloader_usb_device::hexdump(const unsigned char *pucData, unsigned long ulSize)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = 0;
	while( pucDumpCnt<pucDumpEnd )
	{
		// get number of bytes for the next line
		sizChunkSize = 16;
		sizBytesLeft = pucDumpEnd - pucDumpCnt;
		if( sizChunkSize>sizBytesLeft )
		{
			sizChunkSize = sizBytesLeft;
		}

		// start a line in the dump with the address
		printf("%08lX: ", ulAddressCnt);
		// append the data bytes
		sizChunkCnt = sizChunkSize;
		while( sizChunkCnt!=0 )
		{
			printf("%02X ", *(pucDumpCnt++));
			--sizChunkCnt;
		}
		// next line
		printf("\n");
		ulAddressCnt += sizChunkSize;
	}
}



uint16_t romloader_usb_device::crc16(uint16_t usCrc, uint8_t ucData)
{
	usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
	usCrc ^= ucData;
	usCrc ^= (usCrc & 0xff) >> 4;
	usCrc ^= (usCrc & 0x0f) << 12;
	usCrc ^= ((usCrc & 0xff) << 4) << 1;

	return usCrc;
}
