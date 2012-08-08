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

#include "monitor.h"
#include "usb.h"
#include "usb_io.h"
#include "usb_requests_top.h"


/*-------------------------------------------------------------------------*/


void usb_activateInputPipe(void)
{
	HOSTDEF(ptUsbCoreArea);


	/* Select pipe 2. */
	ptUsbCoreArea->ulPIPE_SEL = 0x02;
	/* Set data pointer to Usb_Ep2_Buffer. */
	ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
	/* Enable pipe 2. */
	ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
}


void usb_pingpong(void)
{
	HOSTDEF(ptUsbCoreArea);
	unsigned long ulMainEvent;
	unsigned long ulPipeEvent;
	unsigned long ulPacketSize;
	unsigned long ulValue;


	ulMainEvent = ptUsbCoreArea->ulMAIN_EV;
	if( (ulMainEvent&MSK_USB_MAIN_EV_GPORT_EV)!=0 )
	{
		usb_handleReset();
	}

	/* Is it a pipe event? */
	if( (ulMainEvent&MSK_USB_MAIN_EV_GPIPE_EV)!=0 )
	{
		/* Yes -> test all relevant bits (0-2) of the pipe_ev register. */
		ulPipeEvent = ptUsbCoreArea->ulPIPE_EV;

		/* Test for pipe0 event. */
		if( (ulPipeEvent&(1<<0))!=0 )
		{
			/* Clear event. */
			ptUsbCoreArea->ulPIPE_EV = 1<<0;

			/* Select pipe 0. */
			ptUsbCoreArea->ulPIPE_SEL = 0;

			switch( ptUsbCoreArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID )
			{
			case DEF_USB_PIPE_CTRL_TPID_SETUP:
				/* Pipe Event detected: SETUP */
				/* Read Request. */

				/* Get packet size (in DWORDS) and convert to bytes. */
				ulPacketSize = ptUsbCoreArea->ulPIPE_DATA_PTR << 2;
				/* Data size must be at least standard header. */
				if( ulPacketSize==0x08 )
				{
					usb_io_read_fifo(Usb_Ep0_Buffer>>2, ulPacketSize, setupBuffer);

					/* Convert the setup packet into the structure for faster access. */
					tSetupPkt.tHeader.reqDir   = (setup_requestDir_t)((setupBuffer[0]&0x80U)>>7U);
					tSetupPkt.tHeader.reqType  = (setup_requestType_t)((setupBuffer[0]&0x60U)>>5U);
					tSetupPkt.tHeader.reqRec   = (setup_RequestRec_t)(setupBuffer[0]&0x1fU);
					tSetupPkt.tHeader.reqId    = setupBuffer[1];
					tSetupPkt.tHeader.wValue   = ((unsigned int)(setupBuffer[2])) | ((unsigned int)(setupBuffer[3]<<8U));
					tSetupPkt.tHeader.wIndex   = ((unsigned int)(setupBuffer[4])) | ((unsigned int)(setupBuffer[5]<<8U));
					tSetupPkt.tHeader.wLength  = ((unsigned int)(setupBuffer[6])) | ((unsigned int)(setupBuffer[7]<<8U));

					/* Does the request have an out transaction? */
					if( tSetupPkt.tHeader.reqDir==SETUP_REQDIR_HostToDevice && tSetupPkt.tHeader.wLength!=0 )
					{
						ulValue  = MSK_USB_PIPE_CTRL_ACT;
						ulValue |= DEF_USB_PIPE_CTRL_TPID_OUT;
						ptUsbCoreArea->ulPIPE_CTRL = ulValue;
						
						ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep0_Buffer>>2;
						
						ulValue  = Usb_Ep0_PacketSize;
						ulValue |= MSK_USB_PIPE_DATA_TBYTES_DBV;
						ptUsbCoreArea->ulPIPE_DATA_TBYTES = ulValue;
						
						tOutTransactionNeeded = USB_SetupTransaction_OutTransaction;
					}
					else
					{
						tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
						usb_requests_handle_request_top(&tSetupPkt);
					}
				}
				/* Else ignore packet (strange size) */
				break;


			case DEF_USB_PIPE_CTRL_TPID_IN:
				/* Pipe Event detected: IN */
				/* Just make data buffer again valid to enable USB Status stage. */

				/* Address change pending? */
				if( globalState==USB_State_AddressChangePending )
				{
					/* Set new address. */
					ptUsbCoreArea->ulPIPE_ADDR = uiNewAddress;
					/* Address set. */
					globalState = USB_State_Address;
				}

				ulValue  = MSK_USB_PIPE_DATA_TBYTES_DBV;
				ulValue |= Usb_Ep0_PacketSize;
				ptUsbCoreArea->ulPIPE_DATA_TBYTES = ulValue;

				tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;

				break;


			case DEF_USB_PIPE_CTRL_TPID_OUT:
				/* Pipe out event. */

				/* Data wanted? */
				if( tOutTransactionNeeded==USB_SetupTransaction_OutTransaction )
				{
					/* Get packet size in bytes. */
					ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
					ulValue &= ~MSK_USB_PIPE_DATA_TBYTES_DBV;
					ulPacketSize = Usb_Ep2_PacketSize - ulValue;
					if( ulPacketSize<=Usb_Ep0_PacketSize )
					{
						tSetupPkt.uiDataLen = ulPacketSize;
						usb_io_read_fifo(Usb_Ep0_Buffer>>2, ulPacketSize, tSetupPkt.abData);
						/* Correct data toggle. */
						ulValue  = ptUsbCoreArea->ulPIPE_STAT;
						ulValue ^= MSK_USB_PIPE_STAT_DT;
						ptUsbCoreArea->ulPIPE_STAT = ulValue;
						usb_requests_handle_request_top(&tSetupPkt);
					}
				}

				break;
			}
		}

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
				ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
				ulValue &= ~MSK_USB_PIPE_DATA_TBYTES_DBV;
				ulPacketSize = Usb_Ep2_PacketSize - ulValue;
				if( ulPacketSize<=Usb_Ep2_PacketSize )
				{
					usb_io_read_fifo((Usb_Ep2_Buffer>>2), ulPacketSize, receiveBuffer);

					monitor_process_packet(receiveBuffer, ulPacketSize, MONITOR_USB_MAX_PACKET_SIZE);

					/* Ready for new commands. Reactivate the input pipe. */
					usb_activateInputPipe();
				}
			}
		}
	}
}


