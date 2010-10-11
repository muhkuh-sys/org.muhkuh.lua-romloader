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

  .func start_init_s

start:
	stmfd   sp!, {r0, r1, r2, r3, r4, lr}

	@ Save all parameters.
	ldr     r4, =NetxTestParameter_Init
	str     r0, [r4, #0x0]
	str     r1, [r4, #0x4]
	str     r2, [r4, #0x8]
	str     r3, [r4, #0xc]

	@ save debug console stack position
	mov	r0, sp
	ldr	r1, =blstack_save
	str	r0, [r1]

	@ set own stack position
	ldr	r0, =stack_top
	mov	sp, r0

#if ASIC_TYP==10
	@ copy the data section
	ldr	r0, =__data_loadaddr__
	ldr	r1, =__data_start__
	ldr	r2, =__data_end__
copy_data:
	cmp	r1, r2
	ldrlo	r3, [r0], #4
	strlo	r3, [r1], #4
	blo	copy_data
#endif

	@ clear .bss section (zero init)
	mov	r0, #0
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
clear_bss:
	cmp	r1, r2
	strlo	r0, [r1], #4
	blo	clear_bss

	bl	call_test_main

	@ restore debug console stack
	ldr	r1, =blstack_save
	ldr	r0, [r1]
	mov	sp, r0

	ldmfd	sp!, {r0, r1, r2, r3, r4, lr}
	bx	lr

call_test_main:
	ldr     r4, =NetxTestParameter_Init
	ldr     r0, [r4, #0x0]
	ldr     r1, [r4, #0x4]
	ldr     r2, [r4, #0x8]
	ldr     r3, [r4, #0xc]
	ldr     r4, =usb_monitor
	bx	r4

  .endfunc

@--------------------------------------

	.align 4

NetxTestParameter_Init:
	.word 0
	.word 0
	.word 0
	.word 0

blstack_save:
	.word 0

@--------------------------------------

  .end

