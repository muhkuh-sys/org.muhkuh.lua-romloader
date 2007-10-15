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


#include <errno.h>
#include "romloader_usb_io.h"


#ifdef __WXMSW__
#define ETIMEDOUT WSAETIMEDOUT
#endif


/*-------------------------------------*/


static unsigned int crc16(unsigned int uCrc, unsigned int uData)
{
	uCrc  = (uCrc >> 8) | ((uCrc & 0xff) << 8);
	uCrc ^= uData;
	uCrc ^= (uCrc & 0xff) >> 4;
	uCrc ^= (uCrc & 0x0f) << 12;
	uCrc ^= ((uCrc & 0xff) << 4) << 1;

	return uCrc;
}


static tNetxUsbState readBlock(netx_device_handle hNetxUsbMon, unsigned char *pcReceiveBuffer, unsigned int uiSize, int iTimeoutMs)
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


static tNetxUsbState writeBlock(netx_device_handle hNetxUsbMon, const unsigned char *pcSendBuffer, unsigned int uiSize, int iTimeoutMs)
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



static tNetxUsbState exchange(netx_device_handle hNetxUsbMon, const unsigned char *pcSendBuffer, unsigned char *pcReceiveBuffer)
{
	tNetxUsbState tResult;


	tResult = writeBlock(hNetxUsbMon, pcSendBuffer, 64, 200);
	if( tResult==netxUsbState_Ok )
	{
		tResult = readBlock(hNetxUsbMon, pcReceiveBuffer, 64, 200);
	}
        return tResult;
}


static bool callback(lua_State *L, int iLuaCallbackTag, unsigned long ulProgressData, void *pvCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;
	int iCurrentStack;
	int iResult;
	wxString strMsg;


	// check lua state and callback tag
	if( L!=NULL && iLuaCallbackTag!=0 )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(L);
		// push the function tag on the stack
		lua_rawgeti(L, LUA_REGISTRYINDEX, iLuaCallbackTag);
		// push the arguments on the stack
		lua_pushnumber(L, ulProgressData);
		lua_pushnumber(L, (long)pvCallbackUserData);
		// call the function
		iResult = lua_pcall(L, 2, 1, 0);
		strMsg.Printf(wxT("LuaPCall returned %d"), iResult);
		wxLogMessage(strMsg);
		if( iResult!=0 )
		{
			strMsg = wxlua_getstringtype(L, 0);
			wxLogMessage(strMsg);
			fStillRunning = false;
		}
		else
		{
			// get the function's return value
			fStillRunning = wxlua_getbooleantype(L, -1);
			strMsg.Printf(wxT("got 0x%08X from callback function"), fStillRunning);
			wxLogMessage(strMsg);
		}
	
		// TODO: is settop really necessary?
		iCurrentStack = lua_gettop(L);
		strMsg.Printf(wxT("old stack: %d, new stack: %d"), iOldTopOfStack, iCurrentStack);
		lua_settop(L, iOldTopOfStack);
	}
	else
	{
		// no callback function -> keep running
		fStillRunning = true;
	}

	return fStillRunning;
}


//-------------------------------------


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


tNetxUsbState romloader_usb_openByDevice(netx_device_handle *phNetxUsbMon, struct usb_device *ptNetxDev)
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


tNetxUsbState romloader_usb_sendCommand(netx_device_handle tHandle, wxString strCommand)
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
	tResult = exchange(tHandle, abSend, abRec);
	if( tResult==netxUsbState_Ok )
	{
		// terminate command
		abSend[0] = 0x00;
		tResult = exchange(tHandle, abSend, abRec);
	}

	return tResult;
}


