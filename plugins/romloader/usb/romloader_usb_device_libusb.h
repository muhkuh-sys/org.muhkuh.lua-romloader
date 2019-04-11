/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


#ifndef __ROMLOADER_USB_DEVICE_LIBUSB_H__
#define __ROMLOADER_USB_DEVICE_LIBUSB_H__


#include "romloader_usb_device.h"

#include <errno.h>

/* NOTE: the header is in 'libusb-1.0' for linux and 'libusb-1.0' for windows. */
#include <libusb.h>

#if (!defined(LIBUSB_API_VERSION)) || (defined(LIBUSB_API_VERSION) && LIBUSB_API_VERSION<0x01000102)
#       error "This plugin needs at least libusb 1.0.16."
#endif

#if defined(WIN32)
#       include <windows.h>
#endif


typedef struct usb_device_descriptor LIBUSB_DEVICE_DESCRIPTOR_T;


class romloader_usb_provider;
class romloader_usb_reference;

class romloader_usb_device_libusb : public romloader_usb_device
{
public:
	romloader_usb_device_libusb(const char *pcPluginId);
	~romloader_usb_device_libusb(void);

	const char *libusb_strerror(int iError);

	int detect_interfaces(romloader_usb_reference ***ppptReferences, size_t *psizReferences, romloader_usb_provider *ptProvider);
	void free_references(romloader_usb_reference **pptReferences, size_t sizReferences);
	int Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr);
	void Disconnect(void);

	romloader::TRANSPORTSTATUS_T send_packet(const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned int uiTimeoutMs);
	romloader::TRANSPORTSTATUS_T receive_packet(unsigned char *aucInBuf, size_t sizInBuf, size_t *psizInBuf, unsigned int uiTimeoutMs);
#if 0
	int execute_command(const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned char *aucInBuf, size_t sizInBufMax, size_t *psizInBuf);
#endif

protected:
	NETX_USB_DEVICE_T m_tDeviceId;

private:
	const NETX_USB_DEVICE_T *identifyDevice(libusb_device *ptDevice) const;

	libusb_device *find_netx_device(libusb_device **ptDeviceList, ssize_t ssizDevList, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	int setup_netx_device(libusb_device *ptNetxDevice, const NETX_USB_DEVICE_T *ptId);

	int libusb_reset_and_close_device(void);
//	int libusb_release_and_close_device(void);


	/* General update routines. */
	int update_old_netx_device(libusb_device *ptNetxDevice, libusb_device **pptUpdatedNetxDevice);
	/* netx10 update routines. */
	int netx10_discard_until_timeout(libusb_device_handle *ptDevHandle);
	int netx10_load_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode, size_t sizNetxCode);
	int netx10_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode);
	int netx10_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice);
	/* netx500 update routines. */
	unsigned short netx500_crc16(const unsigned char *pucData, size_t sizData);
	int netx500_exchange_data(libusb_device_handle *ptDevHandle, const unsigned char *pucOutBuffer, unsigned char *pucInBuffer);
	int netx500_discard_until_timeout(libusb_device_handle *ptDevHandle);
	int netx500_load_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode, size_t sizNetxCode);
	int netx500_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode);
	int netx500_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice);
	/* netx56 update routines. */
	int netx56_execute_command(libusb_device_handle* ptDevHandle, const unsigned char *aucOutBuf, size_t sizOutBuf, unsigned char *aucInBuf, size_t *psizInBuf);
	int netx56_load_code(libusb_device_handle* ptDevHandle, const unsigned char* pucNetxCode, size_t sizNetxCode);
	int netx56_start_code(libusb_device_handle *ptDevHandle, const unsigned char *pucNetxCode);
	int netx56_upgrade_romcode(libusb_device *ptDevice, libusb_device **pptUpdatedNetxDevice);
	
/*	libusb_device *find_usb_device_by_location(unsigned char ucLocation_Bus, unsigned char ucLocation_Port); */

	static const char *m_pcPluginNamePattern;

	libusb_context *m_ptLibUsbContext;
	libusb_device_handle *m_ptDevHandle;


	typedef struct
	{
		int iError;
		const char *pcMessage;
	} LIBUSB_STRERROR_T;
	static const LIBUSB_STRERROR_T atStrError[14];
};


#endif  /* __ROMLOADER_USB_DEVICE_LIBUSB_H__ */

