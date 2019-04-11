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


#if ASIC_TYP==ASIC_TYP_NETX56
#       define USB_FIFO_DEPTH_JTag_TX 64
#else
#       error "Add the FIFO size for the ASIC_TYP here."
#endif


typedef enum ACK_RESULT_ENUM
{
	ACK_RESULT_SequenceAckd       =  0,
	ACK_RESULT_OperationCanceled  = -1,
	ACK_RESULT_Timeout            = -2
} ACK_RESULT_T;



typedef enum
{
	USB_FIFO_MODE_Stream       = 0,
	USB_FIFO_MODE_StreamZLP    = 1,
	USB_FIFO_MODE_Packet       = 2,
	USB_FIFO_MODE_Transaction  = 3
} USB_FIFO_MODE_T;

#define MONITOR_MAX_PACKET_SIZE_USB 2048

/* Resend a packet after 1000ms if no ACK was received. */
#define USB_RESEND_TIMEOUT_MS 1000

static unsigned char aucPacketRx[MONITOR_MAX_PACKET_SIZE_USB];
static unsigned char aucPacketTx[MONITOR_MAX_PACKET_SIZE_USB];

static unsigned int sizPacketTxFill;
static unsigned int sizPacketRxHead;
static unsigned int sizPacketRxFill;

static unsigned char ucSequence;



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



static void usb_reset_fifo(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;


	/* Reset all FIFOs. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = HOSTMSK(usb_dev_fifo_ctrl_conf_reset);

	/* Set the FIFO modes and release the reset. */
	ulValue  = USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_interrupt);
	ulValue |= USB_FIFO_MODE_StreamZLP   << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_uart_rx);
	ulValue |= USB_FIFO_MODE_StreamZLP   << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_uart_tx);
	ulValue |= USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_rx);
	ulValue |= USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_tx);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = ulValue;
}



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



static void usb_init(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	unsigned long ulValue;


	usb_reset_fifo();

	/* Set disconnect timeout to max. */
	ulValue  = HOSTMSK(usb_dev_cfg_usb_core_enable);
	ulValue |= 3 << HOSTSRT(usb_dev_cfg_disconn_timeout);
	ptUsbDevCtrlArea->ulUsb_dev_cfg = ulValue;

	/* Disable all IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;
	/* Acknowledge any pending IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = 0xffffffffU;

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
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned int uiCnt;
	unsigned short usCrc16;
	unsigned char aucACK[7];
	unsigned int sizFree;
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

	/* Set the packet length.  */
	ulValue  = sizeof(aucACK) << HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_len_transaction_len);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_len = ulValue;

	/* Wait for free bytes in the FIFO. */
	do
	{
		ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
		ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
		ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
		sizFree = (size_t)(USB_FIFO_DEPTH_JTag_TX-ulValue);
	} while( sizFree<sizeof(aucACK) );

	/* Write data to the FIFO. */
	uiCnt = 0;
	do
	{
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = aucACK[uiCnt];
		++uiCnt;
	} while( uiCnt<sizeof(aucACK) );

	/* Wait until the transaction is finished. */
	do
	{
		ulValue  = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
		ulValue &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_transaction_active);
	} while( ulValue!=0 );
}



