/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include "regdef.h"

#include "uart.h"
#include "uprintf.h"

#include "usb.h"
#include "usb_io.h"
#include "usb_requests_top.h"


void usb_handleReset(void);

//---------------------------------------------------------------------------

void usb_init(void)
{
        globalState = USB_State_Powered;

        // no configuration set
        currentConfig = 0;

        // init cdc layer

        // init receive buffer
        usb_cdc_buf_rec_rpos = 0;
        usb_cdc_buf_rec_wpos = 0;
        usb_cdc_buf_rec_fill = 0;

        // init send buffer
        usb_cdc_buf_send_rpos = 0;
        usb_cdc_buf_send_wpos = 0;
        usb_cdc_buf_send_fill = 0;

	// no connection
	tCdcConnectionState = USB_CDC_ConnectionState_Idle;

	// soft reset
	ptNetXUsbArea->ulUsb_core_ctrl = 1;
	// release reset and set ID Func
	ptNetXUsbArea->ulUsb_core_ctrl = 8;

	// set ID pullup and read connector ID value
	ptNetXUsbArea->ulPORT_CTRL = MSK_USB_PORT_CTRL_ID_PU;
	ptNetXUsbArea->ulPORT_CTRL = 0;
}


void usb_deinit(void)
{
        unsigned long ulCnt;


        for(ulCnt=0; ulCnt<4; ++ulCnt) {
                // select pipe
                ptNetXUsbArea->ulPIPE_SEL = ulCnt;
                // deactivate pipe
                ptNetXUsbArea->ulPIPE_CTRL = 0;
                // data buffer invalid
                ptNetXUsbArea->ulPIPE_DATA_TBYTES = 0;
        }

	// clear termination
	ptNetXUsbArea->ulUsb_core_ctrl = 0;
}


USB_CDC_ConnectionState_t usb_pollConnection(void)
{
        unsigned long ulPortEvent;


        if( globalState==USB_State_Powered ) {
		// set termination
		ptNetXUsbArea->ulPORT_CTRL = 0x180;
		// wait for reset event
		ulPortEvent = ptNetXUsbArea->ulMAIN_EV;
		if( (ulPortEvent&MSK_USB_MAIN_EV_GPORT_EV)!=0 ) {
			usb_handleReset();
		}
		return USB_CDC_ConnectionState_Idle;
	} else {
        	// handle enumeration
        	usb_pingpong();
        	// get the connect flag
                return tCdcConnectionState;
        }
}


//-------------------------------------


