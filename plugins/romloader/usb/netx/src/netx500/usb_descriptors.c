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


#include <stddef.h>

#include "options.h"

#include "usb_io.h"
#include "usb_globals.h"
#include "usb_descriptors.h"

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))

//---------------------------------------------------------------------------


static const unsigned char Device_Descriptor[] =
{
	0x12,                                   // length of desc
	0x01,                                   // descriptor type
	0x00, 0x01,                             // USB spec revision level (BCD)
	0xff,                                   // device class
	0x00,                                   // device subclass
	0xff,                                   // device protocol
	Usb_Ep0_PacketSize,                     // max packet size
	(VendorID&0xff), (VendorID>>8),         // vendor ID
	(ProductID&0xff), (ProductID>>8),       // product ID
	(ProductRel&0xff), (ProductRel>>8),     // product release ID (BCD)
	0x01,                                   // index of manufacturer StringDesc
	0x02,                                   // index of product StringDesc
	0x03,                                   // index of serial number StringDesc
	0x01                                    // number of configurations
};


static const unsigned char Configuration_Descriptor[] =
{
	0x09,                                   // length of desc
	0x02,                                   // descriptor type
	0x20, 0x00,                             // total length of returned data
	0x01,                                   // number of interfaces
	0x01,                                   // number of this configuration
	0x02,                                   // index of config StringDescriptor
	0xc0,                                   // attribute (self powered, no wakeup-support)
	0x00,                                   // max power (in 2mA units)

	// interface 0 descriptor
	0x09,                                   // length of desc
	0x04,                                   // descriptor type = interface descriptor
	0x00,                                   // interface number
	0x00,                                   // alternate setting
	0x02,                                   // number of (non-zero) EPs
	0xff,                                   // interface class
	0x01,                                   // interface subclass
	0xff,                                   // interface protocol
	0x02,                                   // interface StringDescriptor index

	// endpoint 1 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x81,                                   // endpoint 0x81
	0x02,                                   // bulk endpoint
	Usb_Ep1_PacketSize, 0x00,               // max packet size
	0x01,                                   // polling interval

	// endpoint 2 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x01,                                   // endpoint 0x01
	0x02,                                   // bulk endpoint
	Usb_Ep2_PacketSize, 0x00,               // max packet size
	0x01                                    // polling interval (ignored for bulk)
};


/* string descriptor */
static const unsigned char aucTopStringDescriptor[] =
{
	0x04,           /* length */
	0x03,           /* descriptor type = string descriptor */
	0x09, 0x04      /* language #0 : English US */
};


/* stringDescriptor for the Manufacturer */
static const unsigned char aucManufacturer_eng[] =
{
	0x1c,
	0x03,

	'H', 0, 'i', 0, 'l', 0, 's', 0, 'c', 0, 'h', 0, 'e', 0, 'r', 0,
	' ', 0, 'G', 0, 'm', 0, 'b', 0, 'H', 0
};


/* stringDescriptor for the product name */
static const unsigned char aucProduct_eng[] =
{
	0x1c,
	0x03,

	'H', 0, 'B', 0, 'o', 0, 'o', 0, 't', 0, ' ', 0, 'm', 0, 'o', 0, 
	'n', 0, 'i', 0, 't', 0, 'o', 0, 'r', 0
};


/* stringDescriptor for the Serial Number */
static const unsigned char aucSerialNumber_eng[] =
{
	0x22,
	0x03,

	'0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0,
	'0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0
};


static const unsigned char * const apucEngStrings[] =
{
	aucManufacturer_eng,
	aucProduct_eng,
	aucSerialNumber_eng
};

