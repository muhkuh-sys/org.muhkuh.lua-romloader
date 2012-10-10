/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include <stddef.h>

#include "usb_requests_common.h"
#include "usb_requests_cdc.h"
#include "usb_descriptors.h"
#include "usb_globals.h"
#include "usb_io.h"

//-------------------------------------

#define USB_CDC_BUF_SEND_LEN 256

unsigned char usb_cdc_buf_send[USB_CDC_BUF_SEND_LEN];

unsigned int usb_cdc_buf_send_rpos;             // read position
unsigned int usb_cdc_buf_send_wpos;             // write position
unsigned int usb_cdc_buf_send_fill;             // fill level

//-------------------------------------

// Buffer handling, User level functions

// 'put' waits for buffer space and puts the char into the buffer
void usb_cdc_buf_send_put(unsigned int uiValue)
{
	// is at least one char free in the buffer?
	if( usb_cdc_buf_send_fill==USB_CDC_BUF_SEND_LEN )
	{
		while( usb_cdc_buf_send_fill==USB_CDC_BUF_SEND_LEN )
		{
			usb_pingpong();
		}
	}
	else if( usb_cdc_buf_send_fill>=Usb_Ep3_PacketSize )
	{
		usb_pingpong();
	}

	// put one char in the buffer
	usb_cdc_buf_send[usb_cdc_buf_send_wpos] = (unsigned char)uiValue;
	// advance wait position
	++usb_cdc_buf_send_wpos;
	// wrap position?
	if( usb_cdc_buf_send_wpos>=USB_CDC_BUF_SEND_LEN )
	{
		usb_cdc_buf_send_wpos = 0;
	}
	// increment the buffer fill value
	++usb_cdc_buf_send_fill;

	// enough data to reactivate the endpoint?
	if( tSendEpState==USB_SendEndpoint_Idle && usb_cdc_buf_send_fill>=Usb_Ep3_PacketSize )
	{
		usb_sendPendingPacket();
	}
}


void usb_cdc_buf_send_flush(void)
{
        // send all remaining chars in the send buffer
        if( usb_cdc_buf_send_fill==0 )
        {
                return;
        }

        // endpoint idle -> reactivate it
        if( tSendEpState==USB_SendEndpoint_Idle )
        {
                usb_sendPendingPacket();
        }
        // wait for all bytes to be sent
        do {
                usb_pingpong();
        } while( usb_cdc_buf_send_fill!=0 );
}


//-------------------------------------

// Buffer handling, Usb level functions



unsigned int usb_cdc_buf_send_get(unsigned char *pbBuffer, unsigned int uiMaxPacketSize)
{
        unsigned int uiBufferLen;
        unsigned char *pbD, *pbE;


        // get the number of waiting chars in the buffer, but max one packet
        uiBufferLen = (usb_cdc_buf_send_fill > uiMaxPacketSize) ? uiMaxPacketSize : usb_cdc_buf_send_fill;

        if( uiBufferLen!=0 )
        {
                // copy the buffer
                pbD = pbBuffer;
                pbE = pbD + uiBufferLen;
                do
                {
                        *(pbD++) = usb_cdc_buf_send[usb_cdc_buf_send_rpos++];
                        if( usb_cdc_buf_send_rpos>=USB_CDC_BUF_SEND_LEN )
                        {
                                usb_cdc_buf_send_rpos = 0;
                        }
                } while( pbD<pbE );

                // decrement the fill value
                usb_cdc_buf_send_fill -= uiBufferLen;
        }

        return uiBufferLen;
}

//-------------------------------------
