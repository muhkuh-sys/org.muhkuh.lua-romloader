

#include "netx_io_areas.h"

#include "netx56_usb_uart.h"


void netx56_usb_uart_init(void)
{
	
}


unsigned char netx56_usb_uart_get(void)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	/* Get the Usb RX input fill level. */
	do
	{
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_uart_ep_rx_stat;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_uart_ep_rx_stat_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_uart_ep_rx_stat_fill_level);
	} while ( ulFillLevel==0 );

	/* Copy one byte from the receive to the transmit fifo. */
	return (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_uart_rx_data);
}


void netx56_usb_uart_put(unsigned int iChar)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	/* Get the Usb TX input fill level. */
	do
	{
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_uart_ep_tx_stat;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_uart_ep_tx_stat_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_uart_ep_tx_stat_fill_level);
	} while ( ulFillLevel>=64 );

	ptUsbDevFifoArea->ulUsb_dev_uart_tx_data = (unsigned char)iChar;
}


unsigned int netx56_usb_uart_peek(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	/* Get the Usb RX input fill level. */
	ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_uart_ep_rx_stat;
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_uart_ep_rx_stat_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_uart_ep_rx_stat_fill_level);

	return (ulFillLevel==0) ? 0U : 1U;
}


void netx56_usb_uart_flush(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	/* Get the Usb TX input fill level. */
	do
	{
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_uart_ep_tx_stat;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_uart_ep_tx_stat_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_uart_ep_tx_stat_fill_level);
	} while ( ulFillLevel!=0 );
}

