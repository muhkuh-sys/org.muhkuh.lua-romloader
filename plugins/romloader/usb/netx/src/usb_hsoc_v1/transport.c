/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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


#include "transport.h"

#include <string.h>

#include "monitor.h"
#include "monitor_commands.h"
#include "netx_io_areas.h"
#include "options.h"
#include "systime.h"


typedef enum ACK_RESULT_ENUM
{
	ACK_RESULT_SequenceAckd       =  0,
	ACK_RESULT_OperationCanceled  = -1,
	ACK_RESULT_Timeout            = -2
} ACK_RESULT_T;


#define MONITOR_MAX_PACKET_SIZE_USB 64

/* Resend a packet after 1000ms if no ACK was received. */
#define USB_RESEND_TIMEOUT_MS 1000

static unsigned char aucPacketRx[MONITOR_MAX_PACKET_SIZE_USB];
static unsigned char aucPacketTx[MONITOR_MAX_PACKET_SIZE_USB];

static unsigned int sizPacketTxFill;
static unsigned int sizPacketRxHead;
static unsigned int sizPacketRxFill;

static unsigned char ucSequence;


/*-----------------------------------*/


typedef enum
{
	USB_FIFO_MODE_Packet            = 0,
	USB_FIFO_MODE_Streaming         = 1
} USB_FIFO_MODE_T;

typedef enum
{
	USB_FIFO_ACK_Manual             = 0,
	USB_FIFO_ACK_Auto               = 1
} USB_FIFO_ACK_T;

typedef enum
{
	USB_FIFO_Control_Out            = 0,
	USB_FIFO_Control_In             = 1,
	USB_FIFO_JTag_TX                = 2,
	USB_FIFO_JTag_RX                = 3,
	USB_FIFO_Uart_TX                = 4,
	USB_FIFO_Uart_RX                = 5,
	USB_FIFO_Interrupt_In           = 6
} USB_FIFO_T;



/* This is a very nice routine for the CITT XModem CRC from
 * http://www.eagleairaust.com.au/code/crc16.htm or now
 * https://web.archive.org/web/20130106081206/http://www.eagleairaust.com.au/code/crc16.htm
 */
static unsigned short crc16(unsigned short usCrc, unsigned char ucData)
{
	unsigned int uiCrc;


	uiCrc  = (usCrc >> 8U) | ((usCrc & 0xffU) << 8U);
	uiCrc ^= ucData;
	uiCrc ^= (uiCrc & 0xffU) >> 4U;
	uiCrc ^= (uiCrc & 0x0fU) << 12U;
	uiCrc ^= ((uiCrc & 0xffU) << 4U) << 1U;

	return (unsigned short)uiCrc;
}


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



static void usb_reset_fifo(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);


	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode | HOSTMSK(usb_dev_fifo_ctrl_conf_reset);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode;
}



