/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


#include "netx_io_areas.h"

#include "usb_io.h"
#include "usb_globals.h"


void usb_io_read_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, unsigned char *pucBuffer)
{
	const unsigned char *pucSc;
	unsigned char *pucDc, *pucDe;


	pucSc = (const unsigned char*)(HOSTADR(USB_FIFO_BASE) + (uiDwOffset<<2));
	pucDc = pucBuffer;
	pucDe = pucDc + uiByteCount;

	while( pucDc<pucDe ) {
		*pucDc++ = *pucSc++;
	}
}


void usb_io_write_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, const unsigned char *pucBuffer)
{
	/*
	  NOTE: the ahbl switch can not write bytewise to registers or all other
	  bytes in the dword are set to the same value
	*/
	const unsigned char *pucSc;
	unsigned long *pulDc, *pulDe;
	unsigned int uiByteCnt;
	unsigned long ulValue;


	/* round up the bytecount */
	uiByteCount += 3;

	pucSc = pucBuffer;
	pulDc = (unsigned long*)(HOSTADR(USB_FIFO_BASE) + (uiDwOffset<<2));
	pulDe = pulDc + (uiByteCount>>2);

	uiByteCnt = 0;
	ulValue = 0;
	while( pulDc<pulDe ) {
		ulValue >>= 8U;
		ulValue  |= ((unsigned long)(*pucSc++)) << 24U;
		uiByteCnt++;
		if( (uiByteCnt&3)==0 ) {
			*pulDc++ = ulValue;
		}
	}
}

void usb_io_sendDataPacket(unsigned int uiPipeNr, unsigned int uiPacketSize)
{
	HOSTDEF(ptUsbCoreArea);


	ptUsbCoreArea->ulPIPE_SEL = uiPipeNr;
	ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
	ptUsbCoreArea->ulPIPE_DATA_PTR = uiPipeNr<<5;
	ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|uiPacketSize;
}
