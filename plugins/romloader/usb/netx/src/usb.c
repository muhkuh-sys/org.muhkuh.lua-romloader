/***************************************************************************
 *   Copyright (C) 2010 by Hilscher GmbH                                   *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include "usb.h"

#include "netx_io_areas.h"
#include "options.h"
#include "uprintf.h"


/*-----------------------------------*/


#define true (1==1)
#define false (1==0)

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))


/*-----------------------------------*/


static unsigned char aucPacketRx[64];

/*-----------------------------------*/


typedef enum
{
	USB_FIFO_MODE_Packet		= 0,
	USB_FIFO_MODE_Streaming		= 1
} USB_FIFO_MODE_T;

typedef enum
{
	USB_FIFO_ACK_Manual		= 0,
	USB_FIFO_ACK_Auto		= 1
} USB_FIFO_ACK_T;

typedef enum
{
	USB_FIFO_Control_Out		= 0,
	USB_FIFO_Control_In		= 1,
	USB_FIFO_JTag_TX		= 2,
	USB_FIFO_JTag_RX		= 3,
	USB_FIFO_Uart_TX		= 4,
	USB_FIFO_Uart_RX		= 5,
	USB_FIFO_Interrupt_In		= 6
} USB_FIFO_T;

typedef union
{
	unsigned char *puc;
	unsigned short *pus;
	unsigned long *pul;
	unsigned long ul;
} ADR_T;


/*-----------------------------------*/


static void enum_write_config_data(void)
{
	const unsigned long *pulCnt;
	const unsigned long *pulEnd;
	volatile unsigned long *pulDst;


	/* Get a pointer to the start and end of the enumeration data. */
	pulCnt = g_t_options.t_usb_settings.uCfg.aul;
	pulEnd = g_t_options.t_usb_settings.uCfg.aul + (sizeof(USB_CONFIGURATION_T)/sizeof(unsigned long));

	/* Get a pointer to the enumeration ram. */
	pulDst = (volatile unsigned long*)HOSTADDR(usb_dev_enum_ram);

	/* Copy the data. */
	while( pulCnt<pulEnd )
	{
		*(pulDst++) = *(pulCnt++);
	}
}

static int enum_check_config_data(void)
{
	int iOk;
	const unsigned long *pulCnt;
	const unsigned long *pulEnd;
	const unsigned long *pulDst;


	/* Get a pointer to the start and end of the enumeration data. */
	pulCnt = g_t_options.t_usb_settings.uCfg.aul;
	pulEnd = g_t_options.t_usb_settings.uCfg.aul + (sizeof(USB_CONFIGURATION_T)/sizeof(unsigned long));

	/* Get a pointer to the enumeration ram. */
	pulDst = (const unsigned long*)HOSTADDR(usb_dev_enum_ram);

	/* Verify the data. */
	iOk = true;
	while( iOk==true && pulCnt<pulEnd )
	{
		iOk = (*(pulDst++)==*(pulCnt++));
	}

	return iOk;
}


/*-----------------------------------*/


void usb_deinit(void)
{
	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and disable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = 0;
}


int usb_init(void)
{
	unsigned long ulValue;
	int iResult;


	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and enable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_core_enable) | HOSTMSK(usb_dev_cfg_usb_to_jtag_enable);

	/* Set irq mask to 0. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;

	/* Reset all FIFOs. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode | HOSTMSK(usb_dev_fifo_ctrl_conf_reset);
	/* Release the reset. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode;

	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = HOSTMSK(usb_dev_fifo_ctrl_out_handshake_ack);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = 0;

	/* Set the fifo modes.
	   NOTE: This differs from the default config for the UART_TX fifo.
	         It is set to packet mode here. */
	ulValue  = USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Interrupt_In);
	/* Set the auto ack modes.
	   NOTE: This differs from the default config for the UART_TX fifo.
	         It is set to manual mode here. */
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Interrupt_In);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = ulValue;

	enum_write_config_data();
	iResult = enum_check_config_data();

	return iResult;
}


