
	.arm
	.extern dcc_buffer_putchar
	.extern dcc_buffer_flush
	.global dbg_write

	.section .dcc_out_s, "ax"

	b dcc_buffer_putchar
	b dcc_buffer_flush
	
dbg_write:
.L3:
	mrc p14, 0, r3, c0, c0
	tst	r3, #2
	bne	.L3
	mcr p14, 0, r0, c1, c0
	bx	lr

	.end
 