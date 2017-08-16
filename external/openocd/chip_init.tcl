echo "Loading chip_init.tcl"

set ROMLOADER_CHIPTYP_UNKNOWN   0 
set ROMLOADER_CHIPTYP_NETX500   1 
set ROMLOADER_CHIPTYP_NETX100   2 
set ROMLOADER_CHIPTYP_NETX50    3 
set ROMLOADER_CHIPTYP_NETX5     4 
set ROMLOADER_CHIPTYP_NETX10    5 
set ROMLOADER_CHIPTYP_NETX56    6 
set ROMLOADER_CHIPTYP_NETX56B   7 
set ROMLOADER_CHIPTYP_NETX4000_RELAXED  8 
set ROMLOADER_CHIPTYP_NETX90_MPW 10

# fEnableDCCOutput       true: download DCC code, set serial vectors and buffer, false: clear serial vectors
# ulSerialVectorAddr     Address of serial vectors
# strDccCodeFile         DCC code filename
# ulDccCodeAddr          Address of DCC code
# ulDccBufferAddr        Address of DCC buffer
# ulDccBufferPointerAddr Address of DCC buffer pointer

proc setup_dcc_io {fEnableDCCOutput {ulSerialVectorAddr 0} {strDccCodeFile ""}  {ulDccCodeAddr 0} {ulDccBufferAddr 0} {ulDccBufferPointerAddr 0}} {
	if { $fEnableDCCOutput == "true" } {
		puts "Setting up DCC output"

		# dcc put/flush routines
		puts "Loading DCC code"
		load_image $strDccCodeFile $ulDccCodeAddr bin
		
		puts "Setting serial vectors"
		mww       $ulSerialVectorAddr         0                          ; # Get
		mww [expr $ulSerialVectorAddr + 4  ]  $ulDccCodeAddr             ; # Put
		mww [expr $ulSerialVectorAddr + 8  ]  0                          ; # Peek
		mww [expr $ulSerialVectorAddr + 12 ]  [expr $ulDccCodeAddr + 4 ] ; # Flush
		
		# reset buffer pointer
		puts "Setting buffer pointer"
		mww $ulDccBufferPointerAddr  $ulDccBufferAddr
		
	} else {
		puts "Setting up for no DCC output (Clearing serial vectors)"
		mww       $ulSerialVectorAddr         0 ; # Get
		mww [expr $ulSerialVectorAddr + 4  ]  0 ; # Put
		mww [expr $ulSerialVectorAddr + 8  ]  0 ; # Peek
		mww [expr $ulSerialVectorAddr + 12 ]  0 ; # Flush
	}
	
	mdw       $ulSerialVectorAddr        
	mdw [expr $ulSerialVectorAddr + 4  ] 
	mdw [expr $ulSerialVectorAddr + 8  ] 
	mdw [expr $ulSerialVectorAddr + 12 ] 
}

