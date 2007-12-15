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


#include "romloader_usb_io.h"
#include "romloader_usb_main.h"
#include "_luaif/romloader_usb_wxlua_bindings.h"

/*-------------------------------------*/
/* some local prototypes */

bool fn_connect(void *pvHandle);
void fn_disconnect(void *pvHandle);
bool fn_is_connected(void *pvHandle);
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData);
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData);
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("USB Bootloader"),
	wxT(""),
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

int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	wxLog *pOldLogTarget;


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
	wxLogMessage(wxT("bootloader usb plugin init"));

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	usb_init();

	return 0;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_usb_lua_init();
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
	wxLogMessage(wxT("bootloader usb plugin leave"));

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
	std::vector<struct usb_device *> tDeviceList;
	struct usb_bus *ptBusses;
	struct usb_bus *ptBus;
	struct usb_device *ptDev;
	int iResult;
	int iInterfaces;
	bool fDeviceIsNetx;
	muhkuh_plugin_instance *ptInst;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;


	/* detect all busses */
	iResult = usb_find_busses();
	if( iResult<0 )
	{
		/* failed to detect busses */
		return -1;
	}

	/* detect all devices on the busses */
	iResult = usb_find_devices();
	if( iResult<0 )
	{
		/* failed to detect devices */
		return -1;
	}

	strTyp = plugin_desc.strPluginId;
	strLuaCreateFn = wxT("muhkuh.romloader_usb_create");

	iInterfaces = 0;

	/* get array of busses */
	ptBusses = usb_get_busses();

	/* loop over all busses */
	ptBus = ptBusses;
	while(ptBus!=NULL)
	{
		/* loop over all devices */
		ptDev = ptBus->devices;
		while(ptDev!=NULL) 
		{
			// is this device a netx bootmonitor?
			// class==0, vendor==0x0cc4, product==0x0815, revision==1.00
			fDeviceIsNetx = romloader_usb_isDeviceNetx(ptDev);
			if( fDeviceIsNetx==true )
			{
				/* construct the name */
				strName.Printf("romloader_usb_%08x_%02x", ptBus->location, ptDev->devnum);
				ptInst = new muhkuh_plugin_instance(strName, strTyp, false, strLuaCreateFn, ptDev);

				pvInterfaceList->push_back(ptInst);
				++iInterfaces;
			}
			/* next device */
			ptDev = ptDev->next;
		}

		/* scan next bus */
		ptBus = ptBus->next;
	}

	// mark all interfaces which are also in the 'open' list as 'busy'
	// TODO...

	return iInterfaces;
}


/*-------------------------------------*/


static void romloader_usb_close_instance(void *pvHandle)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strMsg;


	// get handle
	tHandle = (netx_device_handle)pvHandle;
	strMsg.Printf(wxT("closing romloader usb at %p"), tHandle);
	wxLogMessage(strMsg);
	tResult = romloader_usb_close(tHandle);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to close the usb connection to the netx"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
}


/*-------------------------------------*/

romloader *romloader_usb_create(void *pvHandle)
{
	tNetxUsbState tResult;
	netx_device_handle tHandle;
	unsigned char *pucData;
	unsigned int uiDataLen;
	struct usb_device *ptNetxDev;
	romloader *ptInstance = NULL;
	wxString strName;
	wxString strTyp;


	// get handle
	ptNetxDev = (struct usb_device *)pvHandle;

	// open device and provide cancel function with this class as parameter
	tResult = romloader_usb_openByDevice(&tHandle, ptNetxDev);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to open the usb connection to the netx"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// get netx welcome message
		tResult = romloader_usb_getNetxData(tHandle, &pucData, &uiDataLen, NULL, 0, NULL);
		if( tResult!=netxUsbState_Ok )
		{
			wxLogMessage(wxT("failed to receive netx response, trying to reset netx!"));
			wxLogMessage( romloader_usb_getErrorString(tResult) );
			// try to reset the device and try again
			tResult = romloader_usb_resetDevice(tHandle);
			if( tResult!=netxUsbState_Ok )
			{
				wxLogError(wxT("failed to reset the netx, giving up!"));
			}
			// open device and provide cancel function with this class as parameter
			wxLogMessage(wxT("reset ok!"));
			tResult = romloader_usb_openByDevice(&tHandle, ptNetxDev);
			if( tResult!=netxUsbState_Ok )
			{
				wxLogError(wxT("failed to open the usb connection to the netx"));
				wxLogError( romloader_usb_getErrorString(tResult) );
			}
			else
			{
				// get netx welcome message
				tResult = romloader_usb_getNetxData(tHandle, &pucData, &uiDataLen, NULL, 0, NULL);
				if( tResult!=netxUsbState_Ok )
				{
					wxLogError(wxT("failed to receive netx response, trying to reset netx!"));
					wxLogError( romloader_usb_getErrorString(tResult) );
					romloader_usb_close(pvHandle);
				}
			}
		}
	}

	if( tResult==netxUsbState_Ok )
	{
		wxLogInfo(wxString::From8BitData((const char*)pucData, uiDataLen));
		free(pucData);

		/* create the new instance */
		strTyp = plugin_desc.strPluginId;
		strName.Printf("romloader_usb_%08x_%02x", ptNetxDev->bus->location, ptNetxDev->devnum);
		ptInstance = new romloader(strName, strTyp, &tFunctionInterface, tHandle, romloader_usb_close_instance, m_ptLuaState);
//		atInstanceCfg[uiIdx].fIsUsed = true;
	}

	return ptInstance;
}


