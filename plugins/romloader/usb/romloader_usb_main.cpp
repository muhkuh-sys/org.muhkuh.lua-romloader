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


#include "romloader_usb_main.h"
#include "_luaif/romloader_usb_wxlua_bindings.h"

#include <wx/wx.h>
#include <wx/regex.h>

#ifdef __WINDOWS__
	#define ETIMEDOUT 116
#endif

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("USB Bootloader"),
	wxT(""),
	{ 0, 0, 1 }
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

bool romloader_usb_isDeviceNetx(struct usb_device *ptDevice)
{
	bool fDeviceIsNetx;


	if( ptDevice==NULL )
	{
		return false;
	}

	fDeviceIsNetx  = true;
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceClass==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceSubClass==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceProtocol==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.idVendor==0x0cc4 );
	fDeviceIsNetx &= ( ptDevice->descriptor.idProduct==0x0815 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bcdDevice==0x0100 );

	return fDeviceIsNetx;
}

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


void romloader_usb_close_instance(wxString &strInterface)
{
}


/*-------------------------------------*/

romloader *romloader_usb_create(void *pvHandle)
{
	struct usb_device *ptNetxDev;
	romloader *ptInstance = NULL;
	wxString strName;
	wxString strTyp;


	if( pvHandle!=NULL )
	{
		// get handle
		ptNetxDev = (struct usb_device *)pvHandle;

		// create the new instance
		strTyp = plugin_desc.strPluginId;
		strName.Printf("romloader_usb_%08x_%02x", ptNetxDev->bus->location, ptNetxDev->devnum);
		ptInstance = new romloader_usb(strName, strTyp, NULL, ptNetxDev, romloader_usb_close_instance, m_ptLuaState);
	}

	return ptInstance;
}


/*-------------------------------------*/


romloader_usb::romloader_usb(wxString strName, wxString strTyp, const romloader_functioninterface *ptFn, struct usb_device *ptNetxDev, romloader_usb_plugin_fn_close_instance fn_close, wxLuaState *ptLuaState)
 : romloader(strName, strTyp, NULL, NULL, NULL, ptLuaState)
 , m_fIsConnected(false)
 , m_ptUsbDev(ptNetxDev)
 , m_ptUsbDevHandle(NULL)
 , m_fn_usb_close(fn_close)
{
	m_strMe.Printf(wxT("romloader_usb(%p): "), this);

	wxLogMessage(m_strMe + wxT("constructor"));

	m_strInterface = strName;
	m_strTyp = strTyp;
}


romloader_usb::~romloader_usb(void)
{
	wxLogMessage(m_strMe + wxT("destructor"));

	disconnect();

	if( m_fn_usb_close!=NULL )
	{
		m_fn_usb_close(m_strInterface);
	}
}


bool romloader_usb::chip_init(void)
{
	bool fResult;


	switch( m_tChiptyp )
	{
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// aboot does not set the serial vectors
			write_data32(0x10001ff0, 0x00200582|1);		// get: usb_receiveChar
			write_data32(0x10001ff4, 0x0020054e|1);		// put: usb_sendChar
			write_data32(0x10001ff8, 0);			// peek: none
			write_data32(0x10001ffc, 0x00200566|1);		// flush: usb_sendFinish
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_UNKNOWN:
		fResult = false;
		break;
	}

	return fResult;
}