void usb_pingpong(void)
{
        unsigned int event1;
        unsigned int event2;
        unsigned int packetSize;
        tUsbCdc_BufferState tBufState;


        event1 = ptNetXUsbArea->ulMAIN_EV;
        if( (event1&MSK_USB_MAIN_EV_GPORT_EV)!=0 ) {
                usb_handleReset();
        }

        // is it a pipe event?
        if( (event1&MSK_USB_MAIN_EV_GPIPE_EV)!=0 ) {
                // yes -> test all relevant bits (0-2) of the pipe_ev register
                event2 = ptNetXUsbArea->ulPIPE_EV;

                // test for pipe0 event
                if (event2 & 1) {
                        // clear event
                        ptNetXUsbArea->ulPIPE_EV = 1;

                        // select pipe 0
                        ptNetXUsbArea->ulPIPE_SEL = 0;

                        switch ( ptNetXUsbArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID ) {
                        case DEF_USB_PIPE_CTRL_TPID_SETUP:
                                // Pipe Event detected: SETUP
                                // read Request

                                // get packetsize (in dwords) and convert to bytes
                                packetSize = ptNetXUsbArea->ulPIPE_DATA_PTR << 2;
                                // datasize must be at least standard header
                                if( packetSize==0x08 ) {
                                        usb_io_read_fifo(Usb_Ep0_Buffer>>2, packetSize, setupBuffer);

        				// convert the setup packet into the structure for faster access
        				tSetupPkt.tHeader.reqDir	= (setup_requestDir_t)((setupBuffer[0]&0x80)>>7);
        				tSetupPkt.tHeader.reqType	= (setup_requestType_t)((setupBuffer[0]&0x60)>>5);
        				tSetupPkt.tHeader.reqRec	= (setup_RequestRec_t)(setupBuffer[0]&0x1f);
        				tSetupPkt.tHeader.reqId		= setupBuffer[1];
        				tSetupPkt.tHeader.wValue	= setupBuffer[2] | (setupBuffer[3]<<8);
        				tSetupPkt.tHeader.wIndex	= setupBuffer[4] | (setupBuffer[5]<<8);
        				tSetupPkt.tHeader.wLength	= setupBuffer[6] | (setupBuffer[7]<<8);

        				// does the request have an out transaction?
        				if( tSetupPkt.tHeader.reqDir==SETUP_REQDIR_HostToDevice && tSetupPkt.tHeader.wLength!=0 ) {
                				ptNetXUsbArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT|DEF_USB_PIPE_CTRL_TPID_OUT;
                				ptNetXUsbArea->ulPIPE_DATA_PTR = Usb_Ep0_Buffer>>2;
                				ptNetXUsbArea->ulPIPE_DATA_TBYTES = Usb_Ep0_PacketSize|MSK_USB_PIPE_DATA_TBYTES_DBV;
                				tOutTransactionNeeded = USB_SetupTransaction_OutTransaction;
        				} else {
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
                                if( globalState==USB_State_AddressChangePending ) {
                                        // set new address
                                        ptNetXUsbArea->ulPIPE_ADDR = uiNewAddress;
                                        // address set
                                        globalState = USB_State_Address;
                                }

                                ptNetXUsbArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|Usb_Ep0_PacketSize;	

                                tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;

                                break;

                        case DEF_USB_PIPE_CTRL_TPID_OUT:
                                // pipe out event

                                // data wanted?
                                if( tOutTransactionNeeded==USB_SetupTransaction_OutTransaction ) {
                                        // get packetsize in bytes
                                        packetSize = Usb_Ep2_PacketSize - (ptNetXUsbArea->ulPIPE_DATA_TBYTES&(~MSK_USB_PIPE_DATA_TBYTES_DBV));
                                        if( packetSize<=Usb_Ep0_PacketSize ) {
                                                tSetupPkt.uiDataLen = packetSize;
                                                usb_io_read_fifo(Usb_Ep0_Buffer>>2, packetSize, tSetupPkt.abData);
                                                // correct data toggle
                                                ptNetXUsbArea->ulPIPE_STAT ^= MSK_USB_PIPE_STAT_DT;
                                                usb_requests_handle_request_top(&tSetupPkt);
                                        }
                                }

                                break;
                        }
                }

                // test for pipe 2 event (data from host arrived?)
                if( (event2&4)!=0 ) {
                        // clear event
                        ptNetXUsbArea->ulPIPE_EV = 4;

                        // select pipe 2
                        ptNetXUsbArea->ulPIPE_SEL = 2;

                        if( (ptNetXUsbArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID)==DEF_USB_PIPE_CTRL_TPID_OUT ) {
                                // get packetsize in bytes
                                packetSize = Usb_Ep2_PacketSize - (ptNetXUsbArea->ulPIPE_DATA_TBYTES&(~MSK_USB_PIPE_DATA_TBYTES_DBV));
                                if( packetSize<=Usb_Ep2_PacketSize ) {
                                        usb_io_read_fifo((Usb_Ep2_Buffer>>2), packetSize, receiveBuffer);

                                        // fill bytes into buffer
                                        tBufState = usb_cdc_buf_rec_put(receiveBuffer, packetSize, Usb_Ep2_PacketSize);

                                        switch( tBufState ) {
                                        case tUsbCdc_BufferState_Ok:
                                                // reenable data buffer
                                                usb_activateInputPipe();
                                                tReceiveEpState = USB_ReceiveEndpoint_Running;
                                                break;
                                        case tUsbCdc_BufferState_Full:
                                                // keep the buffer disabled
                                                tReceiveEpState = USB_ReceiveEndpoint_Blocked;
                                                break;
                                        case tUsbCdc_BufferState_Rejected:
                                                // should never happen, the buffer will be blocked before
                                                // TODO: signal the discard with 'serial state' notification
                                                break;
                                        }
                                }
                        }
                }

                // test for pipe 3 event (data to host has been sent?)
                if( (event2&8)!=0 ) {
                        // clear event
                        ptNetXUsbArea->ulPIPE_EV = 8;

                        // select pipe 3
                        ptNetXUsbArea->ulPIPE_SEL = 3;

                        if( (ptNetXUsbArea->ulPIPE_CTRL & MSK_USB_PIPE_CTRL_TPID)==DEF_USB_PIPE_CTRL_TPID_IN) {
                                // is more data waiting?
                                usb_sendPendingPacket();
                        }
                }
        }
}


void usb_sendPendingPacket(void)
{
        unsigned int uiPacketSize;


        uiPacketSize = usb_cdc_buf_send_get(sendBuffer, Usb_Ep3_PacketSize);
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


void usb_activateInputPipe(void)
{
        // select pipe 2
        ptNetXUsbArea->ulPIPE_SEL = 0x02;
        // set data pointer to Usb_Ep2_Buffer
        ptNetXUsbArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
        // enable pipe 2
        ptNetXUsbArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
}


void usb_handleReset(void)
{
        unsigned int event;


        // get the pending port events
        event  = ptNetXUsbArea->ulPSC_EV;
        event &= MSK_USB_PSC_EV_URES_EV;

        // check for USB Reset event
        if( event!=0 ) {
                // clear the reset event
                ptNetXUsbArea->ulPSC_EV = event;

                // set endpoint 0 packet size
                ptNetXUsbArea->ulPIPE_CFG = Usb_Ep0_PacketSize;

                // go to default state
                globalState = USB_State_Default;

                // reset current config (as if we had more than one)
                currentConfig = 0;

                tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
                tReceiveEpState = USB_ReceiveEndpoint_Running;
                tSendEpState = USB_SendEndpoint_Idle;

                // configure the pipes

		// select pipe #1
		ptNetXUsbArea->ulPIPE_SEL = 1;
		// set endpoint number
		ptNetXUsbArea->ulPIPE_ADDR = 1;
		// set max packet size
		ptNetXUsbArea->ulPIPE_CFG = Usb_Ep1_PacketSize;
		// activate pipe and set direction to 'input'
		ptNetXUsbArea->ulPIPE_CTRL = (1<<2)|1;

	        // select pipe #2
	        ptNetXUsbArea->ulPIPE_SEL = 2;
	        // set endpoint number
	        ptNetXUsbArea->ulPIPE_ADDR = 2;
	        // set max packet size
	        ptNetXUsbArea->ulPIPE_CFG = Usb_Ep2_PacketSize;
	        // set data pointer to Usb_Ep2_Buffer
	        ptNetXUsbArea->ulPIPE_DATA_PTR = Usb_Ep2_Buffer>>2;
	        // data buffer valid, ready to receive bytes
	        ptNetXUsbArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV | Usb_Ep2_PacketSize;
	        // activate pipe and set direction to 'output'
	        ptNetXUsbArea->ulPIPE_CTRL = (1<<2)|0;

	        // select pipe #3
	        ptNetXUsbArea->ulPIPE_SEL = 3;
	        // set endpoint number
	        ptNetXUsbArea->ulPIPE_ADDR = 3;
	        // set max packet size
	        ptNetXUsbArea->ulPIPE_CFG = Usb_Ep3_PacketSize;
	        // activate pipe and set direction to 'input'
	        ptNetXUsbArea->ulPIPE_CTRL = (1<<2)|1;
        }
}

//-------------------------------------

