
	.section .init_code, "ax"
	.arm

	.extern netx_consoleapp_init

	.global NetxTestParameter_Init
	.global start

@--------------------------------------

  .func start_init_s

start:
	stmfd   sp!, {r0, r1, r2, r3, r4, lr}

#if ASIC_TYP==10
	@ copy the data section
	ldr     r1, =__data_loadaddr__
	ldr     r2, =__data_start__
	ldr     r3, =__data_end__
copy_data:
	cmp     r2, r3
	ldrlo   r4, [r1], #4
	strlo   r4, [r2], #4
	blo     copy_data
#endif

	@ clear .bss section (zero init)
	mov     r1, #0
	ldr     r2, =__bss_start__
	ldr     r3, =__bss_end__
clear_bss:
	cmp     r2, r3
	strlo   r1, [r2], #4
	blo     clear_bss

	bl      netx_consoleapp_init

	ldmfd   sp!, {r0, r1, r2, r3, r4, pc}

  .endfunc

@--------------------------------------

  .end

