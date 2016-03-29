/***************************************************************************
 *   Copyright (C) 2013 by Christoph Thelen                                *
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


#include <ctype.h>
#include <stdio.h>

#include "romloader_jtag_main.h"

/*-------------------------------------*/

#if CFG_DEBUGMSG!=0
	/* show all messages by default */
	static unsigned long s_ulCurSettings = 0xffffffff;

	#define DEBUGZONE(n)  (s_ulCurSettings&(0x00000001<<(n)))

	/* NOTE: These defines must match the ZONE_* defines. */
	#define DBG_ZONE_ERROR      0
	#define DBG_ZONE_WARNING    1
	#define DBG_ZONE_FUNCTION   2
	#define DBG_ZONE_INIT       3
	#define DBG_ZONE_VERBOSE    7

	#define ZONE_ERROR          DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING        DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION       DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT           DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE        DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,printf_exp) ((void)((cond)?(uprintf printf_exp),1:0))
#else  /* CFG_DEBUGMSG!=0 */
	#define DEBUGMSG(cond,printf_exp) ((void)0)
#endif /* CFG_DEBUGMSG!=0 */


/*-------------------------------------*/

const char *romloader_jtag_provider::m_pcPluginNamePattern = "romloader_jtag_%02x_%02x";

romloader_jtag_provider::romloader_jtag_provider(swig_type_info *p_romloader_jtag, swig_type_info *p_romloader_jtag_reference)
 : muhkuh_plugin_provider("romloader_jtag")
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::romloader_jtag_provider(): p_romloader_jtag=%p, p_romloader_jtag_reference=%p\n", p_romloader_jtag, p_romloader_jtag_reference));

	/* Get the romloader_jtag LUA type. */
	m_ptPluginTypeInfo = p_romloader_jtag;
	m_ptReferenceTypeInfo = p_romloader_jtag_reference;

	iResult = romloader_jtag_openocd_init();
	if( iResult==0 )
	{
		iResult = romloader_jtag_openocd_detect();
		printf("iResult: %d\n", iResult);
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::romloader_jtag_provider()\n"));
}


romloader_jtag_provider::~romloader_jtag_provider(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::~romloader_jtag_provider()\n"));

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::~romloader_jtag_provider()\n"));
}


int romloader_jtag_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	romloader_jtag_reference **pptReferences;
	romloader_jtag_reference **pptRefCnt;
	romloader_jtag_reference **pptRefEnd;
	romloader_jtag_reference *ptRef;
	size_t sizReferences;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::DetectInterfaces(): ptLuaStateForTableAccess=%p\n", ptLuaStateForTableAccess));

	sizReferences = 0;

	/* detect devices */
	pptReferences = NULL;
	/* TODO: implement detect function. */
