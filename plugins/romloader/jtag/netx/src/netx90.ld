MEMORY
{
	ASSERT_EMPTY(rwx): ORIGIN = 0x00000000, LENGTH = 0
	INTRAM0_MON(rwx) : ORIGIN = 0x00020400, LENGTH = 0x00000400 /* INTRAM0 */
}


ab_buffer      = 0x00020e00;
buffer_end     = 0x00020f00;
pch_buffer_ptr = 0x00020fe0;


INCLUDE plugins/romloader/jtag/netx/src/netx.ld
