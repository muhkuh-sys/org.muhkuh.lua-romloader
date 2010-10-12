/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


	.section .init_code, "ax"
	.arm

	.extern usb_monitor

	.global start
	.global stack_top

@--------------------------------------

	.equ SR_Mode_SVC,       0x13

	.equ SR_Irq,            0x80
	.equ SR_Fiq,            0x40

@--------------------------------------

  .func start_init_s

start:
	@ Enable SVC mode and disable IRQ and FIQ.
	msr     cpsr_c, #SR_Irq|SR_Fiq|SR_Mode_SVC

	@ save the working register.
	stmfd   sp!,    {r4}

	mov     r4,     #0

#if ASIC_TYP==50 || ASIC_TYP==10
	@
	@ This is the arm966e-j init.
	@

	@ Clear data pipe.
	mcr     p15,    0, r4, c7, c10, 4

	@ Set the ctrl register to ...
	@ TBIT enable (#15 = 0)
	@ disable ITCM (#12 = 0)
	@ little endianess (#7 = 0)
	@ enable buffered writes in bufferable areas (#3 = 1)
	@ disable DTCM (#2 = 0)
	@ disable alignment check (#1 = 0)
	ldr     r4,     =0x0f78
	mcr     p15,    0, r4, c1, c0, 0
#elif ASIC_TYP==500 || ASIC_TYP==100
	@
	@ This is the arm926ej-s init.
	@

	@ Invalidate instruction and data caches.
	mcr     p15,    0, r4, c7, c7, 0
	@ Clear data pipe.
	mcr     p15,    0, r4, c7, c10, 4
	@ Clear instruction and data translation buffers.
	mcr     p15,    0, r4, c8, c7, 0

	@ Get the ctrl register.
	mrc     p15,    0, r4, c1, c0, 0
	@ Disable mmu, alignment check and data cache.
	bic     r4,     r4, #0x0007
	@ Disable system protection and instruction cache.
	bic     r4,     r4, #0x1100
	@ Write back ctrl register.
	mcr     p15,    0, r4, c1, c0, 0
#else
#	error "Unknown ASIC_TYP"
#endif

	@ Save all parameters.
	ldr     r4, =NetxTestParameter_Init
	str     r0, [r4, #0x0]
	str     r1, [r4, #0x4]
	str     r2, [r4, #0x8]
	str     r3, [r4, #0xc]

	@ Remember romcode stack position.
	mov     r1, sp
	@ Get working register.
	ldmfd   sp!, {r2}

	@ Set own stack position.
	ldr     r0, =stack_top
	mov     sp, r0

	@ Save the romcode stack position, the working register and the
	@ return address.
	stmfd   sp!, {r1, r2, lr}

#if ASIC_TYP==10
	@ Copy the data section.
	ldr     r0, =__copy_src_start__
	ldr     r1, =__copy_dst_start__
	ldr     r2, =__copy_dst_end__
copy_data:
	cmp     r1, r2
	ldrlo   r3, [r0], #4
	strlo   r3, [r1], #4
	blo     copy_data
#endif

	@ Clear .bss section (zero init).
	mov     r0, #0
	ldr     r1, =__bss_start__
	ldr     r2, =__bss_end__
clear_bss:
	cmp     r1, r2
	strlo   r0, [r1], #4
	blo     clear_bss

	ldr     r4, =NetxTestParameter_Init
	ldr     r0, [r4, #0x0]
	ldr     r1, [r4, #0x4]
	ldr     r2, [r4, #0x8]
	ldr     r3, [r4, #0xc]
	bl      usb_monitor

  .endfunc

@--------------------------------------

	.align 4

NetxTestParameter_Init:
	.word 0
	.word 0
	.word 0
	.word 0

@--------------------------------------

  .end

