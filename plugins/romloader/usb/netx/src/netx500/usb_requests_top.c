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

#include "netx_io_areas.h"

#include "usb_io.h"
#include "usb_requests_common.h"
#include "usb_requests_top.h"
#include "usb_requests_std.h"
#include "usb_globals.h"
#include "usb_descriptors.h"

#include "uprintf.h"


static packet_handler_stall_req_t getStatus(setupPacket_t *pPacket)
{
	packet_handler_stall_req_t tResult;
	unsigned int uiReqTyp;
	int iSendStatus;
	unsigned short usStatus;
	unsigned int uiEndpoint;
	unsigned char aucBuffer[2];


	/* Send a stall by default. */
	iSendStatus = 0;
	usStatus = 0;

	uiReqTyp = pPacket->tHeader.reqType;
	uprintf("ReqestType: 0x%08x\n", uiReqTyp);

	uiReqTyp &= 3U;
	switch(uiReqTyp)
	{
	case 0:
		/* Get Device Status. */
		iSendStatus = 1;
		usStatus = 0x0001;
		break;

	case 1:
		/* Get the Interface status. */
		/* This is reserved, always send 0. */
		iSendStatus = 1;
		usStatus = 0x0000;
		break;

	case 3:
		/* Get the endpoint status. */
		uiEndpoint = pPacket->tHeader.wIndex & 0xffU;
		uprintf("endpoint: 0x%08x\n", uiEndpoint);
		iSendStatus = 1;
		usStatus = 0x0000;
	}

	if( iSendStatus!=0 )
	{
		aucBuffer[0] = (unsigned char)( usStatus      & 0xffU);
		aucBuffer[1] = (unsigned char)((usStatus>> 8) & 0xffU);
		usb_io_write_fifo(0x00, 2, aucBuffer);
		usb_io_sendDataPacket(0, 2);
		tResult = PKT_HANDLER_Send_Nothing;
	}
	else
	{
		tResult = PKT_HANDLER_Send_Stall;
	}

	return tResult;
}

void usb_requests_handle_request_top(setupPacket_t *ptSetupPkt)
{
	packet_handler_stall_req_t tSendStall;
	unsigned int uiIdx;


	tSendStall = PKT_HANDLER_Send_Stall;

	// distinguish the type
	if( ptSetupPkt->tHeader.reqType==SETUP_REQTYPE_Standard )
	{
		switch(ptSetupPkt->tHeader.reqId)
		{
		case SETUP_STD_REQID_Get_Status:
			if(
				/* Data source must be device for this request. */
				ptSetupPkt->tHeader.reqDir==SETUP_REQDIR_DeviceToHost
			  )
			{
				tSendStall = getStatus(ptSetupPkt);
			}
			break;


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

		default:
			uprintf("unknown request!\n");
			uprintf("id: %08x\n", ptSetupPkt->tHeader.reqId);
			uprintf("rec: %08x\n", ptSetupPkt->tHeader.reqRec);
			break;
	        }
	}

	/* Send stall on request. */
	if( tSendStall==PKT_HANDLER_Send_Stall )
	{
		ptUsbCoreArea->ulPIPE_CFG |= MSK_USB_PIPE_CFG_STALL;
	}
	else if( tSendStall==PKT_HANDLER_Send_Zero )
	{
		usb_io_sendDataPacket(0, 0);
	}
}


