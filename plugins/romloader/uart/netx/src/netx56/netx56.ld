/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

ENTRY(start)

MEMORY
{
	ASSERT_EMPTY(rwx):  ORIGIN = 0x00000000,  LENGTH = 0

	INTRAM_MON(rwx):    ORIGIN = 0x08000100,  LENGTH = 0x00003f00
	INTRAM_OPT(rx):     ORIGIN = 0x080912dc,  LENGTH = 0x0000000a
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
		*(.igot.plt)
		*(.iplt)
		*(.vfp11_veneer)
		*(.v4_bx)
	} >ASSERT_EMPTY


	.serial_vectors ORIGIN(INTRAM_MON) :
	{
		tSerialV2Vectors = . ;
		. = . + 0x10;
	} >INTRAM_MON


	.code :
	{
		load_address = . ;

		*(.header)
		*(.init_code)
		*(.text*)
		*(.rodata*)
	} >INTRAM_MON
	end_address = LOADADDR(.code) + SIZEOF(.code);

	.bss :
	{
		/* zero-initialized data */
		__bss_start__ = . ;

		*(.bss)
		*(COMMON)

		__bss_end__ = . ;
	} >INTRAM_MON

	/* Set the top of the stack to the start of the code. */
	stack_top = ORIGIN(INTRAM_MON) + LENGTH(INTRAM_MON);


	.opt ORIGIN(INTRAM_OPT):
	{
		PROVIDE( atUartCfg = . );
		. = . + 10;
	} >INTRAM_OPT


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