tNetxUsbState romloader_usb_getNetxData(netx_device_handle hNetxUsbMon, unsigned char **ppcData, unsigned int *pulDataLen, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
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
		tResult = exchange(hNetxUsbMon, buf_send, buf_rec);
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


tNetxUsbState romloader_usb_executeCommand(void *pvHandle, wxString strCommand, unsigned char **ppucData, unsigned int *puiDataLen)
{
	netx_device_handle tHandle;
	tNetxUsbState tResult;


	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// send the command
	tResult = romloader_usb_sendCommand(tHandle, strCommand);
	if( tResult==netxUsbState_Ok )
	{
		// get the response
		tResult = romloader_usb_getNetxData(tHandle, ppucData, puiDataLen, NULL, 0, NULL);
	}

	return tResult;
}


tNetxUsbState romloader_usb_load(void *pvHandle, const unsigned char *pucData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	netx_device_handle tHandle;
	const unsigned char *pucDataCnt, *pucDataEnd;
	tNetxUsbState tResult;
	unsigned int uiCrc;
	wxString strCommand;
	unsigned char aucBufSend[64];
	unsigned char aucBufRec[64];
	size_t sizChunkSize;
	bool fIsRunning;
	unsigned long ulBytesProcessed;


	// get handle
	tHandle = (netx_device_handle)pvHandle;

	// generate crc checksum
	pucDataCnt = pucData;
	pucDataEnd = pucData + sizDataLen;
	uiCrc = 0xffff;
	// loop over all bytes
	while( pucDataCnt<pucDataEnd )
	{
		uiCrc = crc16(uiCrc, *(pucDataCnt++));
	}

	// generate load command

	strCommand.Printf(wxT("LOAD %08lX %08X %04X"), ulLoadAdr, sizDataLen, uiCrc);
	// send the command
	tResult = romloader_usb_sendCommand(tHandle, strCommand);
	if( tResult!=netxUsbState_Ok )
	{
		wxLogError(wxT("failed to send command!"));
		return tResult;
	}

	// now send the data part
	pucDataCnt = pucData;
	ulBytesProcessed = 0;
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

		wxTheApp->Yield();
		fIsRunning = callback(L, iLuaCallbackTag, ulBytesProcessed, pvCallbackUserData);
		if( fIsRunning!=true )
		{
			tResult = netxUsbState_Cancel;
			break;
		}

		tResult = exchange(tHandle, aucBufSend, aucBufRec);
		if( tResult!=netxUsbState_Ok )
		{
			break;
		}
		pucDataCnt += sizChunkSize;
		ulBytesProcessed += sizChunkSize;
	}

	if( pucDataCnt==pucDataEnd )
	{
		tResult = netxUsbState_Ok;
	}

	return tResult;
}


tNetxUsbState romloader_usb_close(void *pvHandle)
{
	netx_device_handle hNetxUsbMon;
	int iRet;


	// get handle
	hNetxUsbMon = (netx_device_handle)pvHandle;

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


tNetxUsbState romloader_usb_resetDevice(netx_device_handle tHandle)
{
	int iRet;


	iRet = usb_reset(tHandle);
	if( iRet==0 )
	{
		usb_close(tHandle);
		return netxUsbState_Ok;
	}
	else if( iRet==-ENODEV )
	{
		// the old device is already gone -> that's good, ignore the error
		usb_close(tHandle);
		return netxUsbState_Ok;
	}
	else
	{
		return netxUsbState_Error;
	}
}


wxString romloader_usb_getErrorString(tNetxUsbState tResult)
{
	wxString strMessage;


	switch(tResult)
	{
	case netxUsbState_Ok:
		strMessage = wxT("ok");
		break;

	case netxUsbState_OutOfMemory:
		strMessage = wxT("out of memory");
		break;

	case netxUsbState_DeviceNotFound:
		strMessage = wxT("device not found");
		break;

	case netxUsbState_DeviceBusy:
		strMessage = wxT("device is busy");
		break;

	case netxUsbState_ReadError:
		strMessage = wxT("read error");
		break;

	case netxUsbState_WriteError:
		strMessage = wxT("write error");
		break;

	case netxUsbState_BlockSizeError:
		strMessage = wxT("block size error");
		break;

	case netxUsbState_CommandTooLong:
		strMessage = wxT("command too long");
		break;

	case netxUsbState_Timeout:
		strMessage = wxT("timeout");
		break;

	case netxUsbState_Cancel:
		strMessage = wxT("canceled");
		break;

	case netxUsbState_Error:
		strMessage = wxT("error");
		break;

	default:
		strMessage.Printf(wxT("unknown errorcode: %d"), tResult);
		break;
	}

	return strMessage;
}

