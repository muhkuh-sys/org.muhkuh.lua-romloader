/***************************************************************************
 *   Copyright (C) 2010 by Hilscher GmbH                                   *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#ifndef __USB_H__
#define __USB_H__


#include <stddef.h>


#define MONITOR_USB_MAX_PACKET_SIZE 2048

void usb_deinit(void);
void usb_init(void);
void usb_reset_fifo(void);

void usb_loop(void);
void usb_send_packet(const unsigned char *pucPacket, size_t sizPacket);
unsigned long usb_get_rx_fill_level(void);
unsigned long usb_get_tx_fill_level(void);
unsigned char usb_get_byte(void);


#endif  /* __USB_H__ */
