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

#include "uart.h"

#include "netx_io_areas.h"

#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))


#if ASIC_TYP==10 || ASIC_TYP==50 || ASIC_TYP==56
typedef struct
{
	MMIO_CFG_T tMmioRx;
	MMIO_CFG_T tMmioTx;
	MMIO_CFG_T tMmioRts;
	MMIO_CFG_T tMmioCts;
} UART_INSTANCE_T;
#endif


#if ASIC_TYP==10
static const UART_INSTANCE_T tUartInstance =
{
	MMIO_CFG_uart0_rxd,
	MMIO_CFG_uart0_txd,
	MMIO_CFG_uart0_rtsn,
	MMIO_CFG_uart0_ctsn
};
#elif ASIC_TYP==50
static const UART_INSTANCE_T tUartInstance =
{
	MMIO_CFG_uart0_rxd,
	MMIO_CFG_uart0_txd,
	MMIO_CFG_uart0_rts,
	MMIO_CFG_uart0_cts
};
#elif ASIC_TYP==56
static const UART_INSTANCE_T tUartInstance =
{
	MMIO_CFG_uart0_rxd,
	MMIO_CFG_uart0_txd,
	MMIO_CFG_uart0_rtsn,
	MMIO_CFG_uart0_ctsn
};
#endif


void uart_init(const UART_CONFIGURATION_T *ptCfg)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulValue;
#if ASIC_TYP==10 || ASIC_TYP==50 || ASIC_TYP==56
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);
#elif ASIC_TYP==100 || ASIC_TYP==500
	HOSTDEF(ptGpioArea);
#endif


	/* Disable the UART. */
	ptUart0Area->ulUartcr = 0;

	/* Use baudrate mode 2. */
	ptUart0Area->ulUartcr_2 = HOSTMSK(uartcr_2_Baud_Rate_Mode);

	/* set the baudrate */
	ulValue = ptCfg->us_baud_div;
	ptUart0Area->ulUartlcr_l = ulValue & 0xffU;
	ptUart0Area->ulUartlcr_m = ulValue >> 8;

	/* set uart to 8N1, fifo enabled */
	ulValue  = HOSTMSK(uartlcr_h_WLEN);
	ulValue |= HOSTMSK(uartlcr_h_FEN);
	ptUart0Area->ulUartlcr_h = ulValue;

	/* disable all drivers */
	ptUart0Area->ulUartdrvout = ulValue;

	/* disable rts/cts mode */
	ptUart0Area->ulUartrts = 0;

	/* enable the uart */
	ptUart0Area->ulUartcr = HOSTMSK(uartcr_uartEN);

#if ASIC_TYP==10 || ASIC_TYP==50 || ASIC_TYP==56
	/* setup the MMIO pins */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_rx_mmio] = tUartInstance.tMmioRx;
#endif

	/* enable the drivers */
	ulValue = HOSTMSK(uartdrvout_DRVTX);
	ptUart0Area->ulUartdrvout = ulValue;

#if ASIC_TYP==10 || ASIC_TYP==50 || ASIC_TYP==56
	/* setup the MMIO pins */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_tx_mmio] = tUartInstance.tMmioTx;
#elif ASIC_TYP==100 || ASIC_TYP==500
	ptGpioArea->aulGpio_cfg[0] = 2;
	ptGpioArea->aulGpio_cfg[1] = 2;
	ptGpioArea->aulGpio_cfg[2] = 2;
	ptGpioArea->aulGpio_cfg[3] = 2;
#endif
}



void uart_put(unsigned char ucChar)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulVal;


	/* Wait until there is space in the FIFO */
	do
	{
		ulVal  = ptUart0Area->ulUartfr;
		ulVal &= HOSTMSK(uartfr_TXFF);
	} while( ulVal!=0 );

	ptUart0Area->ulUartdr = ucChar;
}



void uart_flush(void)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulVal;


	do
	{
		ulVal  = ptUart0Area->ulUartfr;
		ulVal &= HOSTMSK(uartfr_BUSY);
	} while( ulVal!=0 );
}



unsigned char uart_get(void)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulVal;
	unsigned char ucData;


	/* Wait for data in the FIFO. */
	do
	{
		ulVal  = ptUart0Area->ulUartfr;
		ulVal &= HOSTMSK(uartfr_RXFE);
	} while( ulVal!=0 );

	/* Get the received byte. */
	ucData = (unsigned char)ptUart0Area->ulUartdr;

	return ucData;
}



unsigned int uart_peek(void)
{
	HOSTDEF(ptUart0Area);
	unsigned int uiIsNotEmpty;


	/* The UART does not provide an explicit fill level, so the 'RX FIFO. */
	/* Empty' flag must do. */
	uiIsNotEmpty = ((ptUart0Area->ulUartfr&HOSTMSK(uartfr_RXFE))==0) ? 1U : 0U;

	return uiIsNotEmpty;
}



void uart_close(void)
{
	HOSTDEF(ptUart0Area);
#if ASIC_TYP==100 || ASIC_TYP==500
	HOSTDEF(ptGpioArea)
#endif


	/* Flush the buffer. */
	uart_flush();

#if ASIC_TYP==100 || ASIC_TYP==500
	ptGpioArea->aulGpio_cfg[0] = 2;
	ptGpioArea->aulGpio_cfg[1] = 2;
	ptGpioArea->aulGpio_cfg[2] = 2;
	ptGpioArea->aulGpio_cfg[3] = 2;
#endif

	ptUart0Area->ulUartcr = 0;
	ptUart0Area->ulUartlcr_m = 0;
	ptUart0Area->ulUartlcr_l = 0;
	ptUart0Area->ulUartlcr_h = 0;
	ptUart0Area->ulUartrts = 0;
	ptUart0Area->ulUartdrvout = 0;
}

