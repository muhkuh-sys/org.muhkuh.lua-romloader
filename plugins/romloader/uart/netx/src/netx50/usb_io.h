/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#ifndef __usb_io_h__
#define __usb_io_h__

void usb_io_read_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, unsigned char *pucBuffer);
void usb_io_write_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, const unsigned char *pucBuffer);

void usb_io_sendStall(void);
void usb_io_sendDataPacket(unsigned int uiPipeNr, unsigned int uiPacketSize);

#endif  // __usb_io_h__
