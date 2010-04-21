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


#include <usb.h>

#include "romloader_usb_device.h"


typedef usb_dev_handle libusb_device_handle;
typedef void* libusb_context;
typedef struct usb_device libusb_device;
typedef struct usb_device_descriptor LIBUSB_DEVICE_DESCRIPTOR_T;

typedef enum
{
	LIBUSB_SUCCESS = 0,
	LIBUSB_ERROR_IO = -1,
	LIBUSB_ERROR_INVALID_PARAM = -2,
	LIBUSB_ERROR_ACCESS = -3,
	LIBUSB_ERROR_NO_DEVICE = -4,
	LIBUSB_ERROR_NOT_FOUND = -5,
	LIBUSB_ERROR_BUSY = -6,
	LIBUSB_ERROR_TIMEOUT = -7,
	LIBUSB_ERROR_OVERFLOW = -8,
	LIBUSB_ERROR_PIPE = -9,
	LIBUSB_ERROR_INTERRUPTED = -10,
	LIBUSB_ERROR_NO_MEM = -11,
	LIBUSB_ERROR_NOT_SUPPORTED = -12,
	LIBUSB_ERROR_OTHER = -99,
} LIBUSB_ERROR_T;


class romloader_usb_provider;
class romloader_usb_reference;

class romloader_usb_device_libusb0 : public romloader_usb_device
{
public:
	romloader_usb_device_libusb0(const char *pcPluginId);
	~romloader_usb_device_libusb0(void);

	const char *libusb_strerror(int iError);

	int usb_bulk_pc_to_netx(libusb_device_handle *ptDevHandle, unsigned char ucEndPointOut, const unsigned char *pucDataOut, int iLength, int *piProcessed, unsigned int uiTimeoutMs);
	int usb_bulk_netx_to_pc(libusb_device_handle *ptDevHandle, unsigned char ucEndPointIn, unsigned char *pucDataIn, int iLength, int *piProcessed, unsigned int uiTimeoutMs);


	int detect_interfaces(romloader_usb_reference ***ppptReferences, size_t *psizReferences, romloader_usb_provider *ptProvider);
	int Connect(unsigned int uiBusNr, unsigned int uiDeviceAdr);
	void Disconnect(void);

	size_t usb_receive(unsigned char *pucBuffer, size_t sizBuffer, unsigned int uiTimeoutMs);


	static void *rxThread(void *pvParameter);
	void *localRxThread(void);

protected:
	pthread_t m_tRxThread;

	ROMLOADER_CHIPTYP m_tChiptyp;
	ROMLOADER_ROMCODE m_tRomcode;

	unsigned char m_ucEndpoint_In;
	unsigned char m_ucEndpoint_Out;

private:
	bool fIsDeviceNetx(libusb_device *ptDevice);

	int libusb_get_device_descriptor(libusb_device *dev, LIBUSB_DEVICE_DESCRIPTOR_T *desc);
	ssize_t libusb_get_device_list(libusb_device ***list);
	void libusb_free_device_list(libusb_device **list, int unref_devices);
	libusb_device *find_netx_device(libusb_device **ptDeviceList, ssize_t ssizDevList, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	int setup_netx_device(libusb_device *ptNetxDevice);

	unsigned char libusb_get_bus_number(libusb_device *dev);
	unsigned char libusb_get_device_address(libusb_device *dev);

	int libusb_init(libusb_context **pptContext);
	void libusb_exit(libusb_context *ptContext);

	int libusb_open(libusb_device *ptDevice);
	void libusb_close(void);
	int libusb_reset_and_close_device(void);
	int libusb_release_and_close_device(void);

	int libusb_set_configuration(int iConfiguration);

	int libusb_claim_interface(void);
	int libusb_release_interface(void);

	int start_rx_thread(void);
	int stop_rx_thread(void);

	int get_end_time(unsigned int uiTimeoutMs, struct timespec *ptEndTime);


	static const char *m_pcPluginNamePattern;

	libusb_context *m_ptLibUsbContext;
	libusb_device_handle *m_ptDevHandle;

	pthread_cond_t *m_ptRxDataAvail_Condition;
	pthread_mutex_t *m_ptRxDataAvail_Mutex;

	static const char *m_pcLibUsb_BusPattern;
	static const char *m_pcLibUsb_DevicePattern;


	typedef struct
	{
		LIBUSB_ERROR_T tError;
		const char *pcMessage;
	} LIBUSB_STRERROR_T;

	static const LIBUSB_STRERROR_T atStrError[14];
};


#endif	/* __ROMLOADER_USB_DEVICE_LIBUSB0_H__ */

