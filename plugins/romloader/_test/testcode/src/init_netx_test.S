
	.section .init_code, "ax"
	.arm

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
	strlo   r1, [r2], #4
	blo     clear_bss

	bl      netx_consoleapp_init

	ldmfd   sp!, {r0, r1, r2, r3, lr}
	bx      lr

  .endfunc

@--------------------------------------

  .end

