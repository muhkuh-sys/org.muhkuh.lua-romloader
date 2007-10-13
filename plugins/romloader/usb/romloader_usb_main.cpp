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


#include <usb.h>
typedef usb_dev_handle *netx_device_handle;

#ifdef __WXMSW__
#define ETIMEDOUT WSAETIMEDOUT
#define snprintf _snprintf
#endif

/*-------------------------------------*/
/* some local prototypes */

bool IsDeviceNetx(struct usb_device *ptDevice);

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

tNetxUsbState libnetxusbmon_executeCommand(netx_device_handle tHandle, wxString strCommand, unsigned char **ppucData, unsigned int *puiDataLen);

/*-------------------------------------*/

const muhkuh_plugin_desc plugin_desc =
{
	"netX500 USB Bootloader",
	"romloader_usb_netx500",
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

int fn_init(wxLog *ptLogTarget)
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

	strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
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
			fDeviceIsNetx = IsDeviceNetx(ptDev);
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

static tNetxUsbState libnetxusbmon_openByDevice(netx_device_handle *phNetxUsbMon, struct usb_device *ptNetxDev)
{
	netx_device_handle tDevHandle;
	int iRet;


	// open the netx device
	tDevHandle = usb_open(ptNetxDev);
	if( tDevHandle==NULL )
	{
		// can't open handle
		wxLogError(wxT("failed to open handle! was the device unplugged after the last scan?"));
		return netxUsbState_DeviceNotFound;
	}

	// set the configuration
	iRet = usb_set_configuration(tDevHandle, 1);
	if( iRet!=0 )
	{
		// close the device
		usb_close(tDevHandle);
		// failed to claim interface, assume busy
		wxLogError(wxT("failed to configure the usb interface! is the device in use?"));
		return netxUsbState_DeviceBusy;
	}

	// try to claim the interface
	iRet = usb_claim_interface(tDevHandle, 0);
	if( iRet!=0 )
	{
		// close the device
		usb_close(tDevHandle);
		// failed to claim interface, assume busy
		wxLogError(wxT("failed to claim the usb interface! is the device in use?"));
		return netxUsbState_DeviceBusy;
	}

	// fill the device structure
	*phNetxUsbMon = tDevHandle;

	return netxUsbState_Ok;
}


tNetxUsbState libnetxusbmon_close(netx_device_handle hNetxUsbMon)
{
	int iRet;


	// release the interface
	iRet = usb_release_interface(hNetxUsbMon, 0);
	if( iRet!=0 )
	{
		// failed to release interface
		wxLogError(wxT("failed to release the usb interface"));
		return netxUsbState_Error;
	}

	// close the netx device
	iRet = usb_close(hNetxUsbMon);
	if( iRet!=0 )
	{
		// failed to close handle
		wxLogError(wxT("failed to release the usb handle"));
		return netxUsbState_Error;
	}

	return netxUsbState_Ok;
}


tNetxUsbState libnetxusbmon_readBlock(netx_device_handle hNetxUsbMon, unsigned char *pcReceiveBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;


	// check blocksize
	if( uiSize>64 )
	{
		return netxUsbState_BlockSizeError;
	}
	iSize = (int)uiSize;

	iRet = usb_bulk_read(hNetxUsbMon, 0x81, (char*)pcReceiveBuffer, iSize, iTimeoutMs);
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


tNetxUsbState libnetxusbmon_writeBlock(netx_device_handle hNetxUsbMon, const unsigned char *pcSendBuffer, unsigned int uiSize, int iTimeoutMs)
{
	int iRet;
	int iSize;


	// check blocksize
	if( uiSize>64 )
	{
		return netxUsbState_BlockSizeError;
	}
	iSize = (int)uiSize;

	iRet = usb_bulk_write(hNetxUsbMon, 0x01, (char*)pcSendBuffer, iSize, iTimeoutMs);
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


tNetxUsbState libnetxusbmon_exchange(netx_device_handle hNetxUsbMon, const unsigned char *pcSendBuffer, unsigned char *pcReceiveBuffer)
{
	tNetxUsbState tResult;


	do
	{
		tResult = libnetxusbmon_writeBlock(hNetxUsbMon, pcSendBuffer, 64, 100);
		if( tResult==netxUsbState_Timeout )
		{
			wxYield();
		}
	} while( tResult==netxUsbState_Timeout );

	if( tResult!=netxUsbState_Ok )
	{
		return tResult;
	}


	do
	{
		tResult = libnetxusbmon_readBlock(hNetxUsbMon, pcReceiveBuffer, 64, 100);
		if( tResult==netxUsbState_Timeout )
		{
			wxYield();
		}
	} while( tResult==netxUsbState_Timeout );

        return tResult;
}


tNetxUsbState libnetxusbmon_getNetxData(netx_device_handle hNetxUsbMon, unsigned char **ppcData, unsigned int *pulDataLen)
{
	tNetxUsbState tResult;
	unsigned char buf_send[65];
	unsigned char buf_rec[65];
	unsigned int ulLinelen;
	unsigned char *pcData;
	unsigned char *pcDataNew;
	unsigned int ulDataLen;
	unsigned int ulDataLenNew;
	unsigned int ulDataLenAlloc;


	// alloc initial buffer
	ulDataLen = 0;
	ulDataLenAlloc = 4096;
	pcData = (unsigned char*)malloc(ulDataLenAlloc);
	if( pcData==NULL )
	{
		return netxUsbState_OutOfMemory;
	}

	// receive netx data
	do
	{
		buf_send[0] = 0x00;
		tResult = libnetxusbmon_exchange(hNetxUsbMon, buf_send, buf_rec);
		if( tResult!=netxUsbState_Ok )
		{
			return tResult;
		}

		ulLinelen = buf_rec[0];

		if( ulLinelen!=0 )
		{
			// get new size of databuffer
			ulDataLenNew = ulDataLen + ulLinelen - 1;

			// is enough space left?
			if( ulDataLenNew>ulDataLenAlloc )
			{
				ulDataLenAlloc <<= 2;
				// test for overflow
				if( ulDataLenNew>ulDataLenAlloc )
				{
					free(pcData);
					return netxUsbState_OutOfMemory;
				}
				pcDataNew = (unsigned char*)realloc(pcData, ulDataLenAlloc);
				if( pcDataNew==NULL )
				{
					free(pcData);
					return netxUsbState_OutOfMemory;
				}
				pcData = pcDataNew;
			}

			// copy data
			memcpy(pcData+ulDataLen, buf_rec+1, ulLinelen);

			ulDataLen = ulDataLenNew;
		}
	} while( ulLinelen!=0 );

	*ppcData = pcData;
	*pulDataLen = ulDataLen;

	wxLogMessage(wxT("usb: received response '") + wxString::From8BitData((const char*)pcData, ulDataLen) + wxT("'"));

	return netxUsbState_Ok;
}


tNetxUsbState libnetxusbmon_executeCommand(netx_device_handle tHandle, wxString strCommand, unsigned char **ppucData, unsigned int *puiDataLen)
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
		return netxUsbState_CommandTooLong;
	}

	wxLogMessage(wxT("usb: send command '") + strCommand + wxT("'"));

	// construct the command
	memcpy(abSend+1, strCommand.To8BitData(), sizCmdLen);
	abSend[0] = sizCmdLen+2;
	abSend[sizCmdLen+1] = 0x0a;

	// send the command
	tResult = libnetxusbmon_exchange(tHandle, abSend, abRec);
	if( tResult==netxUsbState_Ok )
	{
		// terminate command
		abSend[0] = 0x00;
		tResult = libnetxusbmon_exchange(tHandle, abSend, abRec);
		if( tResult==netxUsbState_Ok )
		{
			// get the response
			tResult = libnetxusbmon_getNetxData(tHandle, ppucData, puiDataLen);
		}
	}

	return tResult;
}


/*-------------------------------------*/

static void romloader_usb_close_instance(void *pvHandle)
{
	netx_device_handle tHandle;


	// get handle
	tHandle = (netx_device_handle)pvHandle;
	libnetxusbmon_close(tHandle);
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
	tResult = libnetxusbmon_openByDevice(&tHandle, ptNetxDev);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to open the usb connection to the netx"));
	}
	else
	{
		// get netx welcome message
		tResult = libnetxusbmon_getNetxData(tHandle, &pucData, &uiDataLen);
		if( tResult!=netxUsbState_Ok )
		{
			wxLogError(wxT("failed to receive netx response!"));
		}
		else
		{
			wxLogInfo(wxString::From8BitData((const char*)pucData, uiDataLen));
			free(pucData);

			/* create the new instance */
			strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
			strName.Printf("romloader_usb_%08x_%02x", ptNetxDev->bus->location, ptNetxDev->devnum);
			ptInstance = new romloader(strName, strTyp, &tFunctionInterface, tHandle, romloader_usb_close_instance, m_ptLuaState);
//			atInstanceCfg[uiIdx].fIsUsed = true;
		}
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
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned int uiResultData;


	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX BYTE"), ulNetxAddress);

	// send the command
	tResult = libnetxusbmon_executeCommand(tHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		return -1;
	}

	// parse the result
	iMatches = sscanf((char*)pucData, "%8lX: %2X ", &ulResultAddress, &uiResultData);
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

	*pbData = (unsigned char)uiResultData;
	return 0;
}


/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned int uiResultData;


	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX WORD"), ulNetxAddress);

	// send the command
	tResult = libnetxusbmon_executeCommand(tHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
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
	netx_device_handle tHandle;
	tNetxUsbState tResult;
	wxString strCommand;
	unsigned char *pucData;
	unsigned int uiDataLen;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulResultData;


	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// construct the command
	strCommand.Printf(wxT("DUMP %08lX LONG"), ulNetxAddress);

	// send the command
	tResult = libnetxusbmon_executeCommand(tHandle, strCommand, &pucData, &uiDataLen);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
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
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pbData, unsigned long ulSize)
{
	return -1;
}

/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char bData)
{
	return -1;
}

/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	return -1;
}

/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	return -1;
}

/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const unsigned char *pbData, unsigned long ulSize)
{
	return -1;
}

/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0)
{
	return -1;
}

/*-------------------------------------*/

bool IsDeviceNetx(struct usb_device *ptDevice)
{
	bool fDeviceIsNetx;


	if( ptDevice==NULL )
	{
		return false;
	}
/*
	// show devices
	printf("found 0x%04x/0x%04x\n", ptDevice->descriptor.idVendor, ptDevice->descriptor.idProduct);
	return false;
*/
	fDeviceIsNetx  = true;
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceClass==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceSubClass==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bDeviceProtocol==0x00 );
	fDeviceIsNetx &= ( ptDevice->descriptor.idVendor==0x0cc4 );
	fDeviceIsNetx &= ( ptDevice->descriptor.idProduct==0x0815 );
	fDeviceIsNetx &= ( ptDevice->descriptor.bcdDevice==0x0100 );

	return fDeviceIsNetx;
}

/*-------------------------------------*/
