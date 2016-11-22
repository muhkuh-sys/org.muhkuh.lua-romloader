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


#include "netx_io_areas.S"

	.section .init_code, "ax"
	.arm

	.extern bootstrap

	.global start
	.global stack_top

@--------------------------------------

	.equ SR_Mode_SVC,       0x13

	.equ SR_Irq,            0x80
	.equ SR_Fiq,            0x40

@--------------------------------------

  .func start_init_s

start:
	mov     r0,     #0

#if ASIC_TYP==ASIC_TYP_NETX10
	@ netX10 has the TCMs enabled. Code, data and XC buffer must not
	@ share the same INTRAM segment. This is difficult to handle.
	@
	@ Disable the TCMs here.
	@

	@ Clear data pipe.
	mcr     p15,    0, r0, c7, c10, 4

	@ Set the ctrl register to ...
	@ TBIT enable (#15 = 0)
	@ disable ITCM (#12 = 0)
	@ little endianess (#7 = 0)
	@ enable buffered writes in bufferable areas (#3 = 1)
	@ disable DTCM (#2 = 0)
	@ disable alignment check (#1 = 0)
	ldr     r0,     =0x0f78
	mcr     p15,    0, r0, c1, c0, 0
#elif ASIC_TYP==ASIC_TYP_NETX50
	@ SDRAM fix
	@ Setup the DMA controller to stop the fake transfer at 0x800DEAD0.
	ldr     r0,     =HOSTADDR(dmac_ch0)
	ldr     r1,     =HOSTADDR(dmac_reg)

	ldr     r2,     =0x00000001
	str     r2,     [r1, #0x30]    /* Enable DMA controller generally */

	ldr     r2,     =0xBFFFFFFC
	str     r2,     [r0, #0x04]    /* DMA Target address = physical end of SDRAM area */

	ldr     r2,     =0x00480001
	str     r2,     [r0, #0x0C]    /* Setup DMA transfer (memory cycle) */

	ldr     r2,     =0x00000001
	str     r2,     [r0, #0x10]    /* Start DMA transfer*/

	ldr     r2,     =0x00000000
	str     r2,     [r0, #0x0C]    /* Stop and disable DMA */
	str     r2,     [r0, #0x10]
	str     r2,     [r1, #0x30]    /* Disable DMA controller */
#else
#	error "Unknown ASIC_TYP"
#endif

	blx     bootstrap

  .endfunc

@--------------------------------------

  .end

