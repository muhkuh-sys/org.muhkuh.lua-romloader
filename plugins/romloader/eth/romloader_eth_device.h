/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen and M. Trensch                 *
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

#ifndef __ROMLOADER_ETH_DEVICE_H__
#define __ROMLOADER_ETH_DEVICE_H__


#include "../romloader.h"

#ifdef _WINDOWS
#       include <windows.h>
#       define PATH_MAX 259
#endif


#if !defined(SWIG)

/* NOTE: Use "pragma pack" instead of "attribute packed" as the latter does not work on MinGW.
 *       See here for details: https://sourceforge.net/p/mingw-w64/bugs/588/
 */
#pragma pack(push, 1)

struct MIV3_ETHERNET_DISCOVER_MAGIC_STRUCTURE
{
	uint8_t ucMiStatus;
	char acMagic[4];
	uint16_t usVersionMinor;
	uint16_t usVersionMajor;
	uint16_t usMaximumPacketSize;
	uint32_t ulIP;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_ETHERNET_DISCOVER_MAGIC_STRUCTURE)==15, "Packing of MIV3_ETHERNET_DISCOVER_MAGIC_STRUCTURE does not work.");

typedef union MIV3_ETHERNET_DISCOVER_MAGIC_UNION
{
	struct MIV3_ETHERNET_DISCOVER_MAGIC_STRUCTURE s;
	unsigned char auc[15];
} MIV3_ETHERNET_DISCOVER_MAGIC_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_ETHERNET_DISCOVER_MAGIC_T)==15, "Packing of MIV3_ETHERNET_DISCOVER_MAGIC_T does not work.");

#pragma pack(pop)

#endif  /* !defined(SWIG) */


class romloader_eth_device
{
public:
	romloader_eth_device(const char *pcServerName);
	~romloader_eth_device(void);

	/* low level interface is platform specific */
	virtual bool Open(void) = 0;
	virtual void Close(void) = 0;
	virtual int ExecuteCommand(const unsigned char *aucCommand, size_t sizCommand, unsigned char *aucResponse, size_t sizResponse, size_t *psizResponse) = 0;
	virtual romloader::TRANSPORTSTATUS_T SendPacket(const void *pvData, size_t sizData) = 0;
	virtual romloader::TRANSPORTSTATUS_T RecvPacket(unsigned char *pucData, size_t sizData, unsigned long ulTimeout, size_t *psizPacket) = 0;


protected:
	char *m_pcServerName;
	static const unsigned short m_usHBootServerPort = 53281;


private:
};


#endif  /* __ROMLOADER_UART_DEVICE_H__ */

