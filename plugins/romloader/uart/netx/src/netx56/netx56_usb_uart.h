

#ifndef __NETX56_USB_UART_H__
#define __NETX56_USB_UART_H__


void netx56_usb_uart_init(void);

unsigned char netx56_usb_uart_get(void);
void netx56_usb_uart_put(unsigned int iChar);
unsigned int netx56_usb_uart_peek(void);
void netx56_usb_uart_flush(void);


#endif  /* __NETX56_USB_UART_H__ */