static void usb_init(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;


	/* Set IRQ mask to 0. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;

	/* ACK all IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = 0xffffffffU;

	/* Reset all FIFOs. */
	usb_reset_fifo();

	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = HOSTMSK(usb_dev_fifo_ctrl_out_handshake_ack);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = 0;

	/* Set the FIFO modes.
	   NOTE: This differs from the default configuration for the UART_TX FIFO.
	         It is set to packet mode here. */
	ulValue  = USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Interrupt_In);
	/* Set the auto ACK modes.
	   NOTE: This differs from the default configuration for the UART_TX FIFO.
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
}



void transport_init(void)
{
	monitor_init();

	sizPacketTxFill = 0;
	sizPacketRxHead = 0;
	sizPacketRxFill = 0;
	ucSequence = 0;

	usb_init();
}



static void transport_send_ack(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned int uiCnt;
	unsigned short usCrc16;
	unsigned char aucACK[7];
	unsigned long ulValue;


	aucACK[0] = MONITOR_STREAM_PACKET_START;
	aucACK[1] = 2U;  /* The low byte of the data size. */
	aucACK[2] = 0U;  /* The high byte of the data size. */
	aucACK[3] = ucSequence;
	aucACK[4] = MONITOR_PACKET_TYP_ACK;

	/* Generate the CRC for the packet. */
	usCrc16 = 0;
	for(uiCnt=1; uiCnt<5; ++uiCnt)
	{
		usCrc16 = crc16(usCrc16, aucACK[uiCnt]);
	}
	aucACK[5] = (unsigned char)(usCrc16 >> 8U);
	aucACK[6] = (unsigned char)(usCrc16 & 0xff);

	/* Wait for free bytes in the FIFO. */
	do
	{
		ulValue  = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
		ulValue &= HOSTMSK(usb_dev_fifo_ctrl_status4_in_fill_level)|HOSTMSK(usb_dev_fifo_ctrl_status4_out_fill_level);
	} while( ulValue!=0 );

	/* Write data to the FIFO. */
	uiCnt = 0;
	do
	{
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = aucACK[uiCnt];
		++uiCnt;
	} while( uiCnt<sizeof(aucACK) );

	/* Trigger packet send. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;

	/* Wait until the FIFO is empty. */
	/* NOTE: without this, the transfer fails under very rare conditions.
	         I have no idea if this check is enough and the irq_raw condition can
	         be skipped, so I choose the safe side and keep both.
	*/
	do
	{
		ulValue  = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
		ulValue &= HOSTMSK(usb_dev_fifo_ctrl_status4_in_fill_level)|HOSTMSK(usb_dev_fifo_ctrl_status4_out_fill_level);
	} while( ulValue!=0 );

	/* Wait until the packet is sent. */
	do
	{
		ulValue  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
		ulValue &= HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
	} while( ulValue==0 );

	/* Acknowledge the IRQ. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
}



void transport_loop(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned char *pucCnt;
	unsigned char *pucEnd;
	unsigned char ucPacketSequenceNumber;
	unsigned int sizPacket;
	unsigned int sizCrcPosition;
	unsigned short usCrc16;


	/* Wait for a new packet. */
	ulValue  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
	ulValue &= HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);
	if( ulValue!=0 )
	{
		sizPacketRxFill = 0;
		sizPacketRxHead = 0;
		pucCnt = aucPacketRx;

		/* Acknowledge the IRQ. */
		ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);

		/* Get the UART RX input fill level. */
		ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
		if( ulFillLevel>0 )
		{
			/* Is the fill level valid? */
			if( ulFillLevel>MONITOR_MAX_PACKET_SIZE_USB )
			{
				/* Reset the FIFO. */
				usb_reset_fifo();
			}
			else
			{
				/* Copy the complete packet to the buffer. */
				pucEnd = pucCnt + ulFillLevel;
				do
				{
					*(pucCnt++) = (unsigned char)(ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX]);
				} while( pucCnt<pucEnd );

				sizPacketRxFill = ulFillLevel;
			}
		}

		/* The minimum packet size is 5. */
		if( sizPacketRxFill>=5U )
		{
			/* Does the packet start with the identifier? */
			if( aucPacketRx[0]==MONITOR_STREAM_PACKET_START )
			{
				sizPacket  =  (unsigned int)(aucPacketRx[1]);
				sizPacket |= ((unsigned int)(aucPacketRx[2])) << 8U;
				if( sizPacket==0U )
				{
					if( aucPacketRx[3]=='*' && aucPacketRx[4]=='#' )
					{
						/* Discard the size and magic. */
						transport_buffer_skip(5);

						/* This is a knock sequence. Respond with the magic cookie and version info. */
						monitor_send_magic(MONITOR_MAX_PACKET_SIZE_USB);
					}
				}
				/* Does the data fit into the received packet?
				 * The packet has these extra fields which are not part of the data:
				 *   1 byte stream start
				 *   2 bytes data size
				 *   2 bytes CRC16
				 */
				else if( sizPacket+5U==sizPacketRxFill )
				{
					/* Loop over all bytes and build the CRC16 checksum. */
					/* NOTE: the size is just for the user data, but the CRC16 includes the size. */
					usCrc16 = 0;
					sizCrcPosition = 1U;
					while( sizCrcPosition<sizPacketRxFill )
					{
						usCrc16 = crc16(usCrc16, aucPacketRx[sizCrcPosition]);
						++sizCrcPosition;
					}

					if( usCrc16==0 )
					{
						/* OK, the CRC matches! */

						/* Get the sequence number. */
						ucPacketSequenceNumber = aucPacketRx[3];
						if( ucSequence==ucPacketSequenceNumber )
						{
							/* Send an ACK packet here. */
							transport_send_ack();

							/* Increase the sequence number. */
							++ucSequence;

							/* Move the read pointer after the sequence number. */
							sizPacketRxHead = 4U;
							sizPacketRxFill -= 4U;

							/* Process the rest of the packet. */
							monitor_process_packet(sizPacketRxFill - 2U, MONITOR_MAX_PACKET_SIZE_USB - 6U);
						}
					}
				}
			}
		}
	}
}