static void usbmon_send_status(USBMON_STATUS_T tStatus)
{
	/* Write the status to the fifo. */
	ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = tStatus;

	/* Send the packet. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;
}


static void usbmon_read(unsigned long ulAddress, unsigned long ulSize, USBMON_ACCESSSIZE_T tAccessSize)
{
	ADR_T uAdrCnt;
	ADR_T uAdrEnd;
	unsigned long ulValue;
	int iCnt;


	/* Get the start address. */
	uAdrCnt.ul = ulAddress;
	/* Get the end address. */
	uAdrEnd.ul = ulAddress + ulSize;

	/* Write status "Ok" to the fifo. */
	ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = USBMON_STATUS_Ok;

	/* Write data bytes to the fifo. */
	do
	{
		/* Get the next data element in the requested access width. */
		switch(tAccessSize)
		{
		case USBMON_ACCESSSIZE_Byte:
			ulValue = *(uAdrCnt.puc++);
			break;

		case USBMON_ACCESSSIZE_Word:
			ulValue = *(uAdrCnt.pus++);
			break;

		case USBMON_ACCESSSIZE_Long:
			ulValue = *(uAdrCnt.pul++);
			break;
		}

		/* Add the data byte-by-byte to the fifo. */
		iCnt = 1<<tAccessSize;
		do
		{
			ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ulValue & 0xff;
			ulValue >>= 8;
			--iCnt;
		} while( iCnt>0 );
	} while( uAdrCnt.ul<uAdrEnd.ul);

	/* Send the packet. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;
}


static void usbmon_write(unsigned long ulAddress, unsigned long ulDataSize, USBMON_ACCESSSIZE_T tAccessSize)
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	ADR_T uAdrDst;
	unsigned long ulValue;


	/* Get the source start address. */
	pucCnt = aucPacketRx + 5;
	/* Get the source end address. */
	pucEnd = aucPacketRx + 5 + ulDataSize;
	/* Get the destination end address. */
	uAdrDst.ul = ulAddress;

	/* Write data bytes to memory. */
	do
	{
		/* Get the next data element in the requested access width. */
		switch(tAccessSize)
		{
		case USBMON_ACCESSSIZE_Byte:
			*(uAdrDst.puc++) = *(pucCnt++);
			break;

		case USBMON_ACCESSSIZE_Word:
			ulValue  = *(pucCnt++);
			ulValue |= *(pucCnt++) << 8;
			*(uAdrDst.pus++) = (unsigned short)ulValue;
			break;

		case USBMON_ACCESSSIZE_Long:
			ulValue  = *(pucCnt++);
			ulValue |= *(pucCnt++) << 8;
			ulValue |= *(pucCnt++) << 16;
			ulValue |= *(pucCnt++) << 24;
			*(uAdrDst.pul++) = ulValue;
			break;
		}
	} while( pucCnt<pucEnd);

	/* Send the status packet. */
	usbmon_send_status(USBMON_STATUS_Ok);
}



static unsigned char aucNetx10Buffer[64];

static unsigned char netx10_serial_get(unsigned int uiHandle)
{}


static void netx10_serial_put_t(unsigned int uiHandle, unsigned int iChar)
{}


static unsigned int netx10_serial_peek(unsigned int uiHandle)
{}


static void netx10_serial_flush(unsigned int uiHandle)
{}


static void usbmon_call(unsigned long ulAddress, unsigned long ulR0)
{
	uprintf("Call 0x%08x(0x%08x)\n", ulAddress, ulR0);

	/* Send the status packet. */
	usbmon_send_status(USBMON_STATUS_Ok);

	/* Set the vectors. */

	/* Call the routine. */

	/* The call finished, notify the PC. */
	usbmon_send_status(USBMON_STATUS_CallFinished);
}


static unsigned long get_unaligned_dword(const unsigned char *pucBuffer)
{
	unsigned long ulValue;


	ulValue  = pucBuffer[0];
	ulValue |= pucBuffer[1]<<8U;
	ulValue |= pucBuffer[2]<<16U;
	ulValue |= pucBuffer[3]<<24U;

	return ulValue;
}


