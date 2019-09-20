#include "asic_types.h" 
	.extern dcc_buffer_putchar
	.extern dcc_buffer_flush
	.section .dcc_out_s, "ax"


#if ASIC_TYP==ASIC_TYP_NETX90_MPW || ASIC_TYP==ASIC_TYP_NETX90
	.thumb
	.syntax unified
#else
	.arm
	.global dbg_write
#endif

	b dcc_buffer_putchar
	b dcc_buffer_flush
	
	.word ab_buffer     
	.word buffer_end    
	.word pch_buffer_ptr


	
#if ASIC_TYP==ASIC_TYP_NETX4000_RELAXED || ASIC_TYP==ASIC_TYP_NETX4000
dbg_write:
.L3:
	mrc     p14, 0, r1, c0, c1, 0
	tst     r1, #0x20000000
	bne     .L3
	mcr     p14, 0, r0, c0, c5, 0
	bx      lr

#elif ASIC_TYP==ASIC_TYP_NETX90_MPW  || ASIC_TYP==ASIC_TYP_NETX90

/*
@ r0: data to send (32 bit)
@ r1: work register
@ r2: address of NVIC_DBG_DATA_R (0xe000edf8)
@ r3: loop counter (4->0)

dbg_write_m4:
	ldr     r2, =0xe000edf8 
	mov     r3, #4 

	@ wait until NVIC_DBG_DATA_R bit 0 (busy) is 0 
.L3:
	ldrh    r1, [r2] 
	tst		r1, #1
	bne     .L3

	@ put next 8 data bits into NVIC_DBG_DATA_R
	mov     r1, r0, LSL#8
	and     r1, #0xff00
	orr     r1, #1
	strh    r1, [r2]

	lsr     r0, #8
	subs    r3, #1
    bne     .L3

	bx      lr
	nop
*/

#else
dbg_write:
.L3:
	mrc     p14, 0, r3, c0, c0
	tst     r3, #2
	bne     .L3
	mcr     p14, 0, r0, c1, c0
	bx      lr
#endif
	.end
 
