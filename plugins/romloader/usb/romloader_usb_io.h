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


#include <wx/wx.h>
#include <usb.h>

#include <wxlua/include/wxlua.h>
#include <wxluasocket/include/wxldserv.h>
extern "C"
{
	#include <lualib.h>
}


#ifndef __BOOTLOADER_USB_IO_H__
#define __BOOTLOADER_USB_IO_H__


//-------------------------------------


typedef usb_dev_handle *netx_device_handle;


typedef enum
{
	netxUsbState_Ok = 0,
	netxUsbState_OutOfMemory,
	netxUsbState_DeviceNotFound,
	netxUsbState_DeviceBusy,
	netxUsbState_ReadError,
	netxUsbState_WriteError,
	netxUsbState_BlockSizeError,
	netxUsbState_CommandTooLong,
	netxUsbState_Timeout,
	netxUsbState_Cancel,
	netxUsbState_Error
} tNetxUsbState;


//-------------------------------------

bool romloader_usb_isDeviceNetx(struct usb_device *ptDevice);
tNetxUsbState romloader_usb_openByDevice(netx_device_handle *phNetxUsbMon, struct usb_device *ptNetxDev);
tNetxUsbState romloader_usb_executeCommand(void *pvHandle, wxString strCommand, unsigned char **ppucData, unsigned int *puiDataLen);
tNetxUsbState romloader_usb_load(void *pvHandle, const unsigned char *pucData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
tNetxUsbState romloader_usb_close(void *pvHandle);

wxString romloader_usb_getErrorString(tNetxUsbState tResult);

tNetxUsbState romloader_usb_sendCommand(netx_device_handle tHandle, wxString strCommand);
tNetxUsbState romloader_usb_getNetxData(netx_device_handle tHandle, unsigned char **ppcData, unsigned int *pulDataLen, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
tNetxUsbState romloader_usb_resetDevice(netx_device_handle tHandle);

//-------------------------------------

#endif	// __BOOTLOADER_USB_IO_H__

