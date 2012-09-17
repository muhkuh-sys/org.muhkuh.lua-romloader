/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


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
        0x00, 0x02,                             // USB spec revision level (BCD)
        0x02,                                   // device class
        0x00,                                   // device subclass
        0x00,                                   // device protocol
        Usb_Ep0_PacketSize,                     // max packet size
        (VendorID&0xff), (VendorID>>8),         // vendor ID
        (ProductID&0xff), (ProductID>>8),       // product ID
        (ProductRel>>8), (ProductRel&0xff),     // product release ID (BCD)
        0x00,                                   // index of manufacturer StringDesc
        0x00,                                   // index of product StringDesc
        0x00,                                   // index of serial number StringDesc
        0x01                                    // number of configurations
};


const unsigned char Configuration_Descriptor[] =
{
        0x09,                                   // length of desc
        0x02,                                   // descriptor type
        0x3e, 0x00,                             // total length of returned data
        0x02,                                   // number of interfaces
        0x01,                                   // number of this configuration
        0x00,                                   // index of config StringDescriptor
        0xc0,                                   // attribute (self powered, no wakeup-support)
        0x00,                                   // max power (in 2mA units)

        // interface 0 descriptor
        0x09,                                   // length of desc
        0x04,                                   // descriptor type = interface descriptor
        0x00,                                   // interface number
        0x00,                                   // alternate setting
        0x01,                                   // number of (non-zero) EPs
        0x02,                                   // interface class
        0x02,                                   // interface subclass
        0x01,                                   // interface protocol
        0x00,                                   // interface StringDescriptor index

        // something
        0x05,                                   // length of desc
        0x24,                                   // descriptor type = CS_INTERFACE
        0x00,                                   // subtype = Header Functional Descriptor
        0x01, 0x01,                             // USB CDC Version 01.01
        
        // something
        0x05,                                   // length of desc
        0x24,                                   // descriptor type = CS_INTERFACE
        0x01,                                   // subtype = Call Management Descriptor
        0x00,                                   // no call management
        0x01,                                   // ignored for 'no call management'
        
        // something
        0x04,                                   // length of desc
        0x24,                                   // descriptor type = CS_INTERFACE
        0x02,                                   // subtype = Abstract Control Management Descriptor
        0x02,                                   // set features, see 'usbcdc11.pdf', page47

        // endpoint 1 descriptor
        0x07,                                   // length
        0x05,                                   // descriptor type = endpoint descriptor
        0x81,                                   // endpoint 0x01
        0x03,                                   // interrupt
        Usb_Ep1_PacketSize, 0x00,               // max packet size
        0xff,                                   // polling interval

        // interface 1 descriptor
        0x09,                                   // length of desc
        0x04,                                   // descriptor type = interface descriptor
        0x01,                                   // interface number
        0x00,                                   // alternate setting
        0x02,                                   // number of (non-zero) EPs
        0x0a,                                   // interface class for CDC data channel
        0x00,                                   // interface subclass
        0x00,                                   // interface protocol
        0x00,                                   // interface StringDescriptor index

        // endpoint 2 descriptor
        0x07,                                   // length
        0x05,                                   // descriptor type = endpoint descriptor
        0x83,                                   // endpoint 0x83
        0x02,                                   // bulk endpoint
        Usb_Ep2_PacketSize, 0x00,               // max packet size
        0x01,                                   // polling interval (ignored for bulk)

        // endpoint 3 descriptor
        0x07,                                   // length
        0x05,                                   // descriptor type = endpoint descriptor
        0x02,                                   // endpoint 0x02
        0x02,                                   // bulk endpoint
        Usb_Ep3_PacketSize, 0x00,               // max packet size
        0x01                                    // polling interval (ignored for bulk)
};


void sendDescriptor(unsigned int reqiredLength, unsigned int descriptorLength, const unsigned char *descriptor)
{
        unsigned int sendLength;


        if( reqiredLength < descriptorLength)
        {
                sendLength = reqiredLength;
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