proc init_chip {iChiptyp} {
	set fEnableDCCOutput true

	global ROMLOADER_CHIPTYP_UNKNOWN 
	global ROMLOADER_CHIPTYP_NETX500 
	global ROMLOADER_CHIPTYP_NETX100 
	global ROMLOADER_CHIPTYP_NETX50  
	global ROMLOADER_CHIPTYP_NETX5   
	global ROMLOADER_CHIPTYP_NETX10  
	global ROMLOADER_CHIPTYP_NETX56  
	global ROMLOADER_CHIPTYP_NETX56B 
	global ROMLOADER_CHIPTYP_NETX4000_RELAXED
	global ROMLOADER_CHIPTYP_NETX90_MPW

	puts "init_chip $iChiptyp"


	
	###############################
	# SDRAM fix for netX 50
	###############################
	
	if {$iChiptyp == $ROMLOADER_CHIPTYP_NETX50} {
		puts "Executing SDRAM fix for netX 50."
		mww 0x1c005830 0x00000001
		mww 0x1c005104 0xBFFFFFFC
		mww 0x1c00510c 0x00480001
		mww 0x1c005110 0x00000001
		sleep 10
		mww 0x1c00510c 0
		mww 0x1c005110 0
		mww 0x1c005830 0
	}

	###################################
	# init regs 
	###################################
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100 } {
		puts "Setting up registers for netX 500/100."
		# no TCM, stack at 0x7ffc
		#bp 0x200000 4 hw
		#reg cpsr 0xd3
		#reg spsr_svc 0xd3
		#reg r13_svc 0x7ffc
		#reg lr_svc 0x200000
	
		# TCM enabled, stack at 0x10000200
		bp 0x200000 4 hw
		reg cpsr 0xd3
		reg r13_svc 0x10000200
		reg lr_svc 0x200000
		arm926ejs cp15 0 0 7 7 0
		arm926ejs cp15 0 4 7 10 0
		arm926ejs cp15 0 0 8 7 0
		arm926ejs cp15 0 0 1 0 0x00050078
		arm926ejs cp15 0 0 9 1 0x10000001
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX50 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
		puts "Setting up registers for netX 10/50/51/52."
		# ldr     r0,     =0x0f7c  
		# MCR    <co-pro>, <op>, <ARM reg>, <co-pro reg>, <co-pro reg2>, <op2>
		# mcr     p15,       0,        r0,            c1,            c0,  0
		
		# set the ctrl register to ... 
		# 15: 0 TBIT enable
		# 12: 0 disable ITCM
		# 7:  0 little endianess
		# 3:  1 enable buffered writes in bufferable areas 
		# 2:  1 enable DTCM 
		# 1:  0 disable alignment check
		
		# apparently, a second write/read access must be made to the register
		arm966e cp15 2 0x00000f7c
		#arm966e cp15 2 0x00000f7c
		arm966e cp15 2 
		
		# init regs
		bp 0x080000fc 4 hw        ; # set breakpoint
		reg cpsr 0xd3             ; # disable FIQ/IRQ, disable Thumb, set Abort mode
		reg spsr_svc 0xd3
		reg sp_svc 0x08000dfc    ; # stack 0500-0dfc
		reg lr_svc 0x080000fc     ; # when any program returns, jump to the breakpoint address, stopping execution
	
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_RELAXED } {
	
		puts "Init netx 4000 - WIP"

		# enable DTCM
		
		# DTCM Region Register - DTCMRR
		# TRM chapter 4.3.13
		# 
		# MRC p15, 0, <Rd>, c9, c1, 0
		# -> arm mrc pX coproc op1 CRn CRm op2
		
		# MCR p15, 0, <Rd>, c9, c1, 0
		# -> arm mcr pX op1 CRn CRm op2 value
		
		# http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0458c/CHDEFBFI.html
		set MSK_CR7_CP15_DTCMRR_Enable      0x00000001
		set VAL_CR7_CP15_DTCMRR_Size_128KB  8
		set SRT_CR7_CP15_DTCMRR_Size        2
		set SRT_CR7_CP15_DTCMRR_SBZ6        6 
		set VAL_CR7_CP15_DTCMRR_Base_addr   0x00020000
		set VAL_CR7_CP15_DTCMRR             [ expr $VAL_CR7_CP15_DTCMRR_Base_addr | ($VAL_CR7_CP15_DTCMRR_Size_128KB << $SRT_CR7_CP15_DTCMRR_SBZ6) | $MSK_CR7_CP15_DTCMRR_Enable ]
		#set VAL_CR7_CP15_DTCMRR             [ expr $VAL_CR7_CP15_DTCMRR_Base_addr | ($VAL_CR7_CP15_DTCMRR_Size_128KB << $SRT_CR7_CP15_DTCMRR_Size) | $MSK_CR7_CP15_DTCMRR_Enable ]
		
		arm mcr 15 0 9 1 0 $VAL_CR7_CP15_DTCMRR
		arm mcr 15 0 9 1 0 $VAL_CR7_CP15_DTCMRR
		
		bp 0x04100000 4 hw
		reg cpsr 0xd3
		reg sp_svc 0x0003ffec
		reg lr_svc 0x04100000
		
		# test DTCM
		# mww 0x0003fff0 0x12345678
		# set value(0) 0
		# mem2array value 32 0x0003fff0 1
		# puts "0x0003fff0" 
		# puts $value(0)
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
		puts "Init netx 90 MPW"
		reg xPSR 0x01000000
		# reg MSP 0x00040000
		
		bp 0x2009fff8 4 hw
		reg cpsr 0xd3 #?
		reg sp_svc 0x2009fff8
		reg lr_svc 0x2009fffc
		
		
	} else {
		puts "Unknown chip type $iChiptyp"
	}
	

	
	########################################
	# Set up DCC output or disable output
	########################################
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100 } {
		setup_dcc_io $fEnableDCCOutput 0x10001ff0 netx500_dccout_0400_10001e00.bin  0x0400 0x10001e00 0x10001fe0 
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX50 } {
		setup_dcc_io $fEnableDCCOutput 0x04000ff0 netx10_50_51_52_dccout_0400_04000e00.bin 0x0400 0x04000e00 0x04000fe0 
			
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
		setup_dcc_io $fEnableDCCOutput 0x08000100 netx10_50_51_52_dccout_0400_04000e00.bin 0x0400 0x04000e00 0x04000fe0
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_RELAXED } {
		# debugmsgs on Cortex-R/A are not supported by openOCD 0.10.0
		setup_dcc_io false 0x0003fff0 netx4000cr7_dccout_05080000_05080e00.bin 0x05080000 0x05080e00 0x05080fe0
		
		#netx 4000 UART iovcectors: 0x04101445 0x04101479 0x04101449 0x0410144D 
		#puts "Debug: setting UART vectors on netx 4000"
		#mww 0x0003fff0 0x04101445 ; # Get
		#mww 0x0003fff4 0x04101479 ; # Put
		#mww 0x0003fff8 0x04101449 ; # Peek
		#mww 0x0003fffc 0x0410144D ; # Flush
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
		
		
	} else {
		puts "Unknown chip type $iChiptyp"
	}
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 ||
		$iChiptyp == $ROMLOADER_CHIPTYP_NETX50 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B ||
		$iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
		arm7_9 dcc_downloads enable
		arm7_9 fast_memory_access enable
		target_request debugmsgs enable
	}
	
	# Is this speed ok or should it be 1 MHz?
	adapter_khz 6000
	
}


#                          Effective intram locations
# Breakpoint/LR                 00fc 
# Serial Vectors netX10/51/52   0100 - 010f 
# DCC workarea                  0380 - 03ff
# DCC code                      0400 - 04ff 
# Initial stack                 0500 - 0dfc
# Buffer                        0e00 - 0eff (netX 50: DTCM 0x04000e00)
# Buffer pointer                0fe0        (netX 50: DTCM 0x04000fe0)
# Serial Vectors netX50         0ff0 - 0fff (netX 50: DTCM 0x04000ff0)

echo "Done loading chip_init.tcl"