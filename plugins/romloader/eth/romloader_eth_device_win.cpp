/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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


#include "romloader_eth_device_win.h"
#include "machine_interface_commands.h"
#include <iptypes.h>
#include <iphlpapi.h>

#if defined(_MSC_VER)
#       define snprintf _snprintf
#endif


romloader_eth_device_win::romloader_eth_device_win(const char *pcServerName)
  : romloader_eth_device(pcServerName)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int iErr;


	/* No socket yet. */
	m_tHbootServer_Socket = INVALID_SOCKET;

	/* Clear the server address. */
	memset(&m_tHbootServer_Addr, 0, sizeof(m_tHbootServer_Addr));

	/* Initialize the Windows Socket Architecture. */
	wVersionRequested = MAKEWORD(2, 2);
	iErr = WSAStartup(wVersionRequested, &wsaData);
	fprintf(stderr, "WSAStartup: %d\n", iErr);
}


romloader_eth_device_win::~romloader_eth_device_win(void)
{
	Close();
}


bool romloader_eth_device_win::Open(void)
{
	bool fOk;
	unsigned long ulIp;
	int iResult;
	hostent *tHost;


//	iResult = inet_pton(AF_INET, m_pcServerName, &tServerAddress);
//	if( iResult!=1 )
//	{
//		fprintf(stderr, "Failed to convert the servername '%s' to an IP!\n", m_pcServerName);
//		fOk = false;
//	}

	tHost = gethostbyname(m_pcServerName);
	if( tHost==NULL )
	{
		fprintf(stderr, "Failed to convert the servername '%s' to an IP!\n", m_pcServerName);
		fOk = false;
	}
	else
	{
		ulIp = *((unsigned long*)tHost->h_addr_list[0]);
		fOk = open_by_addr(ulIp);
	}

	return fOk;
}


bool romloader_eth_device_win::open_by_addr(unsigned long ulServerIp)
{
	bool fOk;
	SOCKET tServerSocket;


	/* Open a socket. */
	tServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( tServerSocket==INVALID_SOCKET )
	{
		fprintf(stderr, "Failed to create socket!\n");
		fOk = false;
	}
	else
	{
		/* Get the address of the HBoot server. */
		memset(&m_tHbootServer_Addr, 0, sizeof(m_tHbootServer_Addr));
		m_tHbootServer_Addr.tAddrIn.sin_family = AF_INET;
		m_tHbootServer_Addr.tAddrIn.sin_port = htons(m_usHBootServerPort);
		m_tHbootServer_Addr.tAddrIn.sin_addr.s_addr = ulServerIp;

		/* Copy the server socket handle. */
		m_tHbootServer_Socket = tServerSocket;

		fOk = true;
	}

	return fOk;
}


void romloader_eth_device_win::Close(void)
{
	/* Shutdown the socket. */
	shutdown(m_tHbootServer_Socket, SD_BOTH);

	/* Close the socket. */
	closesocket(m_tHbootServer_Socket);
}


int romloader_eth_device_win::SendPacket(const void *pvData, size_t sizData)
{
	int iResult;
	int iSendResult;


	/* Expect success. */
	iResult = 0;

	/* Send a packet. */
	iSendResult = sendto(m_tHbootServer_Socket, (const char*)pvData, sizData, 0, (SOCKADDR *)&m_tHbootServer_Addr.tAddr, sizeof(m_tHbootServer_Addr));
	if( iSendResult==SOCKET_ERROR )
	{
		fprintf(stderr, "Failed to send packet: %d: %s\n", errno, strerror(errno));
	}
	else if( iSendResult!=sizData )
	{
		fprintf(stderr, "Failed to send packet. %d requested, but only %d sent.\n", sizData, iResult);
		iResult = -1;
	}

	return iResult;
}


