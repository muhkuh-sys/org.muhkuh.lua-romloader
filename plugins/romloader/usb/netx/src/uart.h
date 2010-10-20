/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/


#ifndef __UART_H__
#define __UART_H__


typedef enum
{
	UART_BAUDRATE_300    =    3,
	UART_BAUDRATE_600    =    6,
	UART_BAUDRATE_1200   =   12,
	UART_BAUDRATE_2400   =   24,
	UART_BAUDRATE_4800   =   48,
	UART_BAUDRATE_9600   =   96,
	UART_BAUDRATE_19200  =  192,
	UART_BAUDRATE_38400  =  384,
	UART_BAUDRATE_57600  =  576,
	UART_BAUDRATE_115200 = 1152
} UART_BAUDRATE_T;

#define DEV_FREQUENCY 100000000UL

/* DEV_BAUDRATE is 100 times to small -> multiply with 100 (or divide by DEV_FREQUENCY/100) */
#define UART_BAUDRATE_DIV(a) ((a*16*65536)/(DEV_FREQUENCY/100))


typedef struct
{
	unsigned char uc_rx_mmio;
	unsigned char uc_tx_mmio;
	unsigned char uc_rts_mmio;
	unsigned char uc_cts_mmio;
	unsigned short us_baud_div;
} UART_CONFIGURATION_T;


int uart_init(unsigned int uiUartUnit, const UART_CONFIGURATION_T *ptCfg);
void uart_put(unsigned int uiUartUnit, unsigned char ucChar);
void uart_flush(unsigned int uiUartUnit);
unsigned int uart_get(unsigned int uiUartUnit);
unsigned int uart_peek(unsigned int uiUartUnit);
void uart_close(unsigned int uiUartUnit);


#endif	/* __UART_H__ */
