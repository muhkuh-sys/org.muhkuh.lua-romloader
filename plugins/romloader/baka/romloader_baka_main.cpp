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


#include <stdio.h>

#include "romloader_baka_main.h"

#ifdef _WIN32
	#define snprintf _snprintf
#endif

const char *romloader_baka_provider::m_pcPluginNamePattern = "baka_%d";


romloader_baka_provider::romloader_baka_provider(swig_type_info *p_romloader_baka, swig_type_info *p_romloader_baka_reference)
 : muhkuh_plugin_provider("romloader_baka")
 , m_ptInstanceCfg(NULL)
 , m_cfg_iInstances(0)
{
	BAKA_INSTANCE_CFG_T *ptC, *ptE;


	printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* get the romloader_baka lua type */
	m_ptPluginTypeInfo = p_romloader_baka;
	m_ptReferenceTypeInfo = p_romloader_baka_reference;

	m_cfg_iInstances = 4;
	m_ptInstanceCfg = new BAKA_INSTANCE_CFG_T[m_cfg_iInstances];

	/* loop over all instance configs and init them */
	ptC = m_ptInstanceCfg;
	ptE = ptC + m_cfg_iInstances;
	while( ptC<ptE )
	{
		/* init instance */
		ptC->fIsUsed = false;

		/* next instance */
		++ptC;
	}
}


romloader_baka_provider::~romloader_baka_provider(void)
{
	int iCnt;


	printf("%s(%p): provider delete\n", m_pcPluginId, this);

	if( m_ptInstanceCfg!=NULL )
	{
		/* check for connected instances */
		for(iCnt=0; iCnt<m_cfg_iInstances; ++iCnt)
		{
			if( m_ptInstanceCfg[iCnt].fIsUsed==true )
			{
				printf("%s(%p): instance %d is still used\n", m_pcPluginId, this, iCnt);
			}
		}

		delete[] m_ptInstanceCfg;
	}
}


int romloader_baka_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	size_t sizTable;
	int iInterfaceCnt;
	romloader_baka_reference *ptRef;
	bool fIsUsed;
	const size_t sizMaxName = 32;
	char acName[sizMaxName];


	// terminate name buffer
	acName[sizMaxName-1] = 0;

	// get the size of the table
	sizTable = lua_objlen(ptLuaStateForTableAccess, 2);

	// detect all interfaces
	for(iInterfaceCnt=0; iInterfaceCnt<m_cfg_iInstances; ++iInterfaceCnt)
	{
		snprintf(acName, sizMaxName-1, m_pcPluginNamePattern, iInterfaceCnt);
		fIsUsed = m_ptInstanceCfg[iInterfaceCnt].fIsUsed;
		ptRef = new romloader_baka_reference(acName, m_pcPluginId, fIsUsed, this);

		SWIG_NewPointerObj(ptLuaStateForTableAccess, ptRef, m_ptReferenceTypeInfo, 1);
		sizTable++;
		lua_rawseti(ptLuaStateForTableAccess, 2, sizTable);
	}

	return iInterfaceCnt;
}


romloader_baka *romloader_baka_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_baka *ptPlugin;
	const char *pcName;
	int iInterfaceIdx;


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
		else if( sscanf(pcName, m_pcPluginNamePattern, &iInterfaceIdx)!=1 )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else if( (iInterfaceIdx<0) || (iInterfaceIdx>=m_cfg_iInstances) )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid interface index: %d\n", m_pcPluginId, this, iInterfaceIdx);
		}
		else if( m_ptInstanceCfg[iInterfaceIdx].fIsUsed==true )
		{
			fprintf(stderr, "%s(%p): claim_interface(): can not claim interface %d, already in use!\n", m_pcPluginId, this, iInterfaceIdx);
		}
		else
		{
			m_ptInstanceCfg[iInterfaceIdx].fIsUsed = true;
			printf("%s(%p): claim_interface(): claimed interface %d.\n", m_pcPluginId, this, iInterfaceIdx);

			ptPlugin = new romloader_baka(pcName, m_pcPluginId, this);
		}
	}

	return ptPlugin;
}


bool romloader_baka_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	int iInterfaceIdx;


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
		else if( sscanf(pcName, m_pcPluginNamePattern, &iInterfaceIdx)!=1 )
		{
			fprintf(stderr, "%s(%p): release_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else if( (iInterfaceIdx<0) || (iInterfaceIdx>=m_cfg_iInstances) )
		{
			fprintf(stderr, "%s(%p): release_interface(): invalid interface index: %d\n", m_pcPluginId, this, iInterfaceIdx);
		}
		else
		{
			if( m_ptInstanceCfg[iInterfaceIdx].fIsUsed==false )
			{
				printf("%s(%p): release_interface(): interface %d is not claimed\n", m_pcPluginId, this, iInterfaceIdx);
			}
			else
			{
				m_ptInstanceCfg[iInterfaceIdx].fIsUsed = false;
				printf("%s(%p): released interface %d.\n", m_pcPluginId, this, iInterfaceIdx);
			}
			fOk = true;
		}
	}

	return fOk;
}

#if 0
static void romloader_baka_close_instance(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	bool fIsUsed;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_close_instance: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		/* is the instance really used? */
		fIsUsed = atInstanceCfg[uiIdx].fIsUsed;
		if( fIsUsed==false )
		{
			strMsg.Printf(wxT("romloader_baka_close_instance: plugin instance %p is not in use"), pvHandle);
			wxLogError(strMsg);
		}
		else
		{
			strMsg.Printf(wxT("baka %d: close"), uiIdx);
			wxLogMessage(strMsg);
			/* free the instance */
			atInstanceCfg[uiIdx].fIsUsed = false;
		}
	}
}
#endif

