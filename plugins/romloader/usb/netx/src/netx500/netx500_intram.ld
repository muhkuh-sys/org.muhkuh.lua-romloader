/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2010, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

ENTRY(start)

MEMORY
{
	ASSERT_EMPTY(rwx) :	ORIGIN = 0x00000000, LENGTH = 0

	INTRAM0(rwx) :		ORIGIN = 0x00008000, LENGTH = 0x00008000
	INTRAM1(rwx) :		ORIGIN = 0x00010000, LENGTH = 0x00008000
	DTCM(rw) :		ORIGIN = 0x10000000, LENGTH = 0x00002000
}


/* Section Definitions */
SECTIONS
{
	/* These sections must be empty, put them in a memory with a size of
	 * 0 bytes
	 */
	.assert_empty (NOLOAD) :
	{
		/* NOTE: include all sections here which should be empty.
		 */
		*(.data)
		*(.glue*)
		*(.vfp11_veneer)
		*(.v4_bx)
	} >ASSERT_EMPTY

	.code_xip ORIGIN(INTRAM0)+0x40 :
	{
		*(.init_code)
	} >INTRAM0

	.code_ram ORIGIN(INTRAM1) : AT(ORIGIN(INTRAM0)+0x40+SIZEOF(.code_xip))
	{
		__copy_start__ = . ;

		*(.text)

		__copy_end__ = . ;
	} >INTRAM1
	__copy_loadaddr__ = LOADADDR(.code_ram) ;


	.rodata ORIGIN(INTRAM0)+0x40+SIZEOF(.code_xip)+SIZEOF(.code_ram) :
	{
		*(.rodata*)
	} >INTRAM0


	.serialvectors ORIGIN(DTCM)+LENGTH(DTCM)-0x10 :
	{
		tSerialVectors = . ;
		. = . + 0x10;
	} >DTCM


	.bss ORIGIN(INTRAM1)+SIZEOF(.code_ram) : AT(ORIGIN(INTRAM1)+SIZEOF(.code_ram))
	{
		/* zero-initialized data */

		__bss_start__ = . ;

		*(.bss)
		*(COMMON)

		__bss_end__ = . ;
	} >INTRAM1


	/* set the top of the stack to the end of INTRAM1 */
	stack_top = ORIGIN(INTRAM1) + LENGTH(INTRAM1) ;


	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	/* DWARF debug sections.
		Symbols in the DWARF debugging sections are relative to the beginning
		of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
}

