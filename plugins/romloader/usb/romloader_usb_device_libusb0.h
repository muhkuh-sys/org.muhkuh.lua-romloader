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


#ifndef __ROMLOADER_USB_DEVICE_LIBUSB0_H__
#define __ROMLOADER_USB_DEVICE_LIBUSB0_H__


#include <libusb-1.0/libusb.h>

#include "romloader_usb_device.h"


#if defined(WIN32)
#	include <windows.h>
	typedef int ssize_t;
#else
#	include <pthread.h>
#endif


typedef struct usb_device_descriptor LIBUSB_DEVICE_DESCRIPTOR_T;


class romloader_usb_provider;
class romloader_usb_reference;

class romloader_usb_device_libusb0 : public romloader_usb_device
{
public:
	romloader_usb_device_libusb0(const char *pcPluginId);
	~romloader_usb_device_libusb0(void);

	const char *libusb_strerror(int iError);

	int detect_interfaces(romloader_usb_reference ***ppptReferences, size_t *psizReferences, romloader_usb_provider *ptProvider);
	int Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr);
	void Disconnect(void);

	int read_data08(unsigned long ulNetxAddress, unsigned char *pucValue);
	int read_data16(unsigned long ulNetxAddress, unsigned short *pusValue);
	int read_data32(unsigned long ulNetxAddress, unsigned long *pulValue);
	int read_image(unsigned long ulNetxAddress, size_t sizData, unsigned char *pucData);

	int write_data32(unsigned long ulNetxAddress, unsigned long ulData);
	int write_image(unsigned long ulNetxAddress, const unsigned char *pucData, size_t sizData);

	int call(unsigned long ulNetxAddress, unsigned long ulR0);

	size_t usb_receive(unsigned char *pucBuffer, size_t sizBuffer, unsigned int uiTimeoutMs);
	int usb_receive_line(char *pcBuffer, size_t sizBuffer, unsigned int uiTimeoutMs, size_t *psizReceived);
	int usb_send(const char *pcBuffer, size_t sizBuffer);

#if defined(WIN32)
	static DWORD WINAPI rxThread(LPVOID lpParam);
	DWORD localRxThread(void);
#else
	static void *rxThread(void *pvParameter);
	void *localRxThread(void);
#endif

protected:
	ROMLOADER_CHIPTYP m_tChiptyp;
	ROMLOADER_ROMCODE m_tRomcode;

	unsigned char m_ucEndpoint_In;
	unsigned char m_ucEndpoint_Out;

private:
	bool fIsDeviceNetx(libusb_device *ptDevice);

	libusb_device *find_netx_device(libusb_device **ptDeviceList, ssize_t ssizDevList, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	int setup_netx_device(libusb_device *ptNetxDevice);

	int libusb_reset_and_close_device(void);
//	int libusb_release_and_close_device(void);

	int start_rx_thread(void);
	int stop_rx_thread(void);


	const int m_iConfiguration;
	const int m_iInterface;



	static const char *m_pcPluginNamePattern;

	libusb_context *m_ptLibUsbContext;
	libusb_device_handle *m_ptDevHandle;

#if defined(WIN32)
	volatile bool m_fRxThread_RequestTermination;
	HANDLE m_hRxThread;
	HANDLE m_hRxDataAvail;
#else
	int get_end_time(unsigned int uiTimeoutMs, struct timespec *ptEndTime);

	pthread_t *m_ptRxThread;
	pthread_cond_t *m_ptRxDataAvail_Condition;
	pthread_mutex_t *m_ptRxDataAvail_Mutex;
#endif

	static const char *m_pcLibUsb_BusPattern;
	static const char *m_pcLibUsb_DevicePattern;


	typedef struct
	{
		int iError;
		const char *pcMessage;
	} LIBUSB_STRERROR_T;

	static const LIBUSB_STRERROR_T atStrError[14];
};


#endif	/* __ROMLOADER_USB_DEVICE_LIBUSB0_H__ */