/* open the connection to the device */
void romloader_usb::connect(void)
{
	tNetxUsbState tResult;
	wxString strResponse;
	wxString strErrorMsg;


	if( m_ptUsbDev!=NULL && m_fIsConnected==false )
	{
		// open device and provide cancel function with this class as parameter
		tResult = libusb_openDevice(m_ptUsbDev);
		if( tResult!=netxUsbState_Ok )
		{
			strErrorMsg = m_strMe + wxT("failed to open the usb connection to the netx: ") + usb_getErrorString(tResult);
		}
		else
		{
			// get netx welcome message
			tResult = usb_getNetxData(strResponse, NULL, 0, NULL);
			if( tResult!=netxUsbState_Ok )
			{
				wxLogMessage(m_strMe + wxT("failed to receive netx response, trying to reset netx: ") + usb_getErrorString(tResult) );

				// try to reset the device and try again
				tResult = libusb_resetDevice();
				if( tResult!=netxUsbState_Ok )
				{
					strErrorMsg = m_strMe + wxT("failed to reset the netx, giving up!");
				}
				// open device and provide cancel function with this class as parameter
				wxLogMessage(m_strMe + wxT("reset ok!"));

				tResult = libusb_openDevice(m_ptUsbDev);
				if( tResult!=netxUsbState_Ok )
				{
					strErrorMsg = m_strMe + wxT("failed to open the usb connection to the netx: ") + usb_getErrorString(tResult);
				}
				else
				{
					// get netx welcome message
					tResult = usb_getNetxData(strResponse, NULL, 0, NULL);
					if( tResult!=netxUsbState_Ok )
					{
						strErrorMsg = m_strMe + wxT("failed to receive netx response, trying to reset netx: ") + usb_getErrorString(tResult);
						libusb_closeDevice();
					}
				}
			}

			if( tResult==netxUsbState_Ok )
			{
				wxLogInfo(m_strMe + wxT("netx response: ") + strResponse);
				if( detect_chiptyp()!=true )
				{
					strErrorMsg = m_strMe + wxT("failed to detect chiptyp!");
				}
				else if( chip_init()!=true )
				{
					strErrorMsg = m_strMe + wxT("failed to init chip!");
				}
				else
				{
					m_fIsConnected = true;
				}
			}

			if( m_fIsConnected!=true )
			{
				libusb_closeDevice();
			}
		}

		if( m_fIsConnected!=true )
		{
			wxLogError(m_strMe + strErrorMsg);
			m_ptLuaState->wxlua_Error(strErrorMsg);
		}
	}
}


/* close the connection to the device */
void romloader_usb::disconnect(void)
{
	tNetxUsbState tResult;
	wxString strMsg;


	if( m_fIsConnected==true )
	{
		wxLogMessage(m_strMe + wxT("disconnect"));
		tResult = libusb_closeDevice();
		if( tResult==netxUsbState_Ok )
		{
			m_fIsConnected = false;
		}
		else
		{
			wxLogError(m_strMe + wxT("failed to close the usb connection to the netx: ") + usb_getErrorString(tResult) );
		}
	}
}


/* returns the connection state of the device */
bool romloader_usb::is_connected(void) const
{
	return m_fIsConnected;
}


