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

#ifndef __usb_requests_common_h__
#define __usb_requests_common_h__


// NOTE: these enum values must match the order of the request packets! Values are casted!
typedef enum {
        SETUP_REQDIR_HostToDevice = 0,
        SETUP_REQDIR_DeviceToHost = 1
} setup_requestDir_t;


// NOTE: these enum values must match the order of the request packets! Values are casted!
typedef enum {
        SETUP_REQTYPE_Standard  = 0,
        SETUP_REQTYPE_Class     = 1,
        SETUP_REQTYPE_Vendor    = 2,
        SETUP_REQTYPE_Reserved  = 3
} setup_requestType_t;


// NOTE: these enum values must match the order of the request packets! Values are casted!
typedef enum {
        SETUP_REQREC_Device     = 0x00,
        SETUP_REQREC_Interface  = 0x01,
        SETUP_REQREC_Endpoint   = 0x02,
        SETUP_REQREC_Other      = 0x03,
        SETUP_REQREC_Reserved04 = 0x04,
        SETUP_REQREC_Reserved05 = 0x05,
        SETUP_REQREC_Reserved06 = 0x06,
        SETUP_REQREC_Reserved07 = 0x07,
        SETUP_REQREC_Reserved08 = 0x08,
        SETUP_REQREC_Reserved09 = 0x09,
        SETUP_REQREC_Reserved0a = 0x0a,
        SETUP_REQREC_Reserved0b = 0x0b,
        SETUP_REQREC_Reserved0c = 0x0c,
        SETUP_REQREC_Reserved0d = 0x0d,
        SETUP_REQREC_Reserved0e = 0x0e,
        SETUP_REQREC_Reserved0f = 0x0f,
        SETUP_REQREC_Reserved10 = 0x10,
        SETUP_REQREC_Reserved11 = 0x11,
        SETUP_REQREC_Reserved12 = 0x12,
        SETUP_REQREC_Reserved13 = 0x13,
        SETUP_REQREC_Reserved14 = 0x14,
        SETUP_REQREC_Reserved15 = 0x15,
        SETUP_REQREC_Reserved16 = 0x16,
        SETUP_REQREC_Reserved17 = 0x17,
        SETUP_REQREC_Reserved18 = 0x18,
        SETUP_REQREC_Reserved19 = 0x19,
        SETUP_REQREC_Reserved1a = 0x1a,
        SETUP_REQREC_Reserved1b = 0x1b,
        SETUP_REQREC_Reserved1c = 0x1c,
        SETUP_REQREC_Reserved1d = 0x1d,
        SETUP_REQREC_Reserved1e = 0x1e,
        SETUP_REQREC_Reserved1f = 0x1f
} setup_RequestRec_t;


typedef struct {
        setup_requestDir_t      reqDir;
        setup_requestType_t     reqType;
        setup_RequestRec_t      reqRec;
        unsigned int            reqId;
        unsigned int            wValue;
        unsigned int            wIndex;
        unsigned int            wLength;
} setupPacket_Header_t;

typedef struct {
        setupPacket_Header_t    tHeader;        // standard header
        unsigned int            uiDataLen;      // size of following data in bytes
        unsigned char           abData[64];     // data
} setupPacket_t;

typedef enum {
        PKT_HANDLER_Send_Nothing		= 0,
        PKT_HANDLER_Send_Zero			= 1,
        PKT_HANDLER_Send_Stall			= 2
} packet_handler_stall_req_t;


typedef packet_handler_stall_req_t (*pfn_packetHandler)(setupPacket_t *pPacket);

//-------------------------------------

packet_handler_stall_req_t usb_requests_handle_unsupported(setupPacket_t *pPacket);

//-------------------------------------


#endif  // __usb_requests_common_h__