/*	iResult = m_ptUsbDevice->detect_interfaces(&pptReferences, &sizReferences, this); */
	iResult = -1;
	if( iResult==0 && pptReferences!=NULL )
	{
		pptRefCnt = pptReferences;
		pptRefEnd = pptReferences + sizReferences;
		while( pptRefCnt<pptRefEnd )
		{
			ptRef = *pptRefCnt;
			if( ptRef!=NULL )
			{
				add_reference_to_table(ptLuaStateForTableAccess, ptRef);
			}
			++pptRefCnt;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::DetectInterfaces(): sizReferences=%d\n", sizReferences));

	return sizReferences;
}


romloader_jtag *romloader_jtag_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_jtag *ptPlugin;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::ClaimInterface(): ptReference=%p\n", ptReference));

	/* expect error */
	ptPlugin = NULL;


	if( ptReference==NULL )
	{
		fprintf(stderr, "%s(%p): claim_interface(): missing reference!\n", m_pcPluginId, this);
	}
	else
	{
		pcName = ptReference->GetName();
		if( pcName==NULL )
		{
			fprintf(stderr, "%s(%p): claim_interface(): missing name!\n", m_pcPluginId, this);
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, &uiBusNr, &uiDevAdr)!=2 )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else
		{
			ptPlugin = new romloader_jtag(pcName, m_pcPluginId, this, uiBusNr, uiDevAdr);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::ClaimInterface(): ptPlugin=%p\n", ptPlugin));

	return ptPlugin;
}


bool romloader_jtag_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::ReleaseInterface(): ptPlugin=%p\n", ptPlugin));

	/* expect error */
	fOk = false;

	if( ptPlugin==NULL )
	{
		fprintf(stderr, "%s(%p): release_interface(): missing plugin!\n", m_pcPluginId, this);
	}
	else
	{
		pcName = ptPlugin->GetName();
		if( pcName==NULL )
		{
			fprintf(stderr, "%s(%p): release_interface(): missing name!\n", m_pcPluginId, this);
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, &uiBusNr, &uiDevAdr)!=2 )
		{
			fprintf(stderr, "%s(%p): release_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else
		{
			printf("%s(%p): released interface %s.\n", m_pcPluginId, this, pcName);
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::ReleaseInterface(): fOk=%d\n", fOk));

	return fOk;
}


/*-------------------------------------*/

romloader_jtag::romloader_jtag(const char *pcName, const char *pcTyp, romloader_jtag_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, ptProvider)
 , m_ptJtagProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::romloader_jtag(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::romloader_jtag()\n"));
}


romloader_jtag::romloader_jtag(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_jtag_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, pcLocation, ptProvider)
 , m_ptJtagProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::romloader_jtag(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));
	
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::romloader_jtag()\n"));
}


romloader_jtag::~romloader_jtag(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::~romloader_jtag()\n"));

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::~romloader_jtag()\n"));
}


/* open the connection to the device */
void romloader_jtag::Connect(lua_State *ptClientData)
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::Connect(): ptClientData=%p\n", ptClientData));

	if( m_fIsConnected!=false )
	{
		printf("%s(%p): already connected, ignoring new connect request\n", m_pcName, this);
	}
	else
	{
		/* TODO: add the connect method. */
#if 0
		iResult = m_ptJtagDevice->Connect(m_uiBusNr, m_uiDeviceAdr);
		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to connect to device", m_pcName, this);
		}
		else
		{
			/* NOTE: set m_fIsConnected to true here or detect_chiptyp and chip_init will fail! */
			m_fIsConnected = true;

			/* Synchronize with the client to get these settings:
			 *   chip type
			 *   sequence number
			 *   maximum packet size
			 */
			if( synchronize()!=true )
			{
				fprintf(stderr, "%s(%p): failed to synchronize!", m_pcName, this);
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to synchronize!", m_pcName, this);
				m_fIsConnected = false;
				m_ptUsbDevice->Disconnect();
				iResult = -1;
			}
		}
		DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::Connect(): iResult=%d\n", iResult));

		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
#endif
	}
}



/* close the connection to the device */
void romloader_jtag::Disconnect(lua_State *ptClientData)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::Disconnect(): ptClientData=%p\n", ptClientData));

	/* NOTE: allow disconnects even if the plugin was already disconnected. */
#if 0
	if( m_ptUsbDevice!=NULL )
	{
		m_ptUsbDevice->Disconnect();
	}
#endif
	m_fIsConnected = false;

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::Disconnect()\n"));
}



/* read a byte (8bit) from the netx to the pc */
uint8_t romloader_jtag::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	unsigned char ucData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data08(): ptClientData=%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		ucData  = 0x12;
		fOk = true;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data08(): fOk=%d, ucData=0x%02x\n", fOk, ucData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucData;
}


/* read a word (16bit) from the netx to the pc */
uint16_t romloader_jtag::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	unsigned short usData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data16(): ptClientData=%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		usData = 0x1234;
		fOk = true;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data16(): fOk=%d, usData=0x%04x\n", fOk, usData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usData;
}


/* read a long (32bit) from the netx to the pc */
uint32_t romloader_jtag::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	unsigned long ulData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data32(): ptClientData=%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		ulData = 0x12345678;
		fOk = true;
	}

	/* Print the function end marker already here as the MUHKUH_PLUGIN_EXIT_ERROR might leave the function. */
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data32(): fOk=%d, ulData=0x%08lx\n", fOk, ulData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulData;
}