void usb_handleReset(void)
{
	HOSTDEF(ptUsbCoreArea);
        unsigned int event;
	unsigned long ulValue;


        // get the pending port events
        event  = ptUsbCoreArea->ulPSC_EV;
        event &= MSK_USB_PSC_EV_URES_EV;

        // check for USB Reset event
        if( event!=0 )
	{
                // clear the reset event
                ptUsbCoreArea->ulPSC_EV = event;

                // set end point 0 packet size
                ptUsbCoreArea->ulPIPE_CFG = Usb_Ep0_PacketSize;

                // go to default state
                globalState = USB_State_Default;

                // reset current configuration (as if we had more than one)
                currentConfig = 0;

                tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
                tReceiveEpState = USB_ReceiveEndpoint_Running;
                tSendEpState = USB_SendEndpoint_Idle;

                // configure the pipes

		/* Select pipe #1. */
		ptUsbCoreArea->ulPIPE_SEL = 1;
		/* Set end point number. */
		ptUsbCoreArea->ulPIPE_ADDR = 1;
		/* Set max packet size. */
		ulValue  = Usb_Ep1_PacketSize;
		ulValue |= MSK_USB_PIPE_CFG_ET_BULK;
		ptUsbCoreArea->ulPIPE_CFG = ulValue;
		/* Activate pipe and set direction to 'input'. */
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
		/* No data to send yet. */
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = 0;
		
		/* Select pipe #2. */
		ptUsbCoreArea->ulPIPE_SEL = 2;
		/* Set end point number. */
		ptUsbCoreArea->ulPIPE_ADDR = 1;
		/* Set max packet size. */
		ulValue  = Usb_Ep2_PacketSize;
		ulValue |= MSK_USB_PIPE_CFG_ET_BULK;
		ptUsbCoreArea->ulPIPE_CFG = ulValue;
		/* Set data pointer to Usb_Ep2_Buffer. */
		ptUsbCoreArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
		/* Data buffer valid, ready to receive bytes. */
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
		/* Activate pipe and set direction to 'output'. */
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_OUT;
	}
}

/*-------------------------------------------------------------------------*/

