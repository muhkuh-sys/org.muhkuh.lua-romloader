# Start the machine interface in ROM.
# For netX 90 Rev.1 with PHY v2 or v3, and netX 90 Rev.2
# If R0 is not 0, clear the work area at 0x200a6000..0x200a8000
# Detect the chip type and jump to mi_loop()

	.section .init_code, "ax"

#	.extern stack_top

	.global start

@--------------------------------------

@ The Cortex-M4 in the netX90 is a THUMB-only CPU.
	.syntax unified
	.thumb

    .func start, start
    .type start, STT_FUNC
start:
	# R0 == 0: do not clear work area
	cmp     r0, #0
	beq     noclear
	
	# clear work area
	mov     r0, #0xffffffff
	ldr     r1, addr_workarea_start
	ldr     r2, addr_workarea_end
clear:
	str     r0, [r1], #4
	cmp     r1, r2 
	bmi     clear
noclear:

	# check for netX 90 Rev.1 (PHY v2 or v3)
	mov     r0, #0
	ldr     r0, [r0]
	ldr     r1, netx90_rev1_reset
	cmp     r0, r1 
	bne     check_netx90_rev2
	
	ldr     r0, netx90_rev1_addr1
	ldr     r0, [r0]
	ldr     r1, netx90_rev1_val1
	cmp     r0, r1 
	bne     check_netx90_rev2

	ldr     r0, netx90_rev1_addr2
	ldr     r0, [r0]
	ldr     r1, netx90_rev1_mask2
	and     r0, r1
	ldr     r1, netx90_rev1_val2
	cmp     r0, r1
	bne     check_netx90_rev2

	# netX 90 Rev.1 found
	# Start the machine interface
	ldr     r0, pfn_mi_loop_netx90_rev1
	bx      r0

check_netx90_rev2:
	# check for netX 90 Rev.2
	mov     r0, #0
	ldr     r0, [r0]
	ldr     r1, netx90_rev2_reset
	cmp     r0, r1 
	bne     unknown_chip
	
	ldr     r0, netx90_rev2_addr1
	ldr     r0, [r0]
	ldr     r1, netx90_rev2_val1
	cmp     r0, r1 
	bne     unknown_chip

	ldr     r0, netx90_rev2_addr2
	ldr     r0, [r0]
	ldr     r1, netx90_rev2_mask2
	and     r0, r1
	ldr     r1, netx90_rev2_val2
	cmp     r0, r1
	bne     unknown_chip

	# netX 90 Rev.2 found
	# Start the machine interface
	ldr     r0, pfn_mi_loop_netx90_rev2
	bx      r0

	# Do not start machine interface, return to the terminal console.
unknown_chip:
	bx      lr
	

	# work area
addr_workarea_start: .word 0x200a6000
addr_workarea_end:   .word 0x200a8000
	
	# netx 90 rev.1 (PHY v2 and v3):
netx90_rev1_reset:   .word 0x2009fff0

netx90_rev1_addr1:   .word 0x000000c0
netx90_rev1_val1:    .word 0x0010d005
	
netx90_rev1_addr2:   .word 0x00005110
netx90_rev1_mask2:   .word 0xffffffff
netx90_rev1_val2:    .word 0xe001200c
	
	# Address of mi_loop 
pfn_mi_loop_netx90_rev1: .word 0x0000aa9d


	# netx 90 Rev.2 
netx90_rev2_reset:    .word 0x2009fff0

netx90_rev2_addr1:    .word 0x000000c0
netx90_rev2_val1:     .word 0x0010d005
	
netx90_rev2_addr2:    .word 0xff401298
netx90_rev2_mask2:    .word 0xff7fffff
netx90_rev2_val2:     .word 0x0901020d
	
	# Address of mi_loop 
pfn_mi_loop_netx90_rev2:  .word 0x0000aad9
	
.endfunc


@--------------------------------------

  .end

