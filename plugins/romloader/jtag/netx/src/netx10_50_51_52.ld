MEMORY
{
	ASSERT_EMPTY(rwx): ORIGIN = 0x00000000, LENGTH = 0
	INTRAM0_MON(rwx) : ORIGIN = 0x00000400, LENGTH = 0x00001000
}

ab_buffer      = 0x04000e00;
buffer_end     = 0x04000f00;
pch_buffer_ptr = 0x04000fe0;

INCLUDE plugins/romloader/jtag/netx/src/netx.ld