void transport_loop(void)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned long ulReceivedData;
	unsigned char *pucCnt;
	unsigned char *pucEnd;
	unsigned long ulTransactionSize;
	unsigned int sizPacket;
	unsigned short usCrc16;
	unsigned int sizCrcPosition;
	unsigned char ucPacketSequenceNumber;


	/* Receive a new transaction. */
	ulReceivedData = 0;
	pucCnt = aucPacketRx;

	/* Expect the maximum transaction size. */
	ulTransactionSize = 0x2000;

	do
	{
		/* Get the RX input fill level. */
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_stat;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
		if( ulFillLevel>0 )
		{
			/* Get the new size of the transaction. */
			ulReceivedData += ulFillLevel;

			/* Is the updated size of the transaction still smaller than the buffer size? */
			if( ulReceivedData>MONITOR_MAX_PACKET_SIZE_USB )
			{
				/* No, the transaction overflows the buffer. */
				break;
			}
			else
			{
				/* Copy the complete packet to the buffer. */
				pucEnd = pucCnt + ulFillLevel;
				do
				{
					*(pucCnt++) = (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_jtag_rx_data);
				} while( pucCnt<pucEnd );
			}
		}

		/* Is the transaction finished? */
		ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
		ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
		ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
		if( ulValue!=0 )
		{
			ulTransactionSize = ulValue;
		}
	} while( ulReceivedData<ulTransactionSize );

	if( ulReceivedData<ulTransactionSize )
	{
		/* TODO: Discard the transaction. */
		while(1) {};
	}
	/* The minimum packet size is 5. */
	else if( ulTransactionSize>=5 )
	{
		sizPacketRxFill = ulTransactionSize;

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



static ACK_RESULT_T transport_wait_for_ack(unsigned char ucRequiredSequenceNumber, unsigned long ulTimeoutMs)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	ACK_RESULT_T tResult;
	unsigned long ulTimerHandle;
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned long ulReceivedData;
	unsigned long ulTransactionSize;
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
		/* Receive a new transaction. */
		ulReceivedData = 0;
		pucCnt = aucPacketRx;

		/* Expect the maximum transaction size. */
		ulTransactionSize = 0x2000;

		do
		{
			/* Get the RX input fill level. */
			ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_stat;
			ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
			ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
			if( ulFillLevel>0 )
			{
				/* Get the new size of the transaction. */
				ulReceivedData += ulFillLevel;

				/* Is the updated size of the transaction still smaller than the buffer size? */
				if( ulReceivedData>MONITOR_MAX_PACKET_SIZE_USB )
				{
					/* No, the transaction overflows the buffer. */
					break;
				}
				else
				{
					/* Copy the complete packet to the buffer. */
					pucEnd = pucCnt + ulFillLevel;
					do
					{
						*(pucCnt++) = (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_jtag_rx_data);
					} while( pucCnt<pucEnd );
				}
			}

			/* Is the transaction finished? */
			ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
			ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
			ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
			if( ulValue!=0 )
			{
				ulTransactionSize = ulValue;
			}
		} while( ulReceivedData<ulTransactionSize );

		if( ulReceivedData<ulTransactionSize )
		{
			/* Discard the transaction. */
			usb_reset_fifo();
		}
		else
		{
			/* Does the packet start with the stream start. */
			if( aucPacketRx[0]==MONITOR_STREAM_PACKET_START )
			{
				/* Get the packet size. */
				sizPacket  =  (unsigned int)(aucPacketRx[1]);
				sizPacket |= ((unsigned int)(aucPacketRx[2])) << 8U;

				/* Does the data fit into the received packet?
				 * The packet has these extra fields which are not part of the data:
				 *   1 byte stream start
				 *   2 bytes data size
				 *   2 bytes CRC16
				 */
				if( sizPacket+5U==ulReceivedData )
				{
					/* Loop over all bytes and build the CRC16 checksum. */
					/* NOTE: the size is just for the user data, but the CRC16 includes the size. */
					usCrc16 = 0;
					sizCrcPosition = 1U;
					while( sizCrcPosition<ulReceivedData )
					{
						usCrc16 = crc16(usCrc16, aucPacketRx[sizCrcPosition]);
						++sizCrcPosition;
					}
					if( usCrc16==0 )
					{
						/* Get the sequence number. */
						ucPacketSequenceNumber = aucPacketRx[3];

						/* Get the packet type. */
						tPacketTyp = (MONITOR_PACKET_TYP_T)aucPacketRx[4];

						/* Is this an ACK packet with the correct sequence number? */
						if( tPacketTyp==MONITOR_PACKET_TYP_ACK && ucPacketSequenceNumber==ucRequiredSequenceNumber )
						{
							tResult = ACK_RESULT_SequenceAckd;
							break;
						}
						else if( tPacketTyp==MONITOR_PACKET_TYP_CancelOperation )
						{
							tResult = ACK_RESULT_OperationCanceled;
							break;
						}
					}
				}
			}
		}

		iIsElapsed = systime_elapsed(ulTimerHandle, ulTimeoutMs);
	} while( iIsElapsed==0 );

	return tResult;
}