/* read a byte array from the netx to the pc  */
void romloader_jtag::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	int iResult;
	size_t sizChunk;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_image(): ulNetxAddress=0x%08lx, ulSize=0x%08lx, ppcBUFFER_OUT=%p, psizBUFFER_OUT=%p, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulSize, ppcBUFFER_OUT, psizBUFFER_OUT, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	pcBufferStart = NULL;
	sizBuffer = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: not connected!", m_pcName, this);
		fOk = false;
	}
	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	else if ( ulSize>0 )
	{
		pcBufferStart = (char*)malloc(ulSize);
		if( pcBufferStart==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: failed to allocate %d bytes!", m_pcName, this, ulSize);
			fOk = false;
		}
		else
		{
			sizBuffer = ulSize;

			pcBuffer = pcBufferStart;
			lBytesProcessed = 0;
			do
			{
				sizChunk = ulSize;
				if( sizChunk>m_sizMaxPacketSizeClient-1 )
				{
					sizChunk = m_sizMaxPacketSizeClient - 1;
				}

//				memcpy(pcBuffer, m_aucPacketInputBuffer+1, sizChunk);
				pcBuffer += sizChunk;
				ulSize -= sizChunk;
				ulNetxAddress += sizChunk;
				lBytesProcessed += sizChunk;

				fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
				if( fIsRunning!=true )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image cancelled!", m_pcName, this);
					fOk = false;
					break;
				}
			} while( ulSize!=0 );
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_image(): fOk=%d, pcBufferStart=%p, sizBuffer=0x%08x\n", fOk, pcBufferStart, sizBuffer));

	if( fOk == true )
	{
		*ppcBUFFER_OUT = pcBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	}
	else
	{
		if ( pcBufferStart!=NULL)
		{
			free(pcBufferStart);
		}
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* write a byte (8bit) from the pc to the netx */
void romloader_jtag::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data08(): ptClientData=%p, ulNetxAddress=0x08lx, ucData=0x%02lx\n", ptClientData, ulNetxAddress, ucData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		fOk = true;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data08(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_jtag::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data16(): ptClientData=%p, ulNetxAddress=0x08lx, usData=0x%04lx\n", ptClientData, ulNetxAddress, usData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		fOk = true;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data16(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_jtag::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data32(): ptClientData=%p, ulNetxAddress=0x08lx, ulData=0x%08lx\n", ptClientData, ulNetxAddress, ulData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		fOk = true;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data32(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a byte array from the pc to the netx */
void romloader_jtag::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_image(): ulNetxAddress=0x%08lx, pcBUFFER_IN=%p, sizBUFFER_IN=0x%08x, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, pcBUFFER_IN, sizBUFFER_IN, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: not connected!", m_pcName, this);
		fOk = false;
	}
	else if( sizBUFFER_IN!=0 )
	{
		lBytesProcessed = 0;
		do
		{
			sizChunk = sizBUFFER_IN;
			if( sizChunk>(m_sizMaxPacketSizeClient-7) )
			{
				sizChunk = m_sizMaxPacketSizeClient - 7;
			}

			pcBUFFER_IN += sizChunk;
			sizBUFFER_IN -= sizChunk;
			ulNetxAddress += sizChunk;
			lBytesProcessed += sizChunk;

			fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
			if( fIsRunning!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image cancelled!", m_pcName, this);
				fOk = false;
				break;
			}
		} while( sizBUFFER_IN!=0 );
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_image(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* call routine */
void romloader_jtag::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::call(): ulNetxAddress=0x%08lx, ulParameterR0=0x%08lx, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulParameterR0, tLuaFn.L, lCallbackUserData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
			/* Receive message packets. */
			while(1)
			{
				pcProgressData = NULL;
				sizProgressData = 0;
#if 0
				iResult = m_ptUsbDevice->receive_packet(m_aucPacketInputBuffer, m_sizMaxPacketSizeClient, &sizInBuf, 500);
				if( iResult==LIBUSB_ERROR_TIMEOUT )
				{ // should we print anything if a timeout occurs?
				}
				else if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: failed to receive packet! (error %d)", m_pcName, this, iResult);
					fOk = false;
					break;
				}
				else
				{
					ucStatus = m_aucPacketInputBuffer[0] & MONITOR_STATUS_MSK;
					if( sizInBuf==1 && ucStatus==MONITOR_STATUS_CallFinished )
					{
						fOk = true;
						break;
					}
					else if( sizInBuf>=1 && ucStatus==MONITOR_STATUS_CallMessage )
					{
//						printf("Received message:\n");
//						hexdump(aucInBuf+1, sizInBuf-1, 0);
						pcProgressData = (char*)m_aucPacketInputBuffer+1;
						sizProgressData = sizInBuf-1;
					}
					else if( sizInBuf!=0 )
					{
						printf("Received invalid packet:\n");
						hexdump(m_aucPacketInputBuffer, sizInBuf, 0);

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: received invalid packet!", m_pcName, this);
						fOk = false;
						break;
					}
				}

				if (pcProgressData != NULL)
				{
					fIsRunning = callback_string(&tLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
					if( fIsRunning!=true )
					{
						/* Send a cancel request to the device. */
						m_aucPacketOutputBuffer[0] = 0x2b;
						iResult = m_ptUsbDevice->send_packet(m_aucPacketOutputBuffer, 1, 100);

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was cancelled!", m_pcName, this);
						fOk = false;
						break;
					}
				}
#endif
			}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::call(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_jtag::hexdump(const unsigned char *pucData, unsigned long ulSize, unsigned long ulNetxAddress)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = ulNetxAddress;
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


/*-------------------------------------*/


romloader_jtag_reference::romloader_jtag_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_jtag_reference::romloader_jtag_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_jtag_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_jtag_reference::romloader_jtag_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_jtag_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, pcLocation, fIsUsed, ptProvider)
{
}


romloader_jtag_reference::romloader_jtag_reference(const romloader_jtag_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/

