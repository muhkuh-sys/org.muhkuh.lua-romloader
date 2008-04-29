/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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

	.extern netx_consoleapp_init

	.global start

@--------------------------------------

    .func start

start:
	@ save registers on debug console stack
	stmfd	sp!, {r0, r1, r2, r3, lr}

	@ clear .bss section with 0
	mov	r1, #0
	ldr	r2, =__bss_start__
	ldr	r3, =__bss_end__
LoopZI:
	cmp	r2, r3
	strlo	r1, [r2], #4
	blo	LoopZI

	@ call test main routine
	bl	netx_consoleapp_init

	@ restore registers from debug console stack and return
	ldmfd	sp!, {r0, r1, r2, r3, lr}
	bx	lr

    .endfunc

@--------------------------------------

    .end

