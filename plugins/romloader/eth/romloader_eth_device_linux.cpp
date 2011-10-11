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


#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


#include "romloader_eth_device_linux.h"
#include "machine_interface_commands.h"


romloader_eth_device_linux::romloader_eth_device_linux(const char *pcServerName)
  : romloader_eth_device(pcServerName)
{
	/* No socket yet. */
	m_iHbootServer_Socket = -1;

	/* Clear the server address. */
	memset(&m_tHbootServer_Addr, 0, sizeof(m_tHbootServer_Addr));

}


romloader_eth_device_linux::~romloader_eth_device_linux(void)
{
	Close();
}


bool romloader_eth_device_linux::Open(void)
{
	bool fOk;
	struct in_addr tServerAddress;
	int iResult;
	const unsigned short usHBootServerPort = 53280;
	int iServerSocket;


	iResult = inet_pton(AF_INET, m_pcServerName, &tServerAddress);
	if( iResult!=1 )
	{
		fprintf(stderr, "Failed to convert the servername '%s' to an IP!\n", m_pcServerName);
		fOk = false;
	}
	else
	{
		/* Open a socket. */
		iServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if( iServerSocket==-1 )
		{
			fprintf(stderr, "Failed to create socket!\n");
			fOk = false;
		}
		else
		{
			/* Get the address of the HBoot server. */
			memset(&m_tHbootServer_Addr, 0, sizeof(m_tHbootServer_Addr));
			m_tHbootServer_Addr.tAddrIn.sin_family = AF_INET;
			m_tHbootServer_Addr.tAddrIn.sin_port = htons(usHBootServerPort);
			memcpy(&m_tHbootServer_Addr.tAddrIn.sin_addr, &tServerAddress, sizeof(struct in_addr));

			/* Copy the server socket handle. */
			m_iHbootServer_Socket = iServerSocket;

			fOk = true;
		}
	}

	return fOk;
}


void romloader_eth_device_linux::Close(void)
{
	/* Shutdown the socket. */
	shutdown(m_iHbootServer_Socket, SHUT_RDWR);

	/* Close the socket. */
	close(m_iHbootServer_Socket);
}


int romloader_eth_device_linux::SendPacket(const unsigned char *pucData, size_t sizData)
{
	ssize_t ssizResult;
	int iResult;


	/* Expect success. */
	iResult = 0;

	/* Send a packet. */
	ssizResult = sendto(m_iHbootServer_Socket, pucData, sizData, 0, &m_tHbootServer_Addr.tAddr, sizeof(m_tHbootServer_Addr));
	if( ssizResult==-1 )
	{
		fprintf(stderr, "Failed to send packet: %d: %s\n", errno, strerror(errno));
		iResult = -1;
	}
	if( ssizResult!=sizData )
	{
		fprintf(stderr, "Failed to send packet. %d requested, but only %d sent.\n", sizData, ssizResult);
		iResult = -1;
	}

	return iResult;
}


int romloader_eth_device_linux::RecvPacket(unsigned char *pucData, size_t sizData, unsigned long ulTimeout, size_t *psizPacket)
{
	fd_set tRxFileDescSet;
	struct timeval tTimeVal;
	int iResult;
	ssize_t ssizPacket;


	/* Watch the socket to see when it has input. */
	FD_ZERO(&tRxFileDescSet);
	FD_SET(m_iHbootServer_Socket, &tRxFileDescSet);

	/* Wait 0.01 second each until no more data arrives or 100 packets were received. */
	tTimeVal.tv_sec = 0;
	tTimeVal.tv_usec = 10000;

	/* Nothing received yet. */
	ssizPacket = 0;

	iResult = select(m_iHbootServer_Socket+1, &tRxFileDescSet, NULL, NULL, &tTimeVal);
	if( iResult==0 )
	{
		/* Timeout and nothing received. */
	}
	else if( iResult==1 )
	{
		/* Receive the answer. */
		ssizPacket = recvfrom(m_iHbootServer_Socket, pucData, sizData, 0, NULL, 0);
		if( ssizPacket<0 )
		{
			fprintf(stderr, "Failed to receive packet.\n");
			ssizPacket = 0;
			iResult = -1;
		}
		else
		{
			/* All ok! */
			iResult = 0;
		}
	}
	else
	{
		fprintf(stderr, "Failed to wait for data.\n");
	}

	/* Return the packet's size. */
	*psizPacket = (size_t)ssizPacket;

	return iResult;
}