int romloader_eth_device_win::RecvPacket(unsigned char *pucData, size_t sizData, unsigned long ulTimeout, size_t *psizPacket)
{
	fd_set tRxFileDescSet;
	struct timeval tTimeVal;
	int iResult;
	int iPacketSize;


	/* Watch the socket to see when it has input. */
	FD_ZERO(&tRxFileDescSet);
	FD_SET(m_tHbootServer_Socket, &tRxFileDescSet);

	/* Wait 1 second for a new packet. */
	tTimeVal.tv_sec = 1;
	tTimeVal.tv_usec = 0;

	/* Nothing received yet. */
	iPacketSize = 0;

	iResult = select(m_tHbootServer_Socket+1, &tRxFileDescSet, NULL, NULL, &tTimeVal);
	if( iResult==0 )
	{
		/* Timeout and nothing received. */
		fprintf(stderr, "timeout\n");
	}
	else if( iResult==1 )
	{
		/* Receive the answer. */
		iPacketSize = recvfrom(m_tHbootServer_Socket, (char*)pucData, sizData, 0, NULL, 0);
		if( iPacketSize<0 )
		{
			fprintf(stderr, "Failed to receive packet.\n");
			iPacketSize = 0;
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
	*psizPacket = (size_t)iPacketSize;

	return iResult;
}



void romloader_eth_device_win::ScanAdapter(ROMLOADER_ETH_REFLIST_T* ptRefList, unsigned long ulInetAddr)
{
	SOCKET tSocket;

	/* used for interface and destination multicast address */
	struct sockaddr_in saddr;
	/* sizeof sockaddr_in */
	int socklen;

	/* used to configure the socket */
	union
	{
		struct in_addr iaddr;
		unsigned char ttl;
		char ac[sizeof(struct in_addr)];
	} uSocketOptions;

	/* */
	fd_set rfds;

	/* receive timeout */
	struct timeval tv;

	/* packet buffer for send/receive */
	unsigned char aucBuffer[MI_ETH_MAX_PACKET_SIZE];

	/* size of received packet */
	int iPacketSize;

	/* counter for received packets */
	int iCnt;

	/* source address of incoming reply packets to the multicast */
	union
	{
		struct sockaddr tAddr;
		struct sockaddr_in tAddrIn;
	} uSrcAddr;

	/* size of tAddrIn */
	int iAddrLen;

	/* signature */
	const unsigned char aucMagic[5] = { 0x00, 'M', 'O', 'O', 'H' };

	/* fields extracted from reply packet */
	unsigned int uiVersionMin;
	unsigned int uiVersionMaj;
	size_t sizMaxPacket;
	unsigned long ulIp;

	/* number of actually created plugin references */
	size_t sizRefCnt;
	/* capacity of the references array*/
	size_t sizRefMax;
	/* pointer to references array */
	char **ppcRef;
	/* tmp for reallocation */
	char **ppcRefNew;
	/* size of a single reference (strlen("romloader_eth_xxx.xxx.xxx.xxx") + 1) */
	size_t sizEntry;
	/* pointer to a single reference */
	char *pcRef;

	sizRefCnt = ptRefList->sizRefCnt;
	sizRefMax = ptRefList->sizRefMax;
	ppcRef    = ptRefList->ppcRef;

	WORD wVersionRequested;
	WSADATA wsaData;

	int iResult;
	int iErr;


	/* Initialize the Windows Socket Architecture. */
	wVersionRequested = MAKEWORD(2, 2);
	iErr = WSAStartup(wVersionRequested, &wsaData);
	fprintf(stderr, "WSAStartup: %d\n", iErr);  /* todo: exit on error */

	/* Set content of struct saddr and imreq to zero. */
	memset(&saddr, 0, sizeof(struct sockaddr_in));

	/* Open a UDP socket. */
	tSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( tSocket==INVALID_SOCKET )
	{
		int iErr; /* todo: remove? */
		iErr = WSAGetLastError();
		fprintf(stderr, "Error creating socket: (%d)\n", iErr);
	}
	else
	{
		/* Bind the socket to any interface and use the first free port. */
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(0);
		/*saddr.sin_addr.s_addr = htonl(INADDR_ANY);*/
		saddr.sin_addr.s_addr = ulInetAddr;

		iResult = bind(tSocket, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
		if( iResult<0 )
		{
			fprintf(stderr, "Error binding socket to interface");
		}
		else
		{
			/* Set the outgoing interface, referenced by its local IP address.
			   Set to default using INADDR_ANY */
			memset(&(uSocketOptions.iaddr), 0, sizeof(struct in_addr));
			/*uSocketOptions.iaddr.s_addr = INADDR_ANY;*/
			uSocketOptions.iaddr.s_addr = ulInetAddr;
			setsockopt(tSocket, IPPROTO_IP, IP_MULTICAST_IF, uSocketOptions.ac, sizeof(struct in_addr));

			/* Set multicast packet TTL to 3; default TTL is 1. */
			uSocketOptions.ttl = 3;
			setsockopt(tSocket, IPPROTO_IP, IP_MULTICAST_TTL, uSocketOptions.ac, sizeof(unsigned char));

			/* Set destination multicast address. */
			saddr.sin_family = PF_INET;
			saddr.sin_addr.s_addr = inet_addr("224.0.0.251");
			saddr.sin_port = htons(53280);

			// put some data in buffer
			memcpy(aucBuffer, "hello", 6);

			socklen = sizeof(struct sockaddr_in);
			// send packet
			iResult = sendto(tSocket, (const char*)aucBuffer, 6, 0, (struct sockaddr *)&saddr, socklen);

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
				if( iResult==SOCKET_ERROR )
				{
					fprintf(stderr, "Failed to wait for data.\n");
					break;
				}
				else if( iResult==1 )
				{
					iAddrLen = sizeof(uSrcAddr.tAddrIn);
					iPacketSize = recvfrom(tSocket, (char*)aucBuffer, MI_ETH_MAX_PACKET_SIZE, 0, &uSrcAddr.tAddr, &iAddrLen);
					if( iPacketSize==-1 )
					{
						fprintf(stderr, "Failed to receive packet.\n");
						iResult = -1;
					}
					else if(
						iPacketSize>=9 &&
						memcmp(aucBuffer, aucMagic, sizeof(aucMagic))==0
					       )
					{
						uiVersionMin = aucBuffer[5] | aucBuffer[6]<<8U;
						uiVersionMaj = aucBuffer[7] | aucBuffer[8]<<8U;
						sizMaxPacket = aucBuffer[9] | aucBuffer[10]<<8U;
						ulIp = aucBuffer[14] | aucBuffer[13]<<8U  | aucBuffer[12]<<16U  | aucBuffer[11]<<24U;

						printf("Found HBoot V%d.%d at 0x%08lx.\n", uiVersionMaj, uiVersionMin, ulIp);
/*
						{
							char buf[32];
							inet_ntop(AF_INET, &uSrcAddr.tAddrIn.sin_addr.s_addr, buf, sizeof(buf));
							printf("Found HBoot V%d.%d at %s .\n", uiVersionMaj, uiVersionMin, buf);
						}
*/
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
		shutdown(tSocket, SD_BOTH);

		/* Close the socket. */
		closesocket(tSocket);
	}

	ptRefList->sizRefCnt = sizRefCnt;
	ptRefList->sizRefMax = sizRefMax;
	ptRefList->ppcRef    = ppcRef;
}


size_t romloader_eth_device_win::ScanForServers(char ***pppcDeviceNames)
{

	IP_ADAPTER_INFO *pAdaptersInfoList = NULL;
	ULONG ulAdaptersInfoBufferSize = 0;

	IP_ADAPTER_INFO *pAdapterInfo = NULL;
	char *pszAdapterDesc;
	char *pszAdapterIp;
	unsigned long ulInetAddr;

	ROMLOADER_ETH_REFLIST_T tRefList;
	int iRet;

	tRefList.sizRefCnt = 0;
	tRefList.sizRefMax = 0;
	tRefList.ppcRef = NULL;

	/* Get the required buffer size for the adapter info */
	iRet = GetAdaptersInfo(pAdaptersInfoList, &ulAdaptersInfoBufferSize);
	if (iRet == 0)
	{
		/* No adapters found */
		printf("GetAdpaptersInfo: %d (No adapters found)\n", iRet);
	}
	if (iRet != ERROR_BUFFER_OVERFLOW)
	{
		/* other error */
		fprintf(stderr, "GetAdpaptersInfo: %d\n", iRet);
	}
	else
	{
		pAdaptersInfoList = (IP_ADAPTER_INFO *) malloc(ulAdaptersInfoBufferSize);
		if (pAdaptersInfoList == NULL)
		{
			/* allocation error */
			fprintf(stderr, "ScanForServers: ouf of memory!\n(failed to allocate memory for adapter info, %d bytes)\n", ulAdaptersInfoBufferSize);
		}
		else
		{
			iRet = GetAdaptersInfo(pAdaptersInfoList, &ulAdaptersInfoBufferSize);

			if (iRet == 0)
			{

				tRefList.sizRefCnt = 0;
				tRefList.sizRefMax = 16;
				tRefList.ppcRef = (char**)malloc(tRefList.sizRefMax*sizeof(char*));
				if( tRefList.ppcRef==NULL )
				{
					fprintf(stderr, "ScanForServers: out of memory!\n(failed to allocate initial reference list)\n");
				}
				else
				{
					pAdapterInfo = pAdaptersInfoList;
					while (pAdapterInfo)
					{
						if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET)
						{
							/* probe the adapter and add any new references to the list */
							pszAdapterDesc = pAdapterInfo->Description;
							pszAdapterIp =  pAdapterInfo->IpAddressList.IpAddress.String;
							ulInetAddr = inet_addr(pszAdapterIp);

							//print_adapter_info(pAdapterInfo);
							if (ulInetAddr == INADDR_NONE || ulInetAddr == INADDR_ANY || ulInetAddr == 0)
							{
								printf("Skipping adapter %s (%s)\n",  pszAdapterDesc, pszAdapterIp);
							}
							else
							{
								printf("Scanning adapter %s (%s)\n",  pszAdapterDesc, pszAdapterIp);
								ScanAdapter(&tRefList, ulInetAddr);
							}
						}
						pAdapterInfo = pAdapterInfo->Next;
					}
				}
			}
			else
			{
				/* error */
				printf("GetAdpaptersInfo: %d\n", iRet);
			}
			free (pAdaptersInfoList);
		}
	}

	*pppcDeviceNames = tRefList.ppcRef;
	return tRefList.sizRefCnt;
}




int romloader_eth_device_win::ExecuteCommand(const unsigned char *aucCommand, size_t sizCommand, unsigned char *aucResponse, size_t sizResponse, size_t *psizResponse)
{
	int iResult;
	size_t sizRxPacket;
	unsigned char ucStatus;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		iResult = SendPacket(aucCommand, sizCommand);
		if( iResult!=0 )
		{
			fprintf(stderr, "! execute_command: send_packet failed with errorcode %d\n", iResult);
		}
		else
		{
			iResult = RecvPacket(aucResponse, sizResponse, 1000, &sizRxPacket);
			if( iResult!=0 )
			{
				fprintf(stderr, "! execute_command: receive_packet failed with errorcode %d\n", iResult);
			}
			else
			{
				if( sizRxPacket<1 )
				{
					fprintf(stderr, "Timeout!\n");
					iResult = -1;
				}
				else
				{
					ucStatus = aucResponse[0];
					if( ucStatus==0 )
					{
						/* Yay, received something with status ok. */
						*psizResponse = sizRxPacket;
						iResult = 0;

					}
					else
					{
						fprintf(stderr, "Error: status is not ok: %d\n", ucStatus);
						iResult = -1;
					}
				}
			}
		}

		if( iResult!=0 )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				fprintf(stderr, "Retried 10 times and nothing happened. Giving up!\n");
				break;
			}
			else
			{
				fprintf(stderr, "***************************************\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "*            retry                    *\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "***************************************\n");

				/* Close the socket. */
				fprintf(stderr, "close the socket\n");
				closesocket(m_tHbootServer_Socket);

				/* Delay 1 second. */
				Sleep(1000);

				/* Open the socket again. */
				fprintf(stderr, "open the socket again\n");
				open_by_addr(m_tHbootServer_Addr.tAddrIn.sin_addr.s_addr);
			}
		}

	} while( iResult!=0 );

	return iResult;
}

