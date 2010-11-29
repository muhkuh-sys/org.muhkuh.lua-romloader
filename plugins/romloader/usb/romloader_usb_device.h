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


#ifndef __ROMLOADER_USB_DEVICE_H__
#define __ROMLOADER_USB_DEVICE_H__

#include "../romloader.h"


#ifdef WIN32
#	include <Rpc.h>
#else
#	include <uuid/uuid.h>
#	include <memory.h>
#endif


typedef struct
{
	const char *pcName;
	unsigned short usVendorId;
	unsigned short usDeviceId;
	unsigned short usBcdDevice;
	ROMLOADER_CHIPTYP tChiptyp;
	ROMLOADER_ROMCODE tRomcode;
	unsigned char ucEndpoint_In;
	unsigned char ucEndpoint_Out;
} NETX_USB_DEVICE_T;


class romloader_usb_device
{
public:
	romloader_usb_device(const char *pcPluginId);
	~romloader_usb_device(void);

protected:
	char *m_pcPluginId;


	static const NETX_USB_DEVICE_T atNetxUsbDevices[4];

	typedef union
	{
		unsigned char auc[16];
		unsigned long aul[4];
	} UUID_T;
	void uuid_generate(UUID_T *ptUuid);

	void hexdump(const unsigned char *pucData, unsigned long ulSize);

private:
};


#endif	/* __ROMLOADER_USB_DEVICE_H__ */