size_t romloader_eth_device_linux::ScanForServers(char ***pppcDeviceNames)
{
	char **ppcDeviceNames;
	char *pcDeviceName;
	size_t sizCnt;
	size_t sizDevices;
	int tSocket;
	int socklen;
	unsigned char aucBuffer[MI_ETH_MAX_PACKET_SIZE];
	struct sockaddr_in saddr;
	struct in_addr iaddr;
	unsigned char ttl = 3;
	fd_set rfds;
	struct timeval tv;
	int iResult;
	int iCnt;
	union
	{
		struct sockaddr tAddr;
		struct sockaddr_in tAddrIn;
	} uSrcAddr;
	socklen_t tAddrLen;
	unsigned int uiVersionMaj;
	unsigned int uiVersionMin;
	unsigned long ulIp;
	ssize_t ssizPacket;
	size_t sizRefCnt;
	size_t sizRefMax;
	char **ppcRef;
	char **ppcRefNew;
	size_t sizEntry;
	char *pcRef;
	const unsigned char aucMagic[5] = { 0x00, 'M', 'O', 'O', 'H' };


	/* Init References array. */
	sizRefCnt = 0;
	sizRefMax = 16;
	ppcRef = (char**)malloc(sizRefMax*sizeof(char*));
	if( ppcRef==NULL )
	{
		fprintf(stderr, "out of memory!\n");
	}
	else
	{
		/* Set content of struct saddr and imreq to zero. */
		memset(&saddr, 0, sizeof(struct sockaddr_in));
		memset(&iaddr, 0, sizeof(struct in_addr));

		/* Open a UDP socket. */
		tSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if( tSocket<0 )
		{
			fprintf(stderr, "Error creating socket");
		}
		else
		{
			/* Bind the socket to any interface and use the first free port . */
			saddr.sin_family = PF_INET;
			saddr.sin_port = htons(0);
			saddr.sin_addr.s_addr = htonl(INADDR_ANY);
			iResult = bind(tSocket, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
			if( iResult<0 )
			{
				fprintf(stderr, "Error binding socket to interface");
			}
			else
			{
				// use DEFAULT interface
				iaddr.s_addr = INADDR_ANY;

				// Set the outgoing interface to DEFAULT
				setsockopt(tSocket, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(struct in_addr));

				// Set multicast packet TTL to 3; default TTL is 1
				setsockopt(tSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(unsigned char));

				// set destination multicast address
				saddr.sin_family = PF_INET;
				saddr.sin_addr.s_addr = inet_addr("224.0.0.251");
				saddr.sin_port = htons(53280);

				// put some data in buffer
				memcpy(aucBuffer, "hello", 6);

				socklen = sizeof(struct sockaddr_in);
				// send packet
				iResult = sendto(tSocket, aucBuffer, 6, 0, (struct sockaddr *)&saddr, socklen);

				iCnt = 100;
				do
				{
					/* Watch the socket to see when it has input. */
					FD_ZERO(&rfds);
					FD_SET(tSocket, &rfds);

					/* Wait 0.01 second each until no more data arrives or 100 packets were received. */
					tv.tv_sec = 0;
					tv.tv_usec = 10000;

					iResult = select(tSocket+1, &rfds, NULL, NULL, &tv);
					if( iResult==-1 )
					{
						fprintf(stderr, "Failed to wait for data.\n");
						break;
					}
					else if( iResult==1 )
					{
						ssizPacket = recvfrom(tSocket, aucBuffer, MI_ETH_MAX_PACKET_SIZE, 0, &uSrcAddr.tAddr, &tAddrLen);
						if( ssizPacket==-1 )
						{
							fprintf(stderr, "Failed to receive packet.\n");
							iResult = -1;
						}
						else if(
							ssizPacket>=9 &&
							memcmp(aucBuffer, aucMagic, sizeof(aucMagic))==0
						       )
						{
							uiVersionMin = aucBuffer[5] | aucBuffer[6]<<8U;
							uiVersionMaj = aucBuffer[7] | aucBuffer[8]<<8U;

							ulIp = uSrcAddr.tAddrIn.sin_addr.s_addr;
							printf("Found HBoot V%d.%d at %ld.%ld.%ld.%ld .\n", uiVersionMaj, uiVersionMin, ulIp&0xffU, (ulIp>>8U)&0xffU, (ulIp>>16U)&0xffU, (ulIp>>24U)&0xffU);

							/* Is enough space in the array for one more entry? */
							if( sizRefCnt>=sizRefMax )
							{
								/* No -> expand the array. */
								sizRefMax *= 2;
								/* Detect overflow or limitation. */
								if( sizRefMax<=sizRefCnt )
								{
									iResult = -1;
									break;
								}
								/* Reallocate the array. */
								ppcRefNew = (char**)realloc(ppcRef, sizRefMax*sizeof(char*));
								if( ppcRefNew==NULL )
								{
									iResult = -1;
									break;
								}
								ppcRef = ppcRefNew;
							}
							sizEntry = strlen("romloader_eth_xxx.xxx.xxx.xxx") + 1;
							pcRef = (char*)malloc(sizEntry);
							if( pcRef==NULL )
							{
								break;
							}
							snprintf(pcRef, sizEntry, "romloader_eth_%ld.%ld.%ld.%ld", ulIp&0xffU, (ulIp>>8U)&0xffU, (ulIp>>16U)&0xffU, (ulIp>>24U)&0xffU);
							ppcRef[sizRefCnt++] = pcRef;
						}
					}

					--iCnt;
				} while( iCnt>=0 );
			}

			/* Shutdown the socket. */
			shutdown(tSocket, SHUT_RDWR);

			/* Close the socket. */
			close(tSocket);
		}
	}

	*pppcDeviceNames = ppcRef;

	return sizRefCnt;
}