/* read a byte (8bit) from the netx to the pc */
double romloader_usb::read_data08(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{2}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX BYTE"), ulNetxAddress);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg.Printf(wxT("failed to send command: ") + usb_getErrorString(tResult));
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg.Printf(wxT("strange response from device:") + strResponse);
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg.Printf(wxT("failed to extract address from response:") + strResponse);
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg.Printf(wxT("address does not match request:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(wxT("failed to extract value from response:") + strResponse);
			}
			else
			{
				wxLogMessage(m_strMe + wxT("read_data08: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


/* read a word (16bit) from the netx to the pc */
double romloader_usb::read_data16(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{4}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX WORD"), ulNetxAddress);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg.Printf(wxT("failed to send command: ") + usb_getErrorString(tResult));
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg.Printf(wxT("strange response from device:") + strResponse);
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg.Printf(wxT("failed to extract address from response:") + strResponse);
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg.Printf(wxT("address does not match request:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(wxT("failed to extract value from response:") + strResponse);
			}
			else
			{
				wxLogMessage(m_strMe + wxT("read_data16: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


/* read a long (32bit) from the netx to the pc */
double romloader_usb::read_data32(double dNetxAddress)
{
	unsigned long ulNetxAddress;
	unsigned long ulValue;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	wxString strHexValue;
	wxRegEx reDumpResponse(wxT("([0-9A-Fa-f]{8}): ([0-9A-Fa-f]{8}) "));
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulValue = 0;

	// expect failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX"), ulNetxAddress);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg.Printf(wxT("failed to send command: ") + usb_getErrorString(tResult));
	}
	else if( reDumpResponse.Matches(strResponse)!=true )
	{
		strErrorMsg.Printf(wxT("strange response from device:") + strResponse);
	}
	else
	{
		strHexValue = reDumpResponse.GetMatch(strResponse, 1);
		if( strHexValue.ToULong(&ulValue, 16)!=true )
		{
			strErrorMsg.Printf(wxT("failed to extract address from response:") + strResponse);
		}
		else if( ulValue!=ulNetxAddress )
		{
			strErrorMsg.Printf(wxT("address does not match request:") + strResponse);
		}
		else
		{
			strHexValue = reDumpResponse.GetMatch(strResponse, 2);
			if( strHexValue.ToULong(&ulValue, 16)!=true )
			{
				strErrorMsg.Printf(wxT("failed to extract value from response:") + strResponse);
			}
			else
			{
				wxLogMessage(m_strMe + wxT("read_data32: 0x%08lx = 0x%08lx"), ulNetxAddress, ulValue);
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return (double)ulValue;
}


/* read a byte array from the netx to the pc */
wxString romloader_usb::read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulSize;
	tNetxUsbState tResult;
	wxString strCommand;
	bool fOk;
	wxString strErrorMsg;
	wxString strResponse;
	wxString strData;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulSize = (unsigned long)dSize;

	// expect error
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX %08lX BYTE"), ulNetxAddress, ulSize);

	// send the command
	tResult = usb_sendCommand(strCommand);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = wxT("failed to send command: ") + usb_getErrorString(tResult);
	}
	else
	{
		// get the response
		tResult = usb_getNetxData(strResponse, L, iLuaCallbackTag, pvCallbackUserData);
		if( tResult!=netxUsbState_Ok )
		{
			strErrorMsg = wxT("failed to receive command response: ") + usb_getErrorString(tResult);
		}
		else
		{
			fOk = parseDump(strResponse.To8BitData(), strResponse.Len(), ulNetxAddress, ulSize, strData);
			if( fOk!=true )
			{
				strErrorMsg = strData;
				strData.Empty();
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}

	return strData;
}


/* write a byte (8bit) from the pc to the netx */
void romloader_usb::write_data08(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned char ucData;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	unsigned char *pucData;
	unsigned int uiDataLen;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ucData = (unsigned char)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %02X BYTE"), ulNetxAddress, ucData);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = wxT("failed to send command: ") + usb_getErrorString(tResult);
	}
	else if( strResponse.Cmp(wxT("\n>"))==0 )
	{
		wxLogMessage(m_strMe + wxT("write_data08: 0x%08lx = 0x%02x"), ulNetxAddress, ucData);
		fOk = true;
	}
	else
	{
		strErrorMsg = wxT("strange response from netx: ") + strResponse;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_usb::write_data16(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned short usData;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	unsigned char *pucData;
	unsigned int uiDataLen;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	usData = (unsigned short)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %04X WORD"), ulNetxAddress, usData);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = wxT("failed to send command: ") + usb_getErrorString(tResult);
	}
	else if( strResponse.Cmp(wxT("\n>"))==0 )
	{
		wxLogMessage(m_strMe + wxT("write_data16: 0x%08lx = 0x%04x"), ulNetxAddress, usData);
		fOk = true;
	}
	else
	{
		strErrorMsg = wxT("strange response from netx: ") + strResponse;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_usb::write_data32(double dNetxAddress, double dData)
{
	unsigned long ulNetxAddress;
	unsigned long ulData;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strCommand;
	wxString strResponse;
	unsigned char *pucData;
	unsigned int uiDataLen;
	bool fOk;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulData = (unsigned long)dData;

	// assume failure
	fOk = false;

	// construct the command
	strCommand.Printf(wxT("FILL %08lX %08X"), ulNetxAddress, ulData);

	// send the command
	tResult = usb_executeCommand(strCommand, strResponse);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = wxT("failed to send command: ") + usb_getErrorString(tResult);
	}
	else if( strResponse.Cmp(wxT("\n>"))==0 )
	{
		wxLogMessage(m_strMe + wxT("write_data32: 0x%08lx = 0x%08lx"), ulNetxAddress, ulData);
		fOk = true;
	}
	else
	{
		strErrorMsg = wxT("strange response from netx: ") + strResponse;
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* write a byte array from the pc to the netx */
void romloader_usb::write_image(double dNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	tNetxUsbState tResult;
	bool fOk;
	wxString strErrorMsg;
	wxString strResponse;


	ulNetxAddress = (unsigned long)dNetxAddress;

	// expect error
	fOk = false;

	// send the command
	tResult = usb_load(strData.To8BitData(), strData.Len(), ulNetxAddress, L, iLuaCallbackTag, pvCallbackUserData);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = wxT("failed to send load command: ") + usb_getErrorString(tResult);
	}
	else
	{
		// get the response
		tResult = usb_getNetxData(strResponse, NULL, 0, NULL);
		if( tResult!=netxUsbState_Ok )
		{
			strErrorMsg = wxT("failed to get command response: ") + usb_getErrorString(tResult);
		}
		else
		{
			// check the response
			if( strResponse.Cmp(wxT("\n>"))==0 )
			{
				// ok!
				fOk = true;
			}
			else
			{
				strErrorMsg = wxT("strange response from netx: ") + strResponse;
			}
		}
	}

	if( fOk!=true )
	{
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/* call routine */
void romloader_usb::call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	unsigned long ulNetxAddress;
	unsigned long ulParameterR0;
	tNetxUsbState tResult;
	wxString strErrorMsg;
	wxString strResponse;


	ulNetxAddress = (unsigned long)dNetxAddress;
	ulParameterR0 = (unsigned long)dParameterR0;

	// send the command
	tResult = usb_call(ulNetxAddress, ulParameterR0, L, iLuaCallbackTag, pvCallbackUserData);
	if( tResult!=netxUsbState_Ok )
	{
		strErrorMsg = m_strMe + wxT("failed to send command: ") + usb_getErrorString(tResult);
		wxLogError(m_strMe + strErrorMsg);
		m_ptLuaState->wxlua_Error(strErrorMsg);
	}
}


/*-------------------------------------*/


bool romloader_usb::parseDump(const char *pcDump, size_t sizDumpLen, unsigned long ulAddress, unsigned long ulSize, wxString &strData)
{
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulResultData;
	unsigned long ulBytesLeft;
	unsigned long ulChunkSize;
	unsigned long ulChunkCnt;
	unsigned long ulExpectedAddress;
	const char *pcParsePtr;
	const char *pcEndPtr;
	unsigned int uiByte;
	char cEol;
	bool fOk;
	wxString strErrorMsg;


	// expect failure
	fOk = false;

	// clear output data
	strData.Empty();
	strData.Alloc(ulSize);

	// parse the result
	ulBytesLeft = ulSize;
	ulExpectedAddress = ulAddress;
	pcParsePtr = pcDump;
	pcEndPtr = pcParsePtr + sizDumpLen;
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
			strErrorMsg = wxT("dump too short!");
			break;
		}

		// get the address
		iMatches = sscanf(pcParsePtr, "%8lX: ", &ulResultAddress);
		if( iMatches!=1 )
		{
			strErrorMsg.Printf(wxT("failed to parse dump address at offset %d: '%s'"), pcParsePtr-pcDump, pcDump);
			break;
		}
		if( ulResultAddress!=ulExpectedAddress )
		{
			strErrorMsg.Printf(wxT("response address does not match request at offset %d: '%s'"), pcParsePtr-pcDump, pcDump);
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
				strErrorMsg.Printf(wxT("failed to parse hex digits at offset %d: '%s'"), pcParsePtr-pcDump, pcDump);
				break;
			}
			strData.Append((char)uiByte);
			// advance parse ptr to data part of the line
			pcParsePtr += 3;
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

	// all bytes received?
	if( ulBytesLeft==0 )
	{
		// ok!
		fOk = true;
	}
	else
	{
		wxLogError(m_strMe + strErrorMsg);
		strData = strErrorMsg;
	}

	return fOk;
}


tNetxUsbState romloader_usb::usb_load(const char *pcData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	const unsigned char *pucDataCnt, *pucDataEnd;
	tNetxUsbState tResult;
	unsigned int uiCrc;
	wxString strCommand;
	unsigned char aucBufSend[64];
	unsigned char aucBufRec[64];
	size_t sizChunkSize;
	bool fIsRunning;
	long lBytesProcessed;


	pucDataCnt = (const unsigned char*)pcData;
	pucDataEnd = pucDataCnt + sizDataLen;
	// generate crc checksum
	uiCrc = 0xffff;
	// loop over all bytes
	while( pucDataCnt<pucDataEnd )
	{
		uiCrc = crc16(uiCrc, *(pucDataCnt++));
	}

	// generate load command
	strCommand.Printf(wxT("LOAD %08lX %08X %04X"), ulLoadAdr, sizDataLen, uiCrc);

	// send the command
	tResult = usb_sendCommand(strCommand);
	if( tResult==netxUsbState_Ok )
	{
		// now send the data part
		pucDataCnt = (const unsigned char*)pcData;
		lBytesProcessed = 0;
		while( pucDataCnt<pucDataEnd )
		{
			// get the size of the next data chunk
			sizChunkSize = pucDataEnd - pucDataCnt;
			if( sizChunkSize>63 )
			{
				sizChunkSize = 63;
			}
			// copy data to the packet
			memcpy(aucBufSend+1, pucDataCnt, sizChunkSize);
			aucBufSend[0] = sizChunkSize+1;

			fIsRunning = callback_long(L, iLuaCallbackTag, lBytesProcessed, pvCallbackUserData);
			if( fIsRunning!=true )
			{
				tResult = netxUsbState_Cancel;
				break;
			}

			tResult = libusb_exchange(aucBufSend, aucBufRec);
			if( tResult!=netxUsbState_Ok )
			{
				break;
			}
			pucDataCnt += sizChunkSize;
			lBytesProcessed += sizChunkSize;
		}

		if( pucDataCnt==pucDataEnd )
		{
			tResult = netxUsbState_Ok;
		}
	}

	return tResult;
}


tNetxUsbState romloader_usb::usb_call(unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char aucSend[64];
	unsigned char aucRec[64];
	bool fIsRunning;
	wxString strResponse;
	wxString strCallbackData;
	size_t sizChunkRead;
	unsigned char *pucBuf;
	unsigned char *pucCnt, *pucEnd;
	unsigned char aucSbuf[2] = { 0, 0 };


	// construct the "call" command
	strCommand.Printf(wxT("CALL %08lX %08X"), ulNetxAddress, ulParameterR0);

	// send the command
	tResult = usb_sendCommand(strCommand);
	if( tResult==netxUsbState_Ok )
	{
		// wait for the call to finish
		do
		{
			// send handshake
			aucSend[0] = 0x00;
			tResult = libusb_writeBlock(aucSend, 64, 200);
			if( tResult==netxUsbState_Ok )
			{
				do
				{
					// execute callback
					fIsRunning = callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
					strCallbackData.Empty();
					if( fIsRunning!=true )
					{
						tResult = netxUsbState_Cancel;
					}
					else
					{
						// look for data from netx
						tResult = libusb_readBlock(aucRec, 64, 200);
						if( tResult==netxUsbState_Ok )
						{
							tResult = netxUsbState_Timeout;

							// received netx data, check for prompt
							sizChunkRead = aucRec[0];
							if( sizChunkRead>1 && sizChunkRead<=64 )
							{
								// get data
								// NOTE: use Append to make a real copy
								strCallbackData.Append( wxString::From8BitData((const char*)(aucRec+1), sizChunkRead-1) );

								// last packet has '\n>' at the end
								if( sizChunkRead>2 && aucRec[sizChunkRead-2]=='\n' && aucRec[sizChunkRead-1]=='>' )
								{
									// send the rest of the data
									callback_string(L, iLuaCallbackTag, strCallbackData, pvCallbackUserData);
									tResult = netxUsbState_Ok;
								}
							}
							break;
						}
					}
				} while( tResult==netxUsbState_Timeout );
			}
		} while( tResult==netxUsbState_Timeout );
	}

	if( tResult==netxUsbState_Ok )
	{
		aucSend[0] = 0x00;
		tResult = libusb_exchange(aucSend, aucRec);
	}

	return tResult;
}


tNetxUsbState romloader_usb::usb_sendCommand(wxString strCommand)
{
	tNetxUsbState tResult;
	size_t sizCmdLen;
	unsigned char abSend[64];
	unsigned char abRec[64];
	int iCmdLen;


	// check the command size
	// Commands must fit into one usb packet of 64 bytes, the first byte
	// is the length and the last byte must be 0x0a. This means the max
	// command size is 62 bytes.
	sizCmdLen = strCommand.Len();
	if( sizCmdLen>62 )
	{
		wxLogMessage(m_strMe + wxT("command exceeds maximum length of 62 chars: ") + strCommand);
		tResult = netxUsbState_CommandTooLong;
	}
	else
	{
		wxLogMessage(m_strMe + wxT("send command '%s'"), strCommand.fn_str());

		// construct the command
		memcpy(abSend+1, strCommand.To8BitData(), sizCmdLen);
		abSend[0] = sizCmdLen+2;
		abSend[sizCmdLen+1] = 0x0a;

		// send the command
		tResult = libusb_exchange(abSend, abRec);
		if( tResult==netxUsbState_Ok )
		{
			// terminate command
			abSend[0] = 0x00;
			tResult = libusb_exchange(abSend, abRec);
		}
	}

	return tResult;
}


tNetxUsbState romloader_usb::usb_getNetxData(wxString &strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	tNetxUsbState tResult;
	unsigned char buf_send[64];
	unsigned char buf_rec[64];
	unsigned int ulLineLen;


	// init results
	strData.Empty();

	// receive netx data
	do
	{
		buf_send[0] = 0x00;
		tResult = libusb_exchange(buf_send, buf_rec);
		if( tResult!=netxUsbState_Ok )
		{
			break;
		}
		else
		{
			ulLineLen = buf_rec[0];

			if( ulLineLen!=0 )
			{
				strData.Append(wxString::From8BitData((const char*)(buf_rec+1), ulLineLen-1));
			}
		}
	} while( ulLineLen!=0 );

	return tResult;
}


tNetxUsbState romloader_usb::usb_executeCommand(wxString strCommand, wxString &strResponse)
{
	tNetxUsbState tResult;


	// send the command
	tResult = usb_sendCommand(strCommand);
	if( tResult==netxUsbState_Ok )
	{
		// get the response
		tResult = usb_getNetxData(strResponse, NULL, 0, NULL);
	}

	return tResult;
}


wxString romloader_usb::usb_getErrorString(tNetxUsbState tResult)
{
	wxString strMessage;


	switch(tResult)
	{
	case netxUsbState_Ok:
		strMessage = _("ok");
		break;

	case netxUsbState_OutOfMemory:
		strMessage = _("out of memory");
		break;

	case netxUsbState_DeviceNotFound:
		strMessage = _("device not found");
		break;

	case netxUsbState_DeviceBusy:
		strMessage = _("device is busy");
		break;

	case netxUsbState_ReadError:
		strMessage = _("read error");
		break;

	case netxUsbState_WriteError:
		strMessage = _("write error");
		break;

	case netxUsbState_BlockSizeError:
		strMessage = _("block size error");
		break;

	case netxUsbState_CommandTooLong:
		strMessage = _("command too long");
		break;

	case netxUsbState_Timeout:
		strMessage = _("timeout");
		break;

	case netxUsbState_Cancel:
		strMessage = _("canceled");
		break;

	case netxUsbState_Error:
		strMessage = _("error");
		break;

	default:
		strMessage.Printf(_("unknown errorcode: %d"), tResult);
		break;
	}

	return strMessage;
}


/*-------------------------------------*/


tNetxUsbState romloader_usb::libusb_openDevice(struct usb_device *ptNetxDev)
{
	usb_dev_handle *ptDevHandle;
	int iRet;
	tNetxUsbState tResult;


	// open the netx device
	ptDevHandle = usb_open(ptNetxDev);
	if( ptDevHandle==NULL )
	{
		// can't open handle
		wxLogError(m_strMe + _("failed to open handle! was the device unplugged after the last scan?"));
		tResult = netxUsbState_DeviceNotFound;
	}
	else
	{
		// set the configuration
		iRet = usb_set_configuration(ptDevHandle, 1);
		if( iRet!=0 )
		{
			// close the device
			usb_close(ptDevHandle);
			// failed to claim interface, assume busy
			wxLogError(m_strMe + _("failed to configure the usb interface! is the device in use?"));
			tResult = netxUsbState_DeviceBusy;
		}
		else
		{
			// try to claim the interface
			iRet = usb_claim_interface(ptDevHandle, 0);
			if( iRet!=0 )
			{
				// close the device
				usb_close(ptDevHandle);
				// failed to claim interface, assume busy
				wxLogError(m_strMe + _("failed to claim the usb interface! is the device in use?"));
				tResult = netxUsbState_DeviceBusy;
			}
			else
			{
				// fill the device structure
				m_ptUsbDevHandle = ptDevHandle;
				tResult = netxUsbState_Ok;
			}
		}
	}

	return tResult;
}


tNetxUsbState romloader_usb::libusb_closeDevice(void)
{
	tNetxUsbState tResult;
	int iRet;


	// expect failure
	tResult = netxUsbState_Error;

	// release the interface
	iRet = usb_release_interface(m_ptUsbDevHandle, 0);
	if( iRet!=0 )
	{
		// failed to release interface
		wxLogError(m_strMe + wxT("failed to release the usb interface"));
	}
	else
	{
		// close the netx device
		iRet = usb_close(m_ptUsbDevHandle);
		if( iRet!=0 )
		{
			// failed to close handle
			wxLogError(m_strMe + wxT("failed to release the usb handle"));
		}
		else
		{
			tResult = netxUsbState_Ok;
		}
	}

	return tResult;
}


tNetxUsbState romloader_usb::libusb_resetDevice(void)
{
	tNetxUsbState tResult;
	int iRet;


	// expect success
	tResult = netxUsbState_Ok;

	iRet = usb_reset(m_ptUsbDevHandle);
	if( iRet==0 )
	{
		usb_close(m_ptUsbDevHandle);
	}
	else if( iRet==-ENODEV )
	{
		// the old device is already gone -> that's good, ignore the error
		usb_close(m_ptUsbDevHandle);
	}
	else
	{
		tResult = netxUsbState_Error;
	}

	return tResult;
}


tNetxUsbState romloader_usb::libusb_readBlock(unsigned char *pcReceiveBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;


	// check blocksize
	if( uiSize>64 )
	{
		return netxUsbState_BlockSizeError;
	}
	iSize = (int)uiSize;

	iRet = usb_bulk_read(m_ptUsbDevHandle, 0x81, (char*)pcReceiveBuffer, iSize, iTimeoutMs);
	if( iRet==-ETIMEDOUT )
	{
		return netxUsbState_Timeout;
	}
	else if( iRet!=iSize )
	{
		return netxUsbState_ReadError;
	}
	else
	{
		return netxUsbState_Ok;
	}
}


tNetxUsbState romloader_usb::libusb_writeBlock(const unsigned char *pcSendBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;


	// check blocksize
	if( uiSize>64 )
	{
		return netxUsbState_BlockSizeError;
	}
	iSize = (int)uiSize;

	iRet = usb_bulk_write(m_ptUsbDevHandle, 0x01, (char*)pcSendBuffer, iSize, iTimeoutMs);
	if( iRet==-ETIMEDOUT )
	{
		return netxUsbState_Timeout;
	}
	else if( iRet!=iSize )
	{
		return netxUsbState_WriteError;
	}
	else
	{
		return netxUsbState_Ok;
	}
}



tNetxUsbState romloader_usb::libusb_exchange(const unsigned char *pcSendBuffer, unsigned char *pcReceiveBuffer)
{
	tNetxUsbState tResult;


	tResult = libusb_writeBlock(pcSendBuffer, 64, 200);
	if( tResult==netxUsbState_Ok )
	{
		tResult = libusb_readBlock(pcReceiveBuffer, 64, 200);
	}
        return tResult;
}



