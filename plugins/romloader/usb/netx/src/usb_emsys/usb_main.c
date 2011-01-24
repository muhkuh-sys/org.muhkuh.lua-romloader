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

#include "usb_main.h"
#include "usb_io.h"
#include "usb_requests_top.h"
#include "usbmon.h"


//---------------------------------------------------------------------------

static void usb_activateInputPipe(void)
{
	// select pipe 2
	ptUsbCoreArea->ulPIPE_SEL = 0x02;
	// set data pointer to Usb_Ep2_Buffer
	ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
	// enable pipe 2
	ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
}


void usb_pingpong(void)
{
	unsigned long ulMainEvent;
	unsigned long ulPipeEvent;
	unsigned long ulPacketSize;


	ulMainEvent = ptUsbCoreArea->ulMAIN_EV;
	if( (ulMainEvent&MSK_USB_MAIN_EV_GPORT_EV)!=0 )
	{
		usb_handleReset();
	}

	// is it a pipe event?
	if( (ulMainEvent&MSK_USB_MAIN_EV_GPIPE_EV)!=0 )
	{
		// yes -> test all relevant bits (0-2) of the pipe_ev register
		ulPipeEvent = ptUsbCoreArea->ulPIPE_EV;

		// test for pipe0 event
		if( (ulPipeEvent&(1<<0))!=0 )
		{
			// clear event
			ptUsbCoreArea->ulPIPE_EV = 1<<0;

			// select pipe 0
			ptUsbCoreArea->ulPIPE_SEL = 0;

			switch( ptUsbCoreArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID )
			{
			case DEF_USB_PIPE_CTRL_TPID_SETUP:
				// Pipe Event detected: SETUP
				// read Request

				// get packetsize (in dwords) and convert to bytes
				ulPacketSize = ptUsbCoreArea->ulPIPE_DATA_PTR << 2;
				// datasize must be at least standard header
				if( ulPacketSize==0x08 )
				{
					usb_io_read_fifo(Usb_Ep0_Buffer>>2, ulPacketSize, setupBuffer);

					// convert the setup packet into the structure for faster access
					tSetupPkt.tHeader.reqDir   = (setup_requestDir_t)((setupBuffer[0]&0x80)>>7);
					tSetupPkt.tHeader.reqType  = (setup_requestType_t)((setupBuffer[0]&0x60)>>5);
					tSetupPkt.tHeader.reqRec   = (setup_RequestRec_t)(setupBuffer[0]&0x1f);
					tSetupPkt.tHeader.reqId    = setupBuffer[1];
					tSetupPkt.tHeader.wValue   = (unsigned int)(setupBuffer[2] | (setupBuffer[3]<<8U));
					tSetupPkt.tHeader.wIndex   = (unsigned int)(setupBuffer[4] | (setupBuffer[5]<<8U));
					tSetupPkt.tHeader.wLength  = (unsigned int)(setupBuffer[6] | (setupBuffer[7]<<8U));

					// does the request have an out transaction?
					if( tSetupPkt.tHeader.reqDir==SETUP_REQDIR_HostToDevice && tSetupPkt.tHeader.wLength!=0 )
					{
						ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT|DEF_USB_PIPE_CTRL_TPID_OUT;
						ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep0_Buffer>>2;
						ptUsbCoreArea->ulPIPE_DATA_TBYTES = Usb_Ep0_PacketSize|MSK_USB_PIPE_DATA_TBYTES_DBV;
						tOutTransactionNeeded = USB_SetupTransaction_OutTransaction;
					}
					else
					{
						tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
						usb_requests_handle_request_top(&tSetupPkt);
					}
				}
				// else ignore packet (strange size)
				break;

			case DEF_USB_PIPE_CTRL_TPID_IN:
				// Pipe Event detected: IN
				// just make data buffer again valid to enable USB Status stage

				// address change pending?
				if( globalState==USB_State_AddressChangePending )
				{
					// set new address
					ptUsbCoreArea->ulPIPE_ADDR = uiNewAddress;
					// address set
					globalState = USB_State_Address;
				}

				ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|Usb_Ep0_PacketSize;

				tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;

				break;

			case DEF_USB_PIPE_CTRL_TPID_OUT:
				// pipe out event

				// data wanted?
				if( tOutTransactionNeeded==USB_SetupTransaction_OutTransaction )
				{
					// get packetsize in bytes
					ulPacketSize = Usb_Ep2_PacketSize - (ptUsbCoreArea->ulPIPE_DATA_TBYTES&(~MSK_USB_PIPE_DATA_TBYTES_DBV));
					if( ulPacketSize<=Usb_Ep0_PacketSize )
					{
						tSetupPkt.uiDataLen = ulPacketSize;
						usb_io_read_fifo(Usb_Ep0_Buffer>>2, ulPacketSize, tSetupPkt.abData);
						// correct data toggle
						ptUsbCoreArea->ulPIPE_STAT ^= MSK_USB_PIPE_STAT_DT;
						usb_requests_handle_request_top(&tSetupPkt);
					}
				}

				break;
			}
		}
#if 0
		/* Test for pipe 1 event (data to host has been sent?). */
		if( (ulPipeEvent&(1<<1))!=0 )
		{
			/* Clear the event. */
			ptUsbCoreArea->ulPIPE_EV = 1<<1;

			/* Was the last packet a complete packet? */
			if( tSendEpState==USB_SendEndpoint_Running && uiLastPacketSize==Usb_Ep1_PacketSize )
			{
				usb_io_sendDataPacket(1, 0);
			}
		}
#endif
		/* Test for pipe 2 event (data from host arrived?). */
		if( (ulPipeEvent&(1<<2))!=0 )
		{
			/* Clear the event. */
			ptUsbCoreArea->ulPIPE_EV = (1<<2);

			/* Select pipe 2. */
			ptUsbCoreArea->ulPIPE_SEL = 2;

			if( (ptUsbCoreArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID)==DEF_USB_PIPE_CTRL_TPID_OUT )
			{
				/* Get the packetsize in bytes. */
				ulPacketSize = Usb_Ep2_PacketSize - (ptUsbCoreArea->ulPIPE_DATA_TBYTES&(~MSK_USB_PIPE_DATA_TBYTES_DBV));
				if( ulPacketSize<=Usb_Ep2_PacketSize )
				{
					usb_io_read_fifo((Usb_Ep2_Buffer>>2), ulPacketSize, receiveBuffer);

					usbmon_process_packet(receiveBuffer, ulPacketSize);

					/* Ready for new commands. Reactivate the input pipe. */
					usb_activateInputPipe();
				}
			}
		}
	}
}