/*-------------------------------------*/


/* open the connection to the device */
bool fn_connect(void *pvHandle)
{
	return true;
}


/* close the connection to the device */
void fn_disconnect(void *pvHandle)
{
}


/* returns the connection state of the device */
bool fn_is_connected(void *pvHandle)
{
	return false;
}


/* read a byte (8bit) from the netx to the pc */
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucTmpData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned int uiResultData;


	// construct the command
	strCommand.Printf(wxT("DUMP %08lX BYTE"), ulNetxAddress);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucTmpData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
		return -1;
	}

	// parse the result
	iMatches = sscanf((char*)pucTmpData, "%8lX: %2X ", &ulResultAddress, &uiResultData);
	free(pucTmpData);
	if( iMatches!=2 )
	{
		wxLogError(wxT("strange response from netx!"));
		return -1;
	}
	if( ulResultAddress!=ulNetxAddress )
	{
		wxLogError(wxT("response does not match request!"));
		return -1;
	}

	*pucData = (unsigned char)uiResultData;
	return 0;
}


/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned int uiResultData;


	// construct the command
	strCommand.Printf(wxT("DUMP %08lX WORD"), ulNetxAddress);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
		return -1;
	}

	// parse the result
	iMatches = sscanf((char*)pucData, "%8lX: %4X ", &ulResultAddress, &uiResultData);
	free(pucData);
	if( iMatches!=2 )
	{
		wxLogError(wxT("strange response from netx!"));
		return -1;
	}
	if( ulResultAddress!=ulNetxAddress )
	{
		wxLogError(wxT("response does not match request!"));
		return -1;
	}

	*pusData = (unsigned short)uiResultData;
	return 0;
}


/* read a long (32bit) from the netx to the pc */
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulResultData;


	// construct the command
	strCommand.Printf(wxT("DUMP %08lX LONG"), ulNetxAddress);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
		return -1;
	}

	// parse the result
	iMatches = sscanf((char*)pucData, "%8lX: %8lX ", &ulResultAddress, &ulResultData);
	free(pucData);
	if( iMatches!=2 )
	{
		wxLogError(wxT("strange response from netx!"));
		return -1;
	}
	if( ulResultAddress!=ulNetxAddress )
	{
		wxLogError(wxT("response does not match request!"));
		return -1;
	}

	*pulData = ulResultData;
	return 0;
}


