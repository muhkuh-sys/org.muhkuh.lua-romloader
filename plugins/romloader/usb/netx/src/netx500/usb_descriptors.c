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


#include "usb_io.h"
#include "usb_requests_common.h"
#include "usb_globals.h"
#include "usb_requests_common.h"
#include "usb_descriptors.h"


//---------------------------------------------------------------------------


const unsigned char Device_Descriptor[] =
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
	(ProductRel>>8), (ProductRel&0xff),     // product release ID (BCD)
	0x01,                                   // index of manufacturer StringDesc
	0x02,                                   // index of product StringDesc
	0x03,                                   // index of serial number StringDesc
	0x01                                    // number of configurations
};


const unsigned char Configuration_Descriptor[] =
{
	0x09,                                   // length of desc
	0x02,                                   // descriptor type
	0x35, 0x00,                             // total length of returned data
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
	0x05,                                   // number of (non-zero) EPs
	0xff,                                   // interface class
	0x01,                                   // interface subclass
	0xff,                                   // interface protocol
	0x02,                                   // interface StringDescriptor index

	// endpoint 1 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x81,                                   // endpoint 0x01
	0x02,                                   // bulk endpoint
	Usb_Ep1_PacketSize, 0x00,               // max packet size
	0x01,                                   // polling interval

	// endpoint 2 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x02,                                   // endpoint 0x02
	0x02,                                   // bulk endpoint
	Usb_Ep2_PacketSize, 0x00,               // max packet size
	0x01,                                   // polling interval (ignored for bulk)

	// endpoint 3 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x83,                                   // endpoint 0x83
	0x02,                                   // bulk endpoint
	Usb_Ep3_PacketSize, 0x00,               // max packet size
	0x01,                                   // polling interval (ignored for bulk)

	// endpoint 4 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x04,                                   // endpoint 0x02
	0x02,                                   // bulk endpoint
	Usb_Ep4_PacketSize, 0x00,               // max packet size
	0x01,                                   // polling interval (ignored for bulk)

	// endpoint 5 descriptor
	0x07,                                   // length
	0x05,                                   // descriptor type = endpoint descriptor
	0x85,                                   // endpoint 0x85
	0x03,                                   // interrupt endpoint
	Usb_Ep5_PacketSize, 0x00,               // max packet size
	0x01                                    // polling interval (ignored for bulk)
};


void sendDescriptor(unsigned int requiredLength, unsigned int descriptorLength, const unsigned char *descriptor)
{
	unsigned int sendLength;


	if( requiredLength < descriptorLength)
	{
		sendLength = requiredLength;
	}
	else
	{
		sendLength = descriptorLength;
	}

	usb_io_write_fifo(0x00, sendLength, descriptor);
	usb_io_sendDataPacket(0, sendLength);
}


packet_handler_stall_req_t getDescriptor(setupPacket_t *pPacket)
{
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
                return PKT_HANDLER_Send_Nothing;

        case USB_DescriptorType_Configuration:
                sendDescriptor(uiLength, sizeof(Configuration_Descriptor), Configuration_Descriptor);
                return PKT_HANDLER_Send_Nothing;
        default:
                // Other requests are not supported
                return PKT_HANDLER_Send_Stall;
        }
}