int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned long ulValue;
	unsigned int sizLeft;
	unsigned int sizChunk;
	ACK_RESULT_T tResult;
	int iResult;
	unsigned short usCrc16;
	unsigned char ucData;
	unsigned int sizPacketData;


	do
	{
		/* Set the packet length. Add 6 bytes for...
		 *   1 byte stream start
		 *   2 bytes data size
		 *   1 byte sequence number
		 *   2 bytes CRC
		 */
		ulValue  = (sizPacketTxFill+6U) << HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_len_transaction_len);
		ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_len = ulValue;

		/* Send the stream start. */
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = MONITOR_STREAM_PACKET_START;
		/* Send the data size. */
		sizPacketData = sizPacketTxFill + 1U;
		ucData =  sizPacketData        & 0xffU;
		usCrc16 = crc16(0, ucData);
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = ucData;
		ucData = (sizPacketData >> 8U) & 0xffU;
		usCrc16 = crc16(usCrc16, ucData);
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = ucData;
		/* Send the sequence byte. */
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = ucSequence;
		usCrc16 = crc16(usCrc16, ucSequence);

		sizLeft = sizPacketTxFill;
		pucCnt = aucPacketTx;
		while( sizLeft!=0 )
		{
			/* Wait for free bytes in the FIFO. */
			do
			{
				ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
				ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
				ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
				sizChunk = (size_t)(USB_FIFO_DEPTH_JTag_TX-ulValue);
			} while( sizChunk==0 );

			if( sizChunk>sizLeft )
			{
				sizChunk = sizLeft;
			}

			/* Write data to the FIFO. */
			pucEnd = pucCnt + sizChunk;
			do
			{
				ucData = *(pucCnt++);
				usCrc16 = crc16(usCrc16, ucData);
				ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = ucData;
			} while( pucCnt<pucEnd );

			sizLeft -= sizChunk;
		}

		/* Wait for 2 free bytes in the FIFO. */
		do
		{
			ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
			ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
			ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
			sizChunk = (size_t)(USB_FIFO_DEPTH_JTag_TX-ulValue);
		} while( sizChunk<2 );

		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = (unsigned char)(usCrc16 >> 8U);
		ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = (unsigned char)(usCrc16 & 0xff);

		/* Wait until the transaction is finished. */
		do
		{
			ulValue  = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
			ulValue &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_transaction_active);
		} while( ulValue!=0 );

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
	HOSTDEF(ptUsbDevFifoArea);
	unsigned long ulFillLevel;
	unsigned char ucData;


	/* Wait for a byte in the FIFO. */
	do
	{
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
	} while( ulFillLevel==0 );

	/* Get a byte from the FIFO. */
	ucData = (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_jtag_rx_data);
	return ucData;
}



void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the buffer. */
	if( sizPacketTxFill<MONITOR_MAX_PACKET_SIZE_USB )
	{
		aucPacketTx[sizPacketTxFill] = (unsigned char)uiChar;
		++sizPacketTxFill;
	}

	/* Reached the maximum packet size? */
	if( sizPacketTxFill>=MONITOR_MAX_PACKET_SIZE_USB )
	{
		transport_call_console_flush();
	}
}



unsigned int transport_call_console_peek(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
	return (ulFillLevel==0) ? 0U : 1U;
}



void transport_call_console_flush(void)
{
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);

	/* Start a new packet. */
	transport_send_byte(MONITOR_PACKET_TYP_CallMessage);
}