static ACK_RESULT_T transport_wait_for_ack(unsigned char ucRequiredSequenceNumber, unsigned long ulTimeoutMs)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	ACK_RESULT_T tResult;
	unsigned long ulTimerHandle;
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned int sizPacket;
	unsigned int sizCrcPosition;
	unsigned short usCrc16;
	unsigned char *pucCnt;
	unsigned char *pucEnd;
	unsigned char ucPacketSequenceNumber;
	MONITOR_PACKET_TYP_T tPacketTyp;
	int iIsElapsed;


	tResult = ACK_RESULT_Timeout;

	ulTimerHandle = systime_get_ms();
	do
	{
		/* Wait for a packet. */
		ulValue  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
		ulValue &= HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);
		if( ulValue!=0 )
		{
			/* Acknowledge the IRQ. */
			ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);

			/* Get the UART RX input fill level. */
			ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
			ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
			ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
			if( ulFillLevel>0 )
			{
				/* Is the fill level valid? */
				if( ulFillLevel>MONITOR_MAX_PACKET_SIZE_USB )
				{
					/* Reset the FIFO. */
					usb_reset_fifo();
				}
				else
				{
					/* Copy the complete packet to the buffer. */
					pucCnt = aucPacketRx;
					pucEnd = aucPacketRx + ulFillLevel;
					do
					{
						*(pucCnt++) = (unsigned char)ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
					} while( pucCnt<pucEnd );

					/* Does the packet start with the identifier? */
					if( aucPacketRx[0]==MONITOR_STREAM_PACKET_START )
					{
						/* Loop over all bytes and build the CRC16 checksum. */
						/* NOTE: the size is just for the user data, but the CRC16 includes the size. */
						usCrc16 = 0;
						sizCrcPosition = 1U;
						while( sizCrcPosition<ulFillLevel )
						{
							usCrc16 = crc16(usCrc16, aucPacketRx[sizCrcPosition]);
							++sizCrcPosition;
						}

						if( usCrc16==0 )
						{
							/* OK, the CRC matches! */
							sizPacket  =  (unsigned int)(aucPacketRx[1]);
							sizPacket |= ((unsigned int)(aucPacketRx[2])) << 8U;

							/* Get the sequence number. */
							ucPacketSequenceNumber = aucPacketRx[3];

							/* Get the packet type. */
							tPacketTyp = (MONITOR_PACKET_TYP_T)aucPacketRx[4];

							/* Is this an ACK packet with the correct sequence number? */
							if( ulFillLevel==7 && sizPacket==2 && tPacketTyp==MONITOR_PACKET_TYP_ACK && ucPacketSequenceNumber==ucRequiredSequenceNumber )
							{
								tResult = ACK_RESULT_SequenceAckd;
								break;
							}
							else if( ulFillLevel==2 && tPacketTyp==MONITOR_PACKET_TYP_CancelOperation )
							{
								tResult = ACK_RESULT_OperationCanceled;
								break;
							}
						}
					}
				}
			}
		}

		iIsElapsed = systime_elapsed(ulTimerHandle, ulTimeoutMs);
	} while( iIsElapsed==0 );

	return tResult;
}



unsigned char transport_buffer_get(void)
{
        unsigned char ucByte;


        ucByte = aucPacketRx[sizPacketRxHead];

        ++sizPacketRxHead;
        if( sizPacketRxHead>=MONITOR_MAX_PACKET_SIZE_USB )
        {
                sizPacketRxHead -= MONITOR_MAX_PACKET_SIZE_USB;
        }

        --sizPacketRxFill;

        return ucByte;
}



unsigned char transport_buffer_peek(unsigned int sizOffset)
{
        unsigned int sizReadPosition;
        unsigned char ucByte;


        sizReadPosition = sizPacketRxHead + sizOffset;
        if( sizReadPosition>=MONITOR_MAX_PACKET_SIZE_USB )
        {
                sizReadPosition -= MONITOR_MAX_PACKET_SIZE_USB;
        }

        ucByte = aucPacketRx[sizReadPosition];

        return ucByte;
}



void transport_buffer_skip(unsigned int sizSkip)
{
        sizPacketRxHead += sizSkip;
        if( sizPacketRxHead>=MONITOR_MAX_PACKET_SIZE_USB )
        {
                sizPacketRxHead -= MONITOR_MAX_PACKET_SIZE_USB;
        }

        sizPacketRxFill -= sizSkip;
}