#if 0
void usb_sendPendingPacket(void)
{
	unsigned int uiPacketSize;


	uiPacketSize = usb_cdc_buf_send_get(sendBuffer, Usb_Ep3_PacketSize);

	uiPacketSize = 0;


	if( uiPacketSize!=0 ) {
		usb_io_write_fifo(Usb_Ep3_Buffer>>2, uiPacketSize, sendBuffer);
		usb_io_sendDataPacket(3, uiPacketSize);
		uiLastPacketSize = uiPacketSize;
		tSendEpState = USB_SendEndpoint_Running;
	} else {
		// no more data waiting and last packet was full? -> send 0 packet
		if( tSendEpState==USB_SendEndpoint_Running && uiLastPacketSize==Usb_Ep3_PacketSize ) {
			usb_io_sendDataPacket(3, 0);
		}
		tSendEpState = USB_SendEndpoint_Idle;
	}
}
#endif

void usb_handleReset(void)
{
        unsigned int event;


        // get the pending port events
        event  = ptUsbCoreArea->ulPSC_EV;
        event &= MSK_USB_PSC_EV_URES_EV;

        // check for USB Reset event
        if( event!=0 )
	{
                // clear the reset event
                ptUsbCoreArea->ulPSC_EV = event;

                // set endpoint 0 packet size
                ptUsbCoreArea->ulPIPE_CFG = Usb_Ep0_PacketSize;

                // go to default state
                globalState = USB_State_Default;

                // reset current config (as if we had more than one)
                currentConfig = 0;

                tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
                tReceiveEpState = USB_ReceiveEndpoint_Running;
                tSendEpState = USB_SendEndpoint_Idle;

                // configure the pipes

		/* Select pipe #1. */
		ptUsbCoreArea->ulPIPE_SEL = 1;
		/* Set endpoint number. */
		ptUsbCoreArea->ulPIPE_ADDR = 1;
		/* Set max packet size. */
		ptUsbCoreArea->ulPIPE_CFG = Usb_Ep1_PacketSize;
		/* Activate pipe and set direction to 'input'. */
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
		/* No data to send yet. */
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = 0;
		
		/* Select pipe #2. */
		ptUsbCoreArea->ulPIPE_SEL = 2;
		/* Set endpoint number. */
		ptUsbCoreArea->ulPIPE_ADDR = 1;
		/* Set max packet size. */
		ptUsbCoreArea->ulPIPE_CFG = Usb_Ep2_PacketSize;
		/* Set data pointer to Usb_Ep2_Buffer. */
		ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
		/* Data buffer valid, ready to receive bytes. */
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
		/* Activate pipe and set direction to 'output'. */
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_OUT;
	}
}

//-------------------------------------

