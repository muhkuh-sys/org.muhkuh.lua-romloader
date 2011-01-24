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


void usb_deinit(void);
void usb_init(void);
void usb_send_byte(unsigned char ucData);
void usb_send_packet(void);
unsigned long usb_get_rx_fill_level(void);
unsigned long usb_get_tx_fill_level(void);
unsigned char usb_get_byte(void);

unsigned char usb_call_console_get(void);
void usb_call_console_put(unsigned int uiChar);
unsigned int usb_call_console_peek(void);
void usb_call_console_flush(void);

void usb_loop(void) __attribute__ ((noreturn));


#endif  /* __USB_H__ */
