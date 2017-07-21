
	.section .init_code, "ax"
#include "asic_types.h"

@--------------------------------------

@ The Cortex-M4 in the netX90 is a THUMB-only CPU.
@ All others start in 32 bit mode.
#if ASIC_TYP==ASIC_TYP_NETX90_MPW
	.syntax unified
	.thumb
#else
	.arm
#endif

  
	.extern netx_consoleapp_init
	.global start

  
@--------------------------------------

  .func start_init_s

start:
	stmfd   sp!, {r0, r1, r2, r3, lr}

	@ clear .bss section (zero init)
	mov     r1, #0
	ldr     r2, =__bss_start__
	ldr     r3, =__bss_end__
clear_bss:
	cmp     r2, r3
	itt lo
	strlo   r1, [r2], #4
	blo     clear_bss

	blx     netx_consoleapp_init

	ldmfd   sp!, {r0, r1, r2, r3, lr}
	bx      lr

  .endfunc

@--------------------------------------

  .end