void transport_send_byte(unsigned char ucData)
{
	if( sizPacketTxFill<MONITOR_MAX_PACKET_SIZE_USB )
	{
		aucPacketTx[sizPacketTxFill] = ucData;
		++sizPacketTxFill;
	}
}



void transport_send_bytes(const unsigned char *pucData, unsigned int sizData)
{
	/* Does the data still fit into the output buffer? */
	if( (sizPacketTxFill+sizData)>MONITOR_MAX_PACKET_SIZE_USB )
	{
		/* No -> copy only what still fits. */
		sizData = MONITOR_MAX_PACKET_SIZE_USB - sizPacketTxFill;
	}

	/* Copy the data to the buffer. */
	memcpy(aucPacketTx + sizPacketTxFill, pucData, sizData);

	sizPacketTxFill += sizData;
}



int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned long ulRawIrq;
	ACK_RESULT_T tResult;
	int iResult;
	unsigned int sizPacketData;
	unsigned short usCrc16;
	unsigned char ucData;


	do
	{
		/* Send the stream start. */
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = MONITOR_STREAM_PACKET_START;

		/* Send the data size. */
		sizPacketData = sizPacketTxFill + 1U;
		ucData =  sizPacketData        & 0xffU;
		usCrc16 = crc16(0, ucData);
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ucData;
		ucData = (sizPacketData >> 8U) & 0xffU;
		usCrc16 = crc16(usCrc16, ucData);
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ucData;
		/* Send the sequence byte. */
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ucSequence;
		usCrc16 = crc16(usCrc16, ucSequence);

		pucCnt = aucPacketTx;
		pucEnd = aucPacketTx + sizPacketTxFill;
		while( pucCnt<pucEnd )
		{
			ucData = *(pucCnt++);
			usCrc16 = crc16(usCrc16, ucData);
			ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ucData;
		}

		/* Send the CRC16. */
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = (unsigned char)(usCrc16 >> 8U);
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = (unsigned char)(usCrc16 & 0xff);

		/* Trigger packet send. */
		ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
		ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;

		/* Wait until the FIFO is empty. */
		/* NOTE: without this, the transfer fails under very rare conditions.
		         I have no idea if this check is enough and the irq_raw condition can
		         be skipped, so I choose the safe side and keep both.
		*/
		do
		{
			ulRawIrq  = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
			ulRawIrq &= HOSTMSK(usb_dev_fifo_ctrl_status4_in_fill_level)|HOSTMSK(usb_dev_fifo_ctrl_status4_out_fill_level);
		} while( ulRawIrq!=0 );

		/* Wait until the packet is sent. */
		do
		{
			ulRawIrq  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
			ulRawIrq &= HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
		} while( ulRawIrq==0 );

		/* Acknowledge the IRQ. */
		ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);

		if( tRequireAck==TRANSPORT_SEND_PACKET_WITHOUT_ACK )
		{
			tResult = ACK_RESULT_SequenceAckd;
		}
		else
		{
			/* Wait for ACK. */
			tResult = transport_wait_for_ack(ucSequence, USB_RESEND_TIMEOUT_MS);
			if( tResult==ACK_RESULT_SequenceAckd )
			{
				/* Increase the sequence number. */
				++ucSequence;
			}
		}
	} while( tResult==ACK_RESULT_Timeout );

	sizPacketTxFill = 0;

	if( tResult==ACK_RESULT_SequenceAckd )
	{
		iResult = 0;
	}
	else
	{
		iResult = -1;
	}

	return iResult;
}



unsigned char transport_call_console_get(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	HOSTDEF(ptUsvDevFifoArea);
	unsigned long ulFillLevel;
	unsigned char ucData;


	/* Wait for a byte in the FIFO. */
	do
	{
		ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
	} while( ulFillLevel==0 );

	/* Get a byte from the FIFO. */
	ucData = (unsigned char)ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
	return ucData;
}



void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the FIFO. */
	transport_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size?
	 * The maximum size is reduced by...
	 *   1 byte start char
	 *   2 bytes size
	 *   1 byte sequence number
	 * --> Already user data:  1 byte packet type
	 *   2 bytes CRC
	 */
	if( sizPacketTxFill>=MONITOR_MAX_PACKET_SIZE_USB-6U )
	{
		transport_call_console_flush();
	}
}



unsigned int transport_call_console_peek(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
	return (ulFillLevel==0) ? 0U : 1U;
}



void transport_call_console_flush(void)
{
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);

	/* Start a new packet. */
	transport_send_byte(MONITOR_PACKET_TYP_CallMessage);
}
