/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include "usb_requests_top.h"


#include "usb_descriptors.h"
#include "usb_globals.h"
#include "usb_io.h"
#include "usb_requests_std.h"


// line coding
static unsigned char abLineCoding[7];


void usb_requests_handle_request_top(setupPacket_t *ptSetupPkt)
{
	HOSTDEF(ptUsbCoreArea);
	packet_handler_stall_req_t tSendStall;
	setup_cdc_requestId_t tCdcReqId;
	unsigned int uiIdx;


	tSendStall = PKT_HANDLER_Send_Stall;

	// distinguish the type
	if( ptSetupPkt->tHeader.reqType==SETUP_REQTYPE_Standard )
	{
		switch(ptSetupPkt->tHeader.reqId)
		{
		case SETUP_STD_REQID_Set_Address:
			if(
				// this request is only allowed for the device
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Device && 
				// only allowed in default and addressed state
				(globalState==USB_State_Default || globalState==USB_State_Address)
			  )
			{
				// remember new address
				uiNewAddress = ptSetupPkt->tHeader.wValue << 4;
				// now we're in address state
				globalState = USB_State_AddressChangePending;
				tSendStall = PKT_HANDLER_Send_Zero;
			}
			break;


		case SETUP_STD_REQID_Get_Descriptor:
			if(
				// data source must be device for this request
				ptSetupPkt->tHeader.reqDir==SETUP_REQDIR_DeviceToHost &&
				// this request is only valid for the device
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Device &&
				// this request is only valid in Default, Address and Configured state
				(globalState!=USB_State_Powered && globalState!=USB_State_AddressChangePending)
			  )
			{
				tSendStall = getDescriptor(ptSetupPkt);
			}
			break;


		case SETUP_STD_REQID_Get_Configuration:
			if(
				// data source must be device for this request
				ptSetupPkt->tHeader.reqDir==SETUP_REQDIR_DeviceToHost &&
				// this request is only valid for the device
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Device &&
				// only allowed in configured and addressed state
				globalState>=USB_State_Address
			  )
			{
				sendDescriptor(ptSetupPkt->tHeader.wLength, 1, (unsigned char*)&currentConfig);
				tSendStall = PKT_HANDLER_Send_Nothing;
			}
			break;


		case SETUP_STD_REQID_Set_Configuration:
			if(
				// this request is only valid for the device
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Device &&
				// this request is only valid in address and configured state
				globalState>=USB_State_Address
			  )
			{
				uiIdx  = (ptSetupPkt->tHeader.wValue & 0xff);

				if( uiIdx<2 )
				{
					// set current config
					currentConfig = uiIdx;
					globalState = uiIdx + USB_State_Address;
					tSendStall = PKT_HANDLER_Send_Zero;
				}
			}
			break;


		case SETUP_STD_REQID_Get_Interface:
			if(
				// data source must be device for this request
				ptSetupPkt->tHeader.reqDir==SETUP_REQDIR_DeviceToHost &&
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Interface &&
				globalState==USB_State_Configured
			  )
			{
				uiIdx = ptSetupPkt->tHeader.wIndex;
				if( uiIdx<2 )
				{
					sendDescriptor(ptSetupPkt->tHeader.wLength, 1, (unsigned char*)&uiIdx);                                
					tSendStall = PKT_HANDLER_Send_Nothing;
				}
			}
			break;
	
	
		case SETUP_STD_REQID_Set_Interface:
			if(
				ptSetupPkt->tHeader.reqRec==SETUP_REQREC_Interface &&
				globalState!=USB_State_Configured &&
				// set interface
				ptSetupPkt->tHeader.wIndex<2
			  )
			{
				// ok
				tSendStall = PKT_HANDLER_Send_Zero;
			}
			break;
		}
	}
	else if( ptSetupPkt->tHeader.reqType==SETUP_REQTYPE_Class )
	{
		tCdcReqId = (setup_cdc_requestId_t)ptSetupPkt->tHeader.reqId;
		switch( tCdcReqId )
		{
		case SETUP_CDC_REQID_Set_Line_Coding:
			uiIdx = 7;
			do
			{
				--uiIdx;
				abLineCoding[uiIdx] = ptSetupPkt->abData[uiIdx];
			} while( uiIdx!=0 );
			tSendStall = PKT_HANDLER_Send_Zero;
			break;

		case SETUP_CDC_REQID_Get_Line_Coding:
			sendDescriptor(ptSetupPkt->tHeader.wLength, 7, abLineCoding);
			tSendStall = PKT_HANDLER_Send_Nothing;
			break;

		case SETUP_CDC_REQID_Set_Control_Line_State:
			// eval DTR (bit 0) and RTS (bit 1) to enable or disable send and receive part
			tCdcConnectionState = (USB_CDC_ConnectionState_t)(ptSetupPkt->tHeader.wValue & 1);
			// send confirmation
			tSendStall = PKT_HANDLER_Send_Zero;
			break;
		}
	}


	// send stall on request
	if( tSendStall==PKT_HANDLER_Send_Stall )
	{
		ptUsbCoreArea->ulPIPE_CFG |= MSK_USB_PIPE_CFG_STALL;
	}
	else if( tSendStall==PKT_HANDLER_Send_Zero )
	{
		usb_io_sendDataPacket(0, 0);
	}
}


