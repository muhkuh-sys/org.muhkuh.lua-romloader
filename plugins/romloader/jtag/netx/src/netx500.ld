MEMORY
{
	ASSERT_EMPTY(rwx): ORIGIN = 0x00000000, LENGTH = 0
	INTRAM0_MON(rwx):  ORIGIN = 0x00000400, LENGTH = 0x00003f00
}

ab_buffer      = 0x10001e00;
buffer_end     = 0x10001f00;
pch_buffer_ptr = 0x10001fe0;
serial_vectors = 0x10001ff0;

INCLUDE plugins\romloader\jtag\netx\src\netx.ld