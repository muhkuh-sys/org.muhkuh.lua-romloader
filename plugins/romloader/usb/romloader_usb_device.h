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

#if defined(WIN32)
#	include <windows.h>
#else
#	include <pthread.h>
#endif

#include "../romloader.h"


class romloader_usb_device
{
public:
	romloader_usb_device(const char *pcPluginId);
	~romloader_usb_device(void);

	/*
	 * The card buffer interface.
	 */
	static const size_t mc_sizCardSize = 16384;
	struct sBufferCard;

	typedef struct sBufferCard
	{
		unsigned char *pucEnd;
		unsigned char *pucRead;
		unsigned char *pucWrite;
		sBufferCard *ptNext;
		unsigned char aucData[mc_sizCardSize];
	} tBufferCard;

	void initCards(void);
	void deleteCards(void);
	void writeCards(const unsigned char *pucBuffer, size_t sizBufferSize);
	size_t readCards(unsigned char *pucBuffer, size_t sizBufferSize);
	size_t getCardSize(void) const;



protected:
	void card_lock_enter(void);
	void card_lock_leave(void);

#if defined(WIN32)
	HANDLE m_hCardMutex;
#else
	pthread_mutex_t *m_ptCardMutex;
#endif

	tBufferCard *m_ptFirstCard;
	tBufferCard *m_ptLastCard;

	char *m_pcPluginId;


	typedef struct
	{
		const char *pcName;
		unsigned short usVendorId;
		unsigned short usDeviceId;
		ROMLOADER_CHIPTYP tChiptyp;
		ROMLOADER_ROMCODE tRomcode;
		unsigned char ucEndpoint_In;
		unsigned char ucEndpoint_Out;
	} NETX_USB_DEVICE_T;

	static const NETX_USB_DEVICE_T atNetxUsbDevices[2];

	void hexdump(const unsigned char *pucData, unsigned long ulSize);

private:
	size_t readCardData(unsigned char *pucBuffer, size_t sizBufferSize);


};


#endif	/* __ROMLOADER_USB_DEVICE_H__ */

