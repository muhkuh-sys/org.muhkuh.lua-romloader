MEMORY
{
	ASSERT_EMPTY(rwx): ORIGIN = 0x00000000, LENGTH = 0
	INTRAM0_MON(rwx) : ORIGIN = 0x04000400, LENGTH = 0x00001000 /* LLRAM */
}

ab_buffer      = 0x0003fe00;
buffer_end     = 0x0003ff00;
pch_buffer_ptr = 0x0003ffe0; 
serial_vectors = 0x0003fff0;

INCLUDE plugins/romloader/jtag/netx/src/netx.ld
