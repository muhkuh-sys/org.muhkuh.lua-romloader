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

#include "usb_requests_common.h"

#ifndef __usb_requests_std_h__
#define __usb_requests_std_h__

//-------------------------------------

typedef enum {
	SETUP_STD_REQID_Get_Status                                      = 0x00,
	SETUP_STD_REQID_Clear_Feature                                   = 0x01,
	SETUP_STD_REQID_Set_Feature                                     = 0x03,
	SETUP_STD_REQID_Set_Address                                     = 0x05,
	SETUP_STD_REQID_Get_Descriptor                                  = 0x06,
	SETUP_STD_REQID_Set_Descriptor                                  = 0x07,
	SETUP_STD_REQID_Get_Configuration                               = 0x08,
	SETUP_STD_REQID_Set_Configuration                               = 0x09,
	SETUP_STD_REQID_Get_Interface                                   = 0x0a,
	SETUP_STD_REQID_Set_Interface                                   = 0x0b,
	SETUP_STD_REQID_Synch_Frame                                     = 0x0c
} setup_standard_requestId_t;

//-------------------------------------

packet_handler_stall_req_t usb_requests_handle_std(setupPacket_t *pPacket);

packet_handler_stall_req_t setup_std_set_address(setupPacket_t *pPacket);
packet_handler_stall_req_t setup_std_get_descriptor(setupPacket_t *pPacket);
packet_handler_stall_req_t setup_std_get_configuration(setupPacket_t *pPacket);
packet_handler_stall_req_t setup_std_set_configuration(setupPacket_t *pPacket);
packet_handler_stall_req_t setup_std_get_interface(setupPacket_t *pPacket);
packet_handler_stall_req_t setup_std_set_interface(setupPacket_t *pPacket);

void sendStatus(unsigned int rec, unsigned int idx, unsigned int wLength);
void getConfiguration(unsigned int wLength);
void clearFeature(unsigned int requestRec, unsigned int index, unsigned int feature);
void setFeature(unsigned int requestRec, unsigned int index, unsigned int feature);
void handle_request(unsigned char* buffer);

//-------------------------------------

#endif  // __usb_requests_std_h__
