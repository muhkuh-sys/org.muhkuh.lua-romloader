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


#include "romloader_baka_main.h"
#include "_luaif/romloader_baka_wxlua_bindings.h"


/*-------------------------------------*/
/* configuration */

/* number of instances which can be used simultneous */
static const unsigned int uiInstances = 4;

/*-------------------------------------*/
/* some local prototypes */

bool fn_connect(void *pvHandle);
void fn_disconnect(void *pvHandle);
bool fn_is_connected(void *pvHandle);
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData);
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData, unsigned long ulSize);
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char bData);
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const unsigned char *pbData, unsigned long ulSize);
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0);

/*-------------------------------------*/

const muhkuh_plugin_desc plugin_desc =
{
	"Dummy Plugin",
	"romloader_baka",
	{ 0, 0, 1 }
};

const romloader_functioninterface tFunctionInterface =
{
	fn_connect,
	fn_disconnect,
	fn_is_connected,
	fn_read_data08,
	fn_read_data16,
	fn_read_data32,
	fn_read_image,
	fn_write_data08,
	fn_write_data16,
	fn_write_data32,
	fn_write_image,
	fn_call
};

static wxLuaState *m_ptLuaState;

/*-------------------------------------*/
/* config struct for all instances, this is just the connect flag at the moment */

typedef struct
{
	bool fIsUsed;
	bool fIsConnected;
} baka_instance_cfg_t;

baka_instance_cfg_t atInstanceCfg[uiInstances];

/*-------------------------------------*/

int fn_init(wxLog *ptLogTarget)
{
	wxLog *pOldLogTarget;
	baka_instance_cfg_t *ptC, *ptE;
	wxString strMsg;


	/* set main app's log target */
	pOldLogTarget = wxLog::GetActiveTarget();
	if( pOldLogTarget!=ptLogTarget )
	{
		wxLog::SetActiveTarget(ptLogTarget);
		if( pOldLogTarget!=NULL )
		{
			delete pOldLogTarget;
		}
	}

	/* say hi */
	strMsg.Printf(wxT("baka plugin init, preparing %d instances"), uiInstances);
	wxLogMessage(strMsg);

	/* init the lua state */
	m_ptLuaState = NULL;

	/* loop over all instance configs and init them */
	ptC = atInstanceCfg;
	ptE = ptC + uiInstances;
	while( ptC<ptE )
	{
		/* init instance */
		ptC->fIsUsed = false;
		ptC->fIsConnected = false;

		/* next instance */
		++ptC;
	}

	return 0;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_baka_lua_init();
//	fSuccess = wxLuaBinding_romloader_baka_lua_bind(m_ptLuaState);
	if( fSuccess!=true )
	{
		wxLogMessage(wxT("failed to init romloader_baka lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	unsigned int uiCnt;
	wxString strMsg;


	wxLogMessage(wxT("baka plugin leave"));

	/* check for connected instances */
	for(uiCnt=0; uiCnt<uiInstances; ++uiCnt)
	{
		if( atInstanceCfg[uiCnt].fIsUsed==true )
		{
			strMsg.Printf(wxT("baka plugin instance %d is still used"), uiInstances);
			wxLogError(strMsg);
		}
		if( atInstanceCfg[uiCnt].fIsConnected==true )
		{
			strMsg.Printf(wxT("baka plugin instance %d is still connected"), uiInstances);
			wxLogError(strMsg);
		}
	}

	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	unsigned int uiInterfaceCnt;
	muhkuh_plugin_instance *ptInst;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;
	void *pvHandle;
	bool fIsUsed;


	strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
	strLuaCreateFn = wxT("muhkuh.romloader_baka_create");

	// detect all interfaces
	for(uiInterfaceCnt=0; uiInterfaceCnt<uiInstances; ++uiInterfaceCnt)
	{
		strName.Printf("baka_%d", uiInterfaceCnt);
		pvHandle = (void*)uiInterfaceCnt;
		fIsUsed = atInstanceCfg[uiInterfaceCnt].fIsUsed;
		ptInst = new muhkuh_plugin_instance(strName, strTyp, fIsUsed, strLuaCreateFn, pvHandle);

		pvInterfaceList->push_back(ptInst);
	}

	return (int)uiInterfaceCnt;
}


/*-------------------------------------*/

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


/*-------------------------------------*/

romloader *romloader_baka_create(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	wxString strName;
	wxString strTyp;
	romloader *ptInstance;
	bool fIsUsed;


	ptInstance = NULL;

	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_create: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		/* is the instance really unused? */
		fIsUsed = atInstanceCfg[uiIdx].fIsUsed;
		if( fIsUsed==true )
		{
			strMsg.Printf(wxT("romloader_baka_create: plugin instance %p is already in use"), pvHandle);
			wxLogError(strMsg);
		}
		else
		{
			/* create the new instance */
			strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
			strName.Printf("baka_%d", uiIdx);
			ptInstance = new romloader(strName, strTyp, &tFunctionInterface, pvHandle, romloader_baka_close_instance, m_ptLuaState);
			atInstanceCfg[uiIdx].fIsUsed = true;
		}
	}

	return ptInstance;
}


/*-------------------------------------*/

/* open the connection to the device */
bool fn_connect(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_connect: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return false;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: connected"), uiIdx);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].fIsConnected = true;
		return true;
	}
}

/* close the connection to the device */
void fn_disconnect(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_disconnect: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
	}
	else
	{
		strMsg.Printf(wxT("baka %d: disconnected"), uiIdx);
		wxLogMessage(strMsg);
		atInstanceCfg[uiIdx].fIsConnected = false;
	}
}