#if 0
static void to_unicode_simple(const unsigned char *pucAsciiString, unsigned char *pucUnicodeBuffer, size_t sizUnicodeBuffer)
{
	const unsigned char *pucSrcCnt;
	unsigned char *pucDstCnt;
	unsigned char *pucDstEnd;


	pucSrcCnt = pucAsciiString;
	/* Get the string start in the buffer. */
	pucDstCnt = pucUnicodeBuffer;
	pucDstEnd = pucUnicodeBuffer + sizUnicodeBuffer;
	do
	{
		*(pucDstCnt++) = *(pucSrcCnt++);
		*(pucDstCnt++) = '\0';
	} while( pucDstCnt<pucDstEnd );
}
#endif

void usb_descriptors_init(void)
{
/*
	aucProduct_eng[0] = sizeof(aucProduct_eng);
	aucProduct_eng[1] = 3;
	to_unicode_simple(g_t_options.t_usb_settings.uCfg.auc+20+8, aucProduct_eng+2, 32);


	aucSerialNumber_eng[0] = sizeof(aucSerialNumber_eng);
	aucSerialNumber_eng[1] = 3;
	to_unicode_simple(g_t_options.t_usb_settings.uCfg.auc+38+8, aucSerialNumber_eng+2, 32);
*/
}


void sendDescriptor(unsigned int uiMaximumLength, unsigned int uiDescriptorLength, const unsigned char *aucDescriptor)
{
	unsigned int uiSendLength;


	if( uiMaximumLength<uiDescriptorLength)
	{
		uiSendLength = uiMaximumLength;
	}
	else
	{
		uiSendLength = uiDescriptorLength;
	}

	usb_io_write_fifo(0x00, uiSendLength, aucDescriptor);
	usb_io_sendDataPacket(0, uiSendLength);
}


static packet_handler_stall_req_t sendStringDescriptor(unsigned int languageId, unsigned int uiStringIdx, unsigned int uiMaximumLength)                                                                                                                                            
{                                                                            
	const unsigned char *pucStringDescriptor;
	packet_handler_stall_req_t tResult;


	if( uiStringIdx==0 )
	{
		/* config */
		sendDescriptor(uiMaximumLength, sizeof(aucTopStringDescriptor), aucTopStringDescriptor);
		tResult = PKT_HANDLER_Send_Nothing;
	}
	else
	{
		--uiStringIdx;

		if( languageId==0x0409 && uiStringIdx<ARRAYSIZE(apucEngStrings) )
		{
			pucStringDescriptor = apucEngStrings[uiStringIdx];

			/* send english strings */
			sendDescriptor(uiMaximumLength, pucStringDescriptor[0], pucStringDescriptor);
			tResult = PKT_HANDLER_Send_Nothing;
		}
		else
		{
			/* unsupported language => STALL */
			tResult = PKT_HANDLER_Send_Stall;
		}
	}

	return tResult;
}


packet_handler_stall_req_t getDescriptor(setupPacket_t *pPacket)
{
	packet_handler_stall_req_t tResult;
	USB_DescriptorType_t descriptorType;
	unsigned int descriptorIndex;
	unsigned int uiLength;


	descriptorIndex = (pPacket->tHeader.wValue&0x0f);
	descriptorType = (USB_DescriptorType_t)(pPacket->tHeader.wValue>>8);
	uiLength = pPacket->tHeader.wLength;

	// GetDescriptor Request
	switch( descriptorType )
	{
	case USB_DescriptorType_Device:
		sendDescriptor(uiLength, sizeof(Device_Descriptor), Device_Descriptor);
		tResult = PKT_HANDLER_Send_Nothing;
		break;

	case USB_DescriptorType_Configuration:
		sendDescriptor(uiLength, sizeof(Configuration_Descriptor), Configuration_Descriptor);
		tResult = PKT_HANDLER_Send_Nothing;
		break;

	case USB_DescriptorType_String:
		/* language index is wIndex, string index is the descriptor index */
		tResult = sendStringDescriptor(pPacket->tHeader.wIndex, descriptorIndex, uiLength);
		break;

	default:
		// Other requests are not supported
		tResult = PKT_HANDLER_Send_Stall;
		break;
	}

	return tResult;
}

