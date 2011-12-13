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


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


#ifndef __ROMLOADER_ETH_DEVICE_LINUX_H__
#define __ROMLOADER_ETH_DEVICE_LINUX_H__

#include "romloader_eth_device.h"


class romloader_eth_device_linux : public romloader_eth_device
{
public:
	romloader_eth_device_linux(const char *pcServerName);
	virtual ~romloader_eth_device_linux();

	bool Open(void);
	void Close(void);
	int ExecuteCommand(const unsigned char *aucCommand, size_t sizCommand, unsigned char *aucResponse, size_t sizResponse, size_t *psizResponse);
	int SendPacket(const unsigned char *pucData, size_t sizData);
	int RecvPacket(unsigned char *pucData, size_t sizData, unsigned long ulTimeout, size_t *psizPacket);

	static size_t ScanForServers(char ***pppcPortNames);

private:
	bool open_by_addr(struct in_addr *ptServerAddress);

	int m_iHbootServer_Socket;
	union
	{
		struct sockaddr tAddr;
		struct sockaddr_in tAddrIn;
	} m_tHbootServer_Addr;

};


#endif  /* __ROMLOADER_ETH_DEVICE_LINUX_H__ */
