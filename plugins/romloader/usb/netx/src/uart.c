/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2009, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

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
#endif
} UART_INSTANCE_T;


static const UART_INSTANCE_T atUartInstances[] =
{
#if ASIC_TYP==10
	{
		(NX10_UART_AREA_T * const)Addr_NX10_uart0,
		MMIO_CFG_uart0_rxd,
		MMIO_CFG_uart0_txd,
		MMIO_CFG_uart0_rtsn,
		MMIO_CFG_uart0_ctsn
	},

	{
		(NX10_UART_AREA_T * const)Addr_NX10_uart1,
		MMIO_CFG_uart1_rxd,
		MMIO_CFG_uart1_txd,
		MMIO_CFG_uart1_rtsn,
		MMIO_CFG_uart1_ctsn
	}
#elif ASIC_TYP==50
	{
		(NX50_UART_AREA_T * const)Addr_NX50_uart0,
		MMIO_CFG_uart0_rxd,
		MMIO_CFG_uart0_txd,
		MMIO_CFG_uart0_rts,
		MMIO_CFG_uart0_cts
	},

	{
		(NX50_UART_AREA_T * const)Addr_NX50_uart1,
		MMIO_CFG_uart1_rxd,
		MMIO_CFG_uart1_txd,
		MMIO_CFG_uart1_rts,
		MMIO_CFG_uart1_cts
	},

	{
		(NX50_UART_AREA_T * const)Addr_NX50_uart2,
		MMIO_CFG_uart2_rxd,
		MMIO_CFG_uart2_txd,
		MMIO_CFG_uart2_rts,
		MMIO_CFG_uart2_cts
	}
#elif ASIC_TYP==500 || ASIC_TYP==100
	{
		(NX500_UART_AREA_T * const)Addr_NX500_uart0
	},

	{
		(NX500_UART_AREA_T * const)Addr_NX500_uart1
	},

	{
		(NX500_UART_AREA_T * const)Addr_NX500_uart2
	}
#endif
};


int uart_init(unsigned int uiUartUnit, const UART_CONFIGURATION_T *ptCfg)
{
	unsigned long ulValue;
	HOSTADEF(UART) *ptUartArea;
	int iResult;
#if ASIC_TYP==100 || ASIC_TYP==500
	unsigned int uiIdx;
#endif


	/* expect error */
	iResult = -1;

	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

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
		ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_rx_mmio] = atUartInstances[uiUartUnit].tMmioRx;
#endif

		/* enable the drivers */
		ulValue = HOSTMSK(uartdrvout_DRVTX);
		ptUartArea->ulUartdrvout = ulValue;

#if ASIC_TYP==10 || ASIC_TYP==50
		/* setup the MMIO pins */
		ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
		ptMmioCtrlArea->aulMmio_cfg[ptCfg->uc_tx_mmio] = atUartInstances[uiUartUnit].tMmioTx;
#elif ASIC_TYP==100 || ASIC_TYP==500
		uiIdx = uiUartUnit << 2;
		ptGpioArea->aulGpio_cfg[uiIdx+0] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+1] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+2] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+3] = 2;
#endif

		iResult = 0;
	}

	return iResult;
}


void uart_put(unsigned int uiUartUnit, unsigned int uiChar)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;


	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

		/* Wait until there is space in the FIFO */
		do
		{
			ulVal  = ptUartArea->ulUartfr;
			ulVal &= HOSTMSK(uartfr_TXFF);
		} while( ulVal!=0 );

		ptUartArea->ulUartdr = uiChar;
	}
}


void uart_flush(unsigned int uiUartUnit)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;


	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

		do
		{
			ulVal  = ptUartArea->ulUartfr;
			ulVal &= HOSTMSK(uartfr_BUSY);
		} while( ulVal!=0 );
	}
}


unsigned char uart_get(unsigned int uiUartUnit)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned long ulVal;
	unsigned char ucData;


	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

		/* wait for data in the fifo */
		do
		{
			ulVal  = ptUartArea->ulUartfr;
			ulVal &= HOSTMSK(uartfr_RXFE);
		} while( ulVal!=0 );

		/* Get the received byte */
		ucData = (unsigned char)ptUartArea->ulUartdr;
	}
	else
	{
		ucData = 0;
	}

	return ucData;
}


unsigned int uart_peek(unsigned int uiUartUnit)
{
	HOSTADEF(UART) *ptUartArea;
	unsigned int uiIsNotEmpty;


	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

		/* the uart does not provide an explicit fill level, so the 'RX Fifo */
		/* Empty' flag must do. */
		uiIsNotEmpty = ((ptUartArea->ulUartfr&HOSTMSK(uartfr_RXFE))==0) ? 1U : 0U;
	}
	else
	{
		uiIsNotEmpty = 0;
	}

	return uiIsNotEmpty;
}


void uart_close(unsigned int uiUartUnit)
{
	HOSTADEF(UART) *ptUartArea;
#if ASIC_TYP==100 || ASIC_TYP==500
	unsigned int uiIdx;
#endif


	if( uiUartUnit<ARRAYSIZE(atUartInstances) )
	{
		/* flush the buffer */
		uart_flush(uiUartUnit);

#if ASIC_TYP==100 || ASIC_TYP==500
		uiIdx = uiUartUnit << 2;
		ptGpioArea->aulGpio_cfg[uiIdx+0] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+1] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+2] = 2;
		ptGpioArea->aulGpio_cfg[uiIdx+3] = 2;
#endif

		/* get the uart area */
		ptUartArea = atUartInstances[uiUartUnit].ptArea;

		ptUartArea->ulUartcr = 0;
		ptUartArea->ulUartlcr_m = 0;
		ptUartArea->ulUartlcr_l = 0;
		ptUartArea->ulUartlcr_h = 0;
		ptUartArea->ulUartrts = 0;
		ptUartArea->ulUartdrvout = 0;
	}
}

