/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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
#include <stddef.h>
#include <stdint.h>

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__


typedef enum TRANSPORT_SEND_PACKET_ACK_ENUM
{
	TRANSPORT_SEND_PACKET_WITHOUT_ACK = 0,
	TRANSPORT_SEND_PACKET_WITH_ACK = 1
} TRANSPORT_SEND_PACKET_ACK_T;


void transport_init(void);
void transport_loop(void);

void transport_send_byte(unsigned char ucData);
void transport_send_bytes(const unsigned char *pucData, unsigned int sizData);
int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck);

unsigned char transport_buffer_get(void);
unsigned char transport_buffer_peek(unsigned int sizOffset);
void transport_buffer_skip(unsigned int sizSkip);

unsigned char transport_call_console_get(void);
void transport_call_console_put(unsigned int uiChar);
unsigned int transport_call_console_peek(void);
void transport_call_console_flush(void);


#endif  /* __TRANSPORT_H__ */
