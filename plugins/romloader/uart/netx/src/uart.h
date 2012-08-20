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


void uart_init(const UART_CONFIGURATION_T *ptCfg);
void uart_put(unsigned char ucChar);
void uart_flush(void);
unsigned char uart_get(void);
unsigned int uart_peek(void);
void uart_close(void);


#endif	/* __UART_H__ */