/* returns the connection state of the device */
bool fn_is_connected(void *pvHandle)
{
	unsigned int uiIdx;
	wxString strMsg;
	bool fIsConnected;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_is_connected: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return false;
	}
	else
	{
		fIsConnected = atInstanceCfg[uiIdx].fIsConnected;
		strMsg.Printf(wxT("baka %d: is_connected = %d"), uiIdx, (fIsConnected?1:0));
		wxLogMessage(strMsg);
		return fIsConnected;
	}
}

/* read a byte (8bit) from the netx to the pc */
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_read_data08: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: read08 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pbData!=NULL )
		{
			*pbData = 0x00;
		}
		return 0;
	}
}

/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_read_data16: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: read16 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pusData!=NULL )
		{
			*pusData = 0x0000;
		}
		return 0;
	}
}

/* read a long (32bit) from the netx to the pc */
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_read_data32: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: read32 from 0x%08lx"), uiIdx, ulNetxAddress);
		wxLogMessage(strMsg);
		if( pulData!=NULL )
		{
			*pulData = 0x00000000;
		}
		return 0;
	}
}

/* read a byte array from the netx to the pc */
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData, unsigned long ulSize)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_read_image: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: read_image from 0x%08lx with %ld bytes"), uiIdx, ulNetxAddress, ulSize);
		wxLogMessage(strMsg);
		if( pbData!=NULL )
		{
			memset(pbData, 0, ulSize);
		}
		return 0;
	}
}

/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char bData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_write_data08: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: write08 from 0x%08lx with 0x%02x"), uiIdx, ulNetxAddress, bData);
		wxLogMessage(strMsg);
		return 0;
	}
}

/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_write_data16: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: write16 from 0x%08lx with 0x%04x"), uiIdx, ulNetxAddress, usData);
		wxLogMessage(strMsg);
		return 0;
	}
}

/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_write_data32: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: write32 from 0x%08lx with 0x%08lx"), uiIdx, ulNetxAddress, ulData);
		wxLogMessage(strMsg);
		return 0;
	}
}

/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const unsigned char *WXUNUSED(pbData), unsigned long ulSize)
{
	unsigned int uiIdx;
	wxString strMsg;


	/* check the handle */
	uiIdx = (unsigned int)pvHandle;
	if( uiIdx>=uiInstances )
	{
		strMsg.Printf(wxT("romloader_baka_fn_write_image: handle %p is no valid plugin handle"), pvHandle);
		wxLogError(strMsg);
		return -1;
	}
	else
	{
		strMsg.Printf(wxT("baka %d: write_image from 0x%08lx with %ld bytes"), uiIdx, ulNetxAddress, ulSize);
		wxLogMessage(strMsg);
		return 0;
	}
}

/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0)
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

/*-------------------------------------*/
