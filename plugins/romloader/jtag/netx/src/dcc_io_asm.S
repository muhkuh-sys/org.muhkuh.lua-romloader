#include "asic_types.h" 
	.extern dcc_buffer_putchar
	.extern dcc_buffer_flush
	.global dbg_write
	.section .dcc_out_s, "ax"
	
#if ASIC_TYP==ASIC_TYP_NETX4000_RELAXED 
	.arm
	b dcc_buffer_putchar
	b dcc_buffer_flush
	
	.word ab_buffer     
	.word buffer_end    
	.word pch_buffer_ptr
	
dbg_write:
.L3:
	mrc     p14, 0, r1, c0, c1, 0
	tst     r1, #0x20000000
	bne     .L3
	mcr     p14, 0, r0, c0, c5, 0
	bx      lr

#elif ASIC_TYP==ASIC_TYP_NETX90_MPW
	.thumb
	b dcc_buffer_putchar
	b dcc_buffer_flush
	
	.word ab_buffer     
	.word buffer_end    
	.word pch_buffer_ptr
	
dbg_write:
	@ todo
	bx      lr

#else
	.arm
	b dcc_buffer_putchar
	b dcc_buffer_flush
	
	.word ab_buffer     
	.word buffer_end    
	.word pch_buffer_ptr
	
dbg_write:
.L3:
	mrc     p14, 0, r3, c0, c0
	tst     r3, #2
	bne     .L3
	mcr     p14, 0, r0, c1, c0
	bx      lr
#endif
	.end
 