/* read a byte array from the netx to the pc */
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulResultData;
	unsigned long ulBytesLeft;
	unsigned long ulChunkSize;
	unsigned long ulChunkCnt;
	unsigned long ulExpectedAddress;
	char *pcParsePtr;
	char *pcEndPtr;
	unsigned int uiByte;
	int iResult;
	char cEol;


	// expect error
	iResult = -1;

	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX %08lX BYTE"), ulNetxAddress, ulSize);

	// send the command
	tResult = romloader_usb_sendCommand(tHandle, strCommand);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// get the response
		tResult = romloader_usb_getNetxData(tHandle, &pucData, &uiDataLen, L, iLuaCallbackTag, pvCallbackUserData);
		if( tResult!=netxUsbState_Ok )
		{
			wxLogError(wxT("failed to receive command response!"));
			wxLogError( romloader_usb_getErrorString(tResult) );
		}
		else
		{
			// parse the result
			ulBytesLeft = ulSize;
			ulExpectedAddress = ulNetxAddress;
			pcParsePtr = (char*)pucData;
			pcEndPtr = pcParsePtr + uiDataLen;
			while( pcParsePtr<pcEndPtr && ulBytesLeft!=0 )
			{
				// get the number of expected bytes in the next row
				ulChunkSize = 16;
				if( ulChunkSize>ulBytesLeft )
				{
					ulChunkSize = ulBytesLeft;
				}
				// is enough input data left?
				if( (pcParsePtr+10+ulChunkSize*3)>=pcEndPtr )
				{
					wxLogError(wxT("strange response from netx!"));
					break;
				}
				// get the address
				iMatches = sscanf(pcParsePtr, "%8lX: ", &ulResultAddress);
				if( iMatches!=1 )
				{
					wxLogError(wxT("strange response from netx!"));
					break;
				}
				if( ulResultAddress!=ulExpectedAddress )
				{
					wxLogError(wxT("response does not match request!"));
					break;
				}
				// advance parse ptr to data part of the line
				pcParsePtr += 10;
				// get all bytes
				ulChunkCnt = ulChunkSize;
				while( ulChunkCnt!=0 )
				{
					// get one hex digit
					iMatches = sscanf(pcParsePtr, "%2X ", &uiByte);
					if( iMatches!=1 )
					{
						wxLogError(wxT("strange response from netx!"));
						break;
					}
					// advance parse ptr to data part of the line
					pcParsePtr += 3;
					*(pcData++) = (char)uiByte;
					// one number processed
					--ulChunkCnt;
				}
				ulBytesLeft -= ulChunkSize;
				// inc address
				ulExpectedAddress += ulChunkSize;
				// were all bytes processed?
				if( ulChunkCnt!=0 )
				{
					// no -> stop processing
					break;
				}
				// skip over the rest of the line
				while( pcParsePtr<pcEndPtr )
				{
					cEol = *(pcParsePtr++);
					if( cEol==0x00 || cEol==0x0a )
					{
						break;
					}
				}
			}
			free(pucData);
			// all bytes received?
			if( ulBytesLeft==0 )
			{
				// ok!
				iResult = 0;
			}
		}
	}

	return iResult;
}


/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iResult;


	// assume failure
	iResult = -1;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %02X BYTE"), ulNetxAddress, ucData);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// check the response
		if( uiDataLen==2 && pucData[0]==0x0a && pucData[1]=='>' )
		{
			// ok!
			iResult = 0;
		}
		else
		{
			wxLogError(wxT("strange response from netx!"));
		}
		free(pucData);
	}

	return iResult;
}


/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iResult;


	// assume failure
	iResult = -1;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %04X WORD"), ulNetxAddress, usData);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// check the response
		if( uiDataLen==2 && pucData[0]==0x0a && pucData[1]=='>' )
		{
			// ok!
			iResult = 0;
		}
		else
		{
			wxLogError(wxT("strange response from netx!"));
		}
		free(pucData);
	}

	return iResult;
}


/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iResult;


	// assume failure
	iResult = -1;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %08lX LONG"), ulNetxAddress, ulData);

	// send the command
	tResult = romloader_usb_executeCommand(pvHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// check the response
		if( uiDataLen==2 && pucData[0]==0x0a && pucData[1]=='>' )
		{
			// ok!
			iResult = 0;
		}
		else
		{
			wxLogError(wxT("strange response from netx!"));
		}
		free(pucData);
	}

	return iResult;
}


/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	int iResult;
	unsigned char *pucData;
	unsigned int uiDataLen;


	// expect error
	iResult = -1;

	// send the command
	tResult = romloader_usb_load(pvHandle, (unsigned char*)pcData, ulSize, ulNetxAddress, L, iLuaCallbackTag, pvCallbackUserData);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// get handle
		tHandle = (netx_device_handle)pvHandle;

		// get the response
		tResult = romloader_usb_getNetxData(tHandle, &pucData, &uiDataLen, NULL, 0, NULL);
		if( tResult!=netxUsbState_Ok )
		{
			wxLogError(wxT("failed to get command response!"));
			wxLogError( romloader_usb_getErrorString(tResult) );
		}
		else
		{
			// check the response
			if( uiDataLen==2 && pucData[0]==0x0a && pucData[1]=='>' )
			{
				// ok!
				iResult = 0;
			}
			else
			{
				wxLogError(wxT("strange response from netx!"));
			}
			free(pucData);
		}
	}

	return iResult;
}


/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	tNetxUsbState tResult;
	int iResult;
	unsigned char *pucData;
	unsigned int uiDataLen;


	// expect error
	iResult = -1;

	// send the command
	tResult = romloader_usb_call(pvHandle, ulNetxAddress, ulParameterR0, &pucData, &uiDataLen, L, iLuaCallbackTag, pvCallbackUserData);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		wxLogError( romloader_usb_getErrorString(tResult) );
	}
	else
	{
		// check the response
		if( uiDataLen==2 && pucData[0]==0x0a && pucData[1]=='>' )
		{
			// ok!
			iResult = 0;
		}
		else
		{
			wxLogError(wxT("strange response from netx!"));
		}
		free(pucData);
	}

	return iResult;
}


/*-------------------------------------*/