static void usb_process_packet(unsigned long ulPacketSize)
{
	USBMON_COMMAND_T tCmd;
	unsigned long ulDataSize;
	unsigned long ulAddress;
	USBMON_ACCESSSIZE_T tAccessSize;
	unsigned long ulR0;


	/* Get the command and the data size from the first byte. */
	tCmd = (USBMON_COMMAND_T)(aucPacketRx[0] >> 5U);
	ulDataSize = aucPacketRx[0] & 0x1fU;

	/* Get the address. */
	ulAddress = get_unaligned_dword(aucPacketRx + 1);

	if( tCmd==USBMON_COMMAND_Execute )
	{
		if( ulPacketSize!=9U )
		{
			usbmon_send_status(USBMON_STATUS_InvalidPacketSize);
		}
		else
		{
			ulR0 = get_unaligned_dword(aucPacketRx + 5);
			usbmon_call(ulAddress, ulR0);
		}
	}
	else if( tCmd==USBMON_COMMAND_Reserved )
	{
		uprintf("[USBMON] reserved.\n");
	}
	else
	{
		tAccessSize = (USBMON_ACCESSSIZE_T)(tCmd&3);

		/* Distinguish read and write commands by bit 2.
		   NOTE: This works because read commands start at 0 and write
		         commands at 4. */
		if( (tCmd&4)==0 )
		{
			uprintf("[USBMON] read data.\n");
			if( ulPacketSize!=5 )
			{
				usbmon_send_status(USBMON_STATUS_InvalidPacketSize);
			}
			else if( ulDataSize>31 )
			{
				usbmon_send_status(USBMON_STATUS_InvalidSizeParameter);
			}
			else
			{
				usbmon_read(ulAddress, ulDataSize, tAccessSize);
			}
		}
		else
		{
			uprintf("[USBMON] write data.\n");
			if( ulPacketSize<6 )
			{
				uprintf("[USBMON] packet too small: %d\n", ulPacketSize);
				usbmon_send_status(USBMON_STATUS_InvalidPacketSize);
			}
			else if( ulDataSize>27 )
			{
				uprintf("[USBMON] packet too big: %d\n", ulPacketSize);
				usbmon_send_status(USBMON_STATUS_InvalidSizeParameter);
			}
			else
			{
				usbmon_write(ulAddress, ulDataSize, tAccessSize);
			}
		}
	}
}


void usb_loop(void)
{
	unsigned long ulUsbStateOld;
	unsigned long ulUsbState;
	unsigned long ulFillLevel;
	unsigned char *pucCnt;
	unsigned char *pucEnd;


	ulUsbStateOld = 0xffffffff;

	/* start loopback */
	while(1)
	{
		ulUsbState  = ptUsbDevCtrlArea->ulUsb_dev_status;
		ulUsbState &= HOSTMSK(usb_dev_status_usb_bus_reset) |
		              HOSTMSK(usb_dev_status_usb_configured) |
		              HOSTMSK(usb_dev_status_usb_addressed);
		if( ulUsbStateOld!=ulUsbState )
		{
			uprintf("State: 0x%08x\n", ulUsbState);
/*
			if( (ulUsbState&HOSTMSK(usb_dev_status_usb_bus_reset))!=0 )
			{
				uprintf("USB Reset\n");
			}
			if( (ulUsbState&HOSTMSK(usb_dev_status_usb_configured))!=0 )
			{
				uprintf("USB Configured\n");
			}
			if( (ulUsbState&HOSTMSK(usb_dev_status_usb_addressed))!=0 )
			{
				uprintf("USB Addressed\n");
			}
*/
			ulUsbStateOld = ulUsbState;
		}

		/* wait for a character in the input fifo */

		/* get the Uart RX input fill level */
		ulFillLevel  = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
		ulFillLevel &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
		if( ulFillLevel>0 )
		{
			/* Is the fill level valid? */
			if( ulFillLevel>64 )
			{
				/* No -> reset the fifo. */
				ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = 1<<USB_FIFO_Uart_RX;
			}
			else
			{
				/* Copy the complete packet to the buffer. */
				pucCnt = aucPacketRx;
				pucEnd = aucPacketRx + ulFillLevel;
				do
				{
					*(pucCnt++) = (unsigned char)ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
				} while( pucCnt<pucEnd );

				usb_process_packet(ulFillLevel);
			}
		}
	}
}