/*-------------------------------------*/

romloader_baka::romloader_baka(const char *pcName, const char *pcTyp, romloader_baka_provider *ptProvider)
 : romloader(pcName, pcTyp, ptProvider)
{
	printf("%s(%p): created in romloader_baka\n", m_pcName, this);
}


romloader_baka::~romloader_baka(void)
{
	printf("%s(%p): deleted in romloader_baka\n", m_pcName, this);
}


/* open the connection to the device */
void romloader_baka::Connect(lua_State *ptClientData)
{
	printf("%s(%p): connect()\n", m_pcName, this);

	if( m_fIsConnected==true )
	{
		printf("%s(%p): connect() called when already connected\n", m_pcName, this);
	}
	else
	{
		m_fIsConnected = true;
	}
}


/* close the connection to the device */
void romloader_baka::Disconnect(lua_State *ptClientData)
{
	printf("%s(%p): disconnect()\n", m_pcName, this);

	/* NOTE: allow disconnects even if the plugin was already disconnected. */
	m_fIsConnected = false;
}


/* read a byte (8bit) from the netx to the pc */
unsigned char romloader_baka::read_data08(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	printf("%s(%p): read_data08(0x%08lx) = 0x00\n", m_pcName, this, ulNetxAddress);

	return 0;
}


/* read a word (16bit) from the netx to the pc */
unsigned short romloader_baka::read_data16(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	printf("%s(%p): read_data16(0x%08lx) = 0x0000\n", m_pcName, this, ulNetxAddress);

	return 0;
}


/* read a long (32bit) from the netx to the pc */
unsigned long romloader_baka::read_data32(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	printf("%s(%p): read_data32(0x%08lx) = 0x00000000\n", m_pcName, this, ulNetxAddress);

	return 0;
}


void romloader_baka::read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcOutputData, unsigned long *pulOutputData, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcData;
	bool fStillRunning;


	printf("%s(%p): read_image(0x%08lx, 0x%08lx) = 0x00 .. 0x00\n", m_pcName, this, ulNetxAddress, ulSize);

	fStillRunning = callback_long(&tLuaFn, 0, lCallbackUserData);

	pcData = NULL;
	if( ulSize!=0 )
	{
		pcData = new char[ulSize];
		memset(pcData, 0, ulSize);

		// show a hexdump of the data
		hexdump(pcData, ulSize, ulNetxAddress);
	}

	fStillRunning = callback_long(&tLuaFn, ulSize, lCallbackUserData);

	swiglua_ref_clear(&tLuaFn);

	*ppcOutputData = pcData;
	*pulOutputData = ulSize;
}


/* write a byte (8bit) from the pc to the netx */
void romloader_baka::write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData)
{
	printf("%s(%p): write_data08(0x%08lx, 0x%02x)\n", m_pcName, this, ulNetxAddress, ucData);
}


/* write a word (16bit) from the pc to the netx */
void romloader_baka::write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData)
{
	printf("%s(%p): write_data16(0x%08lx, 0x%04x)\n", m_pcName, this, ulNetxAddress, usData);
}


/* write a long (32bit) from the pc to the netx */
void romloader_baka::write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData)
{
	printf("%s(%p): write_data32(0x%08lx, 0x%08lx)\n", m_pcName, this, ulNetxAddress, ulData);
}

/* write a byte array from the pc to the netx */
void romloader_baka::write_image(unsigned long ulNetxAddress, const char *pcInputData, unsigned long ulInputData, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fStillRunning;


	printf("%s(%p): write_image(0x%08lx)\n", m_pcName, this, ulNetxAddress);

	fStillRunning = callback_long(&tLuaFn, 0, lCallbackUserData);

	// show a hexdump of the data
	hexdump(pcInputData, ulInputData, ulNetxAddress);

	fStillRunning = callback_long(&tLuaFn, ulInputData, lCallbackUserData);

	swiglua_ref_clear(&tLuaFn);
}


#if 0
/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_call: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: call 0x%08lx with param 0x%08lx"), uiIdx, ulNetxAddress, ulParameterR0);
		wxLogMessage(strMsg);
		return 0;
	}
}
#endif

void romloader_baka::hexdump(const char *pcData, unsigned long ulSize, unsigned long ulNetxAddress)
{
	const char *pcDumpCnt, *pcDumpEnd;
	unsigned long ulAddressCnt;
	unsigned long ulSkipOffset;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pcDumpCnt = pcData;
	pcDumpEnd = pcData + ulSize;
	ulAddressCnt = ulNetxAddress;
	while( pcDumpCnt<pcDumpEnd )
	{
		// get number of bytes for the next line
		sizChunkSize = 16;
		sizBytesLeft = pcDumpEnd-pcDumpCnt;
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
			printf("%02X ", (unsigned char)(*(pcDumpCnt++)));
			--sizChunkCnt;
		}
		// next line
		printf("\n");
		ulAddressCnt += sizChunkSize;
		// only show first and last 3 lines for very long files
		if( (pcDumpCnt-pcData)==0x30 && (pcDumpEnd-pcData)>0x100 )
		{
			ulSkipOffset  = ulSize + 0xf;
			ulSkipOffset &= ~0xf;
			ulSkipOffset -= 0x30;
			pcDumpCnt = pcData + ulSkipOffset;
			ulAddressCnt = ulNetxAddress + ulSkipOffset;
			printf("... (skipping 0x%08lX bytes)", ulSkipOffset-0x30);
		}
	}
}

/*-------------------------------------*/


romloader_baka_reference::romloader_baka_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_baka_reference::romloader_baka_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_baka_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_baka_reference::romloader_baka_reference(const romloader_baka_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}




/*-------------------------------------*/

