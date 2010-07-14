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
	void flushCards(void);
	void dump_all_cards(void);
	bool expect_string(const char *pcString);
	bool parse_hex_digit(size_t sizDigits, unsigned long *pulResult);
	int parse_uue(size_t sizLength, unsigned char *pucData, unsigned long ulStart, unsigned long ulEnd);
	int uue_generate(const unsigned char *pucData, size_t sizData, char **ppcUueData, size_t *psizUueData);

	virtual size_t usb_receive(unsigned char *pucBuffer, size_t sizBuffer, unsigned int uiTimeoutMs) = 0;
	virtual int usb_receive_line(char *pcBuffer, size_t sizBuffer, unsigned int uiTimeoutMs, size_t *psizReceived) = 0;
	virtual int usb_send(const char *pcBuffer, size_t sizBuffer) = 0;

protected:
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

