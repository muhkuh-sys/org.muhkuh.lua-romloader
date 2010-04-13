
	.section .init_code, "ax"
	.arm

	.extern test_main

	.global NetxTestParameter_Init
	.global start
	.global stack_top

#if ASIC_TYP==500 || ASIC_TYP==100
	.equ Adr_dTCM_base,			0x10000000
#endif

@--------------------------------------

  .func start_init_s

start:
	stmfd	sp!, {r0, r1, r2, lr}

	@ save pointer to netx parameter block
	ldr	r1, =NetxTestParameter_Init
	str	r0, [r1]

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
/*
	mov	r0, #0
	mov	r1, #65
	ldr	r2, =0x0400fff4
	blx	r2

	mov	r0, #0
	mov	r1, #66
	ldr	r2, =0x0400fff4
	blx	r2

	mov	r0, #0
	mov	r1, #67
	ldr	r2, =0x0400fff4
	blx	r2

	mov	r0, #0
	mov	r1, #68
	ldr	r2, =0x0400fff4
	blx	r2

	mov	r0, #0
	ldr	r2, =0x0400fffc
	blx	r2
*/
	@ restore debug console stack
	ldr	r1, =blstack_save
	ldr	r0, [r1]
	mov	sp, r0

	ldmfd	sp!, {r0, r1, r2, lr}
	bx	lr

call_test_main:
	ldr	r0, =test_main
	bx	r0

  .endfunc

@--------------------------------------

	.align 4

NetxTestParameter_Init:
	.word 0

blstack_save:
	.word 0

@--------------------------------------

  .end

