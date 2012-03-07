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

#include "uart.h"

#include "netx_io_areas.h"


#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))


typedef struct
{
	HOSTADEF(UART) * const ptArea;
#if ASIC_TYP==10 || ASIC_TYP==50
	MMIO_CFG_T tMmioRx;
	MMIO_CFG_T tMmioTx;
	MMIO_CFG_T tMmioRts;
	MMIO_CFG_T tMmioCts;
#elif ASIC_TYP==500 || ASIC_TYP==100
	unsigned int uiGpioRx;
	unsigned int uiGpioTx;
	unsigned int uiGpioRts;
	unsigned int uiGpioCts;
#endif
} UART_INSTANCE_T;


static const UART_INSTANCE_T tUartInstance =
{
#if ASIC_TYP==10
	(NX10_UART_AREA_T * const)Addr_NX10_uart0,
	MMIO_CFG_uart0_rxd,
	MMIO_CFG_uart0_txd,
	MMIO_CFG_uart0_rtsn,
	MMIO_CFG_uart0_ctsn

#elif ASIC_TYP==50
	(NX50_UART_AREA_T * const)Addr_NX50_uart0,
	MMIO_CFG_uart0_rxd,
	MMIO_CFG_uart0_txd,
	MMIO_CFG_uart0_rts,
	MMIO_CFG_uart0_cts

#elif ASIC_TYP==100
	(NX100_UART_AREA_T * const)Addr_NX100_uart0,
	0,
	1,
	2,
	3

#elif ASIC_TYP==500
	(NX500_UART_AREA_T * const)Addr_NX500_uart0,
	0,
	1,
	2,
	3

#endif
};


void uart_init(const UART_CONFIGURATION_T *ptCfg)
{
	unsigned long ulValue;
	HOSTADEF(UART) *ptUartArea;


	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	/* disable uart */
	ptUartArea->ulUartcr = 0;

	/* use baudrate mode 2 */
	ptUartArea->ulUartcr_2 = HOSTMSK(uartcr_2_Baud_Rate_Mode);

	/* set the baudrate */
	ulValue = ptCfg->us_baud_div;
	ptUartArea->ulUartlcr_l = ulValue & 0xffU;
	ptUartArea->ulUartlcr_m = ulValue >> 8;

	/* set uart to 8N1, fifo enabled */
	ulValue  = HOSTMSK(uartlcr_h_WLEN);
	ulValue |= HOSTMSK(uartlcr_h_FEN);
	ptUartArea->ulUartlcr_h = ulValue;

	/* disable all drivers */
	ptUartArea->ulUartdrvout = ulValue;

	/* disable rts/cts mode */
	ptUartArea->ulUartrts = 0;

	/* enable the uart */
	ptUartArea->ulUartcr = HOSTMSK(uartcr_uartEN);

#if ASIC_TYP==10 || ASIC_TYP==50
	/* setup the MMIO pins */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_rx_mmio] = tUartInstance.tMmioRx;

	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_tx_mmio] = tUartInstance.tMmioTx;
#elif ASIC_TYP==500 || ASIC_TYP==100
	ptGpioArea->aulGpio_cfg[tUartInstance.uiGpioRx] = 2;
	ptGpioArea->aulGpio_cfg[tUartInstance.uiGpioTx] = 2;
	ptGpioArea->aulGpio_cfg[tUartInstance.uiGpioRts] = 2;
	ptGpioArea->aulGpio_cfg[tUartInstance.uiGpioCts] = 2;
#endif
	/* enable the drivers */
	ulValue = HOSTMSK(uartdrvout_DRVTX);
	ptUartArea->ulUartdrvout = ulValue;
}


void uart_put(unsigned int uiChar)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;


	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	/* Wait until there is space in the FIFO */
	do
	{
		ulVal  = ptUartArea->ulUartfr;
		ulVal &= HOSTMSK(uartfr_TXFF);
	} while( ulVal!=0 );

	ptUartArea->ulUartdr = uiChar & 0xff;
}


void uart_flush(void)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;


	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	do
	{
		ulVal  = ptUartArea->ulUartfr;
		ulVal &= HOSTMSK(uartfr_BUSY);
	} while( ulVal!=0 );
}


unsigned char uart_get(void)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;
	unsigned int uiData;


	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	/* wait for data in the fifo */
	do
	{
		ulVal  = ptUartArea->ulUartfr;
		ulVal &= HOSTMSK(uartfr_RXFE);
	} while( ulVal!=0 );

	/* Get the received byte */
	uiData = (unsigned int)ptUartArea->ulUartdr;

	return (unsigned char)uiData;
}


unsigned int uart_peek(void)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned int uiIsNotEmpty;


	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	/* the uart does not provide an explicit fill level, so the 'RX Fifo */
	/* Empty' flag must do. */
	uiIsNotEmpty = ((ptUartArea->ulUartfr&HOSTMSK(uartfr_RXFE))==0) ? 1U : 0U;

	return uiIsNotEmpty;
}


void uart_close(void)
{
	HOSTADEF(UART) *ptUartArea;


	/* flush the buffer */
	uart_flush();

	/* get the uart area */
	ptUartArea = tUartInstance.ptArea;

	ptUartArea->ulUartcr = 0;
	ptUartArea->ulUartlcr_m = 0;
	ptUartArea->ulUartlcr_l = 0;
	ptUartArea->ulUartlcr_h = 0;
	ptUartArea->ulUartrts = 0;
	ptUartArea->ulUartdrvout = 0;
}
