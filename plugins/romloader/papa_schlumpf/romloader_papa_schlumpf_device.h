/***************************************************************************
 *   Copyright (C) 2016 by Christoph Thelen                                *
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

#ifndef __ROMLOADER_PAPA_SCHLUMPF_DEVICE_LIBUSB_H__
#define __ROMLOADER_PAPA_SCHLUMPF_DEVICE_LIBUSB_H__

#include <errno.h>

/* NOTE: the header is in 'libusb-1.0' for linux and 'libusb-1.0' for windows. */
#include <libusb.h>

#if (!defined(LIBUSB_API_VERSION)) || (defined(LIBUSB_API_VERSION) && LIBUSB_API_VERSION<0x01000102)
#       error "This plugin needs at least libusb 1.0.16."
#endif


class romloader_papa_schlumpf_device
{
public:
	romloader_papa_schlumpf_device(const char *pcPluginId);
	~romloader_papa_schlumpf_device(void);

	typedef enum PAPA_SCHLUMPF_ERROR_ENUM
	{
		PAPA_SCHLUMPF_ERROR_Ok = 0,
		PAPA_SCHLUMPF_ERROR_LibUsb_Open = 1,
		PAPA_SCHLUMPF_ERROR_NoHardwareFound = 2,
		PAPA_SCHLUMPF_ERROR_FailedToListUSBDevices = 3,
		PAPA_SCHLUMPF_ERROR_FoundMoreThanOneDevice = 4,
		PAPA_SCHLUMPF_ERROR_FailedToOpenDevice = 5,
		PAPA_SCHLUMPF_ERROR_FailedToClaimInterface = 6,
		PAPA_SCHLUMPF_ERROR_TransferError = 7,
		PAPA_SCHLUMPF_ERROR_MemoryError = 8,
		PAPA_SCHLUMPF_ERROR_netxError = 9
	} PAPA_SCHLUMPF_ERROR_T;

	/*Indicates status for the raw usb communication*/
	typedef enum USB_STATUS_ENUM
	{
		USB_STATUS_OK = 0,
		USB_STATUS_NOT_ALL_BYTES_TRANSFERED = 1,
		USB_STATUS_ERROR = 2,
		USB_STATUS_RECEIVED_EMPTY_PACKET = 3
	} USB_STATUS_T;

	static const int iPathMax = 32;
	typedef struct INTERFACE_REFERENCE_STRUCT
	{
		unsigned int uiBusNr;
		unsigned int uiDevAdr;
		/* bus number as a digit plus path elements as single digits */
		/* This is the Location ID format used by USBView, but is this sufficient? */
		char acPathString[iPathMax * 2 + 2];
	} INTERFACE_REFERENCE_T;
	PAPA_SCHLUMPF_ERROR_T detect_interfaces(INTERFACE_REFERENCE_T **pptInterfaces, size_t *psizInterfaces);


	romloader_papa_schlumpf_device::USB_STATUS_T sendAndReceivePackets(uint8_t* aucOutBuf, uint32_t sizOutBuf, uint8_t* aucInBuf, uint32_t* sizInBuf);
	romloader_papa_schlumpf_device::USB_STATUS_T sendPacket(uint8_t* aucOutBuf, uint32_t sizOutBuf);
	romloader_papa_schlumpf_device::USB_STATUS_T receivePacket(uint8_t* aucInBuf, uint32_t* pSizInBuf);


private:
	char *m_pcPluginId;

	libusb_device_handle *m_ptLibUsbDeviceHandle;
	libusb_context       *m_ptLibUsbContext;
};


#endif  /* __ROMLOADER_PAPA_SCHLUMPF_DEVICE_LIBUSB_H__ */
