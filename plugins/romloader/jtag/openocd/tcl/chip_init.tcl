echo "Loading chip_init.tcl"

set ROMLOADER_CHIPTYP_UNKNOWN            0
set ROMLOADER_CHIPTYP_NETX500            1
set ROMLOADER_CHIPTYP_NETX100            2
set ROMLOADER_CHIPTYP_NETX50             3
set ROMLOADER_CHIPTYP_NETX5              4
set ROMLOADER_CHIPTYP_NETX10             5
set ROMLOADER_CHIPTYP_NETX56             6
set ROMLOADER_CHIPTYP_NETX56B            7
set ROMLOADER_CHIPTYP_NETX4000_RELAXED   8
set ROMLOADER_CHIPTYP_NETX90_MPW        10
set ROMLOADER_CHIPTYP_NETX4000_FULL     11
set ROMLOADER_CHIPTYP_NETX4100_SMALL    12
set ROMLOADER_CHIPTYP_NETX90            13

# fEnableDCCOutput       true: download DCC code, set serial vectors and buffer, false: clear serial vectors
# ulSerialVectorAddr     Address of serial vectors
# strDccCodeFile         DCC code filename
# strArmThumb            "arm" or "thumb"
# ulDccCodeAddr          Address of DCC code
# ulDccBufferAddr        Address of DCC buffer
# ulDccBufferPointerAddr Address of DCC buffer pointer

proc setup_dcc_io {fEnableDCCOutput {ulSerialVectorAddr 0} {strDccCodeFile ""} {strArmThumb "arm"} {ulDccCodeAddr 0}  {ulDccBufferAddr 0} {ulDccBufferPointerAddr 0}} {
	if { $fEnableDCCOutput == "true" } {
		puts "Setting up DCC output"

		# dcc put/flush routines
		puts "Loading DCC code"
		load_image $strDccCodeFile $ulDccCodeAddr bin

		puts "Setting serial vectors"
		
		if { $strArmThumb == "thumb" } {
			set iOffset 1
		} else {
			set iOffset 0
		}

		mww       $ulSerialVectorAddr         0                                    ; # Get
		mww [expr $ulSerialVectorAddr + 4  ]  [expr $ulDccCodeAddr + $iOffset]     ; # Put
		mww [expr $ulSerialVectorAddr + 8  ]  0                                    ; # Peek
		mww [expr $ulSerialVectorAddr + 12 ]  [expr $ulDccCodeAddr + 4 + $iOffset] ; # Flush
		
		# reset buffer pointer
		puts "Setting buffer pointer"
		mww $ulDccBufferPointerAddr  $ulDccBufferAddr

		target_request debugmsgs enable
		
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
	mdw $ulDccBufferPointerAddr
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
	global ROMLOADER_CHIPTYP_NETX4000_FULL
	global ROMLOADER_CHIPTYP_NETX4100_SMALL
	global ROMLOADER_CHIPTYP_NETX90


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
		reg sp_svc 0x10000200
		reg lr_svc 0x200000

		# arm926ejs cp15 opcode_1 opcode_2 CRn CRm value
		# MCR/MRC p15, <Opcode_1>, <Rd>, c15, <CRm>, <Opcode_2>
		# arm926ejs cp15 0 0 7 7 0
		# arm926ejs cp15 0 4 7 10 0
		# arm926ejs cp15 0 0 8 7 0
		# arm926ejs cp15 0 0 1 0 0x00050078
		# arm926ejs cp15 0 0 9 1 0x10000001

		# arm mcr pX op1 CRn CRm op2 value
		arm mcr 15 0 7  7 0 0
		arm mcr 15 0 7 10 4 0
		arm mcr 15 0 8  7 0 0
		arm mcr 15 0 1  0 0 0x00050078
		arm mcr 15 0 9  1 0 0x10000001

		
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
	
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_RELAXED \
			|| $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_FULL \
			|| $iChiptyp == $ROMLOADER_CHIPTYP_NETX4100_SMALL} {

		puts "Init netx 4000"
		
		bp 0x04100000 4 hw
		reg cpsr 0xd3
		reg sp_svc 0x0003ffec
		reg lr_svc 0x04100000
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
	
		puts "Setting up registers for netx 90 MPW"

		puts "Setting analog parameters"
		# d:\projekt\netx90\blinki_jtag\Blinki\scripts\netx90_analog_init.cfg
		mww 0xff0016c0 0x00000000
		mww 0xff0016c4 0x00000000
		mww 0xff0016c8 0x00049f04
		
		# Configure the PLL.
		mww 0xFF0016C8 0x00049f04
		
		# Start the PLL.
		mww 0xFF0016C8 0x00049f05
		
		# Release the PLL reset.
		mww 0xFF0016C8 0x00049f07
		
		# Disable the PLL bypass.
		mww 0xFF0016C8 0x00049f03
		
		# Switch to 100MHz clock by setting d_dcdc_use_clk to 1.
		mww 0xFF0016C8 0x01049f03
		
		# Lock the analog parameter and hide the ROM.
		mww 0xff0016cc 0x00000005
		
		
		
		puts "Configuring pad ctrl for MMIO4-7 (input, disable pull-down)"
		proc read_data32 {addr} {
			set value(0) 0
			mem2array value 32 $addr 1
			return $value(0)
		}

		proc unlock_asic_ctrl {} {
			set accesskey [read_data32 0xff4012c0]
			mww 0xff4012c0 [expr $accesskey]
		}

		unlock_asic_ctrl
		mww 0xff4010c8 0x00000000
		
		unlock_asic_ctrl
		mww 0xff4010cc 0x00000000
		
		unlock_asic_ctrl
		mww 0xff4010d0 0x00000000
		
		unlock_asic_ctrl
		mww 0xff4010d4 0x00000000
		
		mdw 0
		mdw 0xff4010c8
		mdw 0xff4010cc
		mdw 0xff4010d0
		mdw 0xff4010d4
		
		
		reg xPSR 0x01000000
		# reg msp 0x00040000
		bp 0x00023ffd 2 hw
		reg sp 0x2009ff80
		reg lr 0x00023ffd
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90 } {

		puts "Setting up registers for netx 90"

		puts "Setting analog parameters"
		mww 0xff001680 0x00000000
		mww 0xff001684 0x00000000
		mww 0xff0016a8 0x00049f04

		# Configure the PLL.
		mww 0xff0016a8 0x00049f04

		# Start the PLL.
		mww 0xff0016a8 0x00049f05

		# Release the PLL reset.
		mww 0xff0016a8 0x00049f07

		# Disable the PLL bypass.
		mww 0xff0016a8 0x00049f03

		# Switch to 100MHz clock by setting d_dcdc_use_clk to 1.
		mww 0xff0016a8 0x01049f03

		# Lock the analog parameter and hide the ROM.
		mww 0xff0016cc 0x00000005



		puts "Configuring pad ctrl for MMIO4-7 (input, disable pull-down)"
		proc read_data32 {addr} {
			set value(0) 0
			mem2array value 32 $addr 1
			return $value(0)
		}

		proc unlock_asic_ctrl {} {
			set accesskey [read_data32 0xff4012c0]
			mww 0xff4012c0 [expr $accesskey]
		}

		unlock_asic_ctrl
		mww 0xff4010f0 0x00000000

		unlock_asic_ctrl
		mww 0xff4010f4 0x00000000

		unlock_asic_ctrl
		mww 0xff4010f8 0x00000000

		unlock_asic_ctrl
		mww 0xff4010fc 0x00000000

		mdw 0
		mdw 0xff4010f0
		mdw 0xff4010f4
		mdw 0xff4010f8
		mdw 0xff4010fc


		reg xPSR 0x01000000
		bp 0x00023ffd 2 hw
		reg sp 0x2009ff80
		reg lr 0x00023ffd

	} else {
		puts "Unknown chip type $iChiptyp"
	}
	
	########################################
	# Set IO compatibility mode on netx 51/52 
	########################################
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
	
	set ADDR_NX56_ASIC_CTRL_ACCESS_KEY                     0x1018c17c
	set ADDR_NX56_ASIC_CTRL_SAMPLE_AT_NRES                 0x1018c150
	set MSK_NX56_ASIC_CTRL_SAMPLE_AT_NRES_A19_A18          0x000c0000
	set ADDR_NX56_ASIC_CTRL_MISC_ASIC_CTRL                 0x1018c140
	
		set ul_sample_at_nres [mread32 $ADDR_NX56_ASIC_CTRL_SAMPLE_AT_NRES]
		if { $ul_sample_at_nres & $MSK_NX56_ASIC_CTRL_SAMPLE_AT_NRES_A19_A18 == 0 } {
			puts "netX 51/52 in netx 50 compatibility mode"
		} else {
			puts "Disabling netx 50 IO compatibility mode"
			set ul_misc_asic_ctrl [mread32 $ADDR_NX56_ASIC_CTRL_ACCESS_KEY]
			mww $ADDR_NX56_ASIC_CTRL_ACCESS_KEY $ul_misc_asic_ctrl
			mww $ADDR_NX56_ASIC_CTRL_MISC_ASIC_CTRL 0x61
		}
	}
	
	
	########################################
	# Set up DCC output or disable output
	########################################
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100 } {
		setup_dcc_io $fEnableDCCOutput 0x10001ff0 dcc_netx500.bin arm 0x0400 0x10001e00 0x10001fe0 
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX50 } {
		setup_dcc_io $fEnableDCCOutput 0x04000ff0 dcc_netx10_50_51_52.bin arm 0x0400 0x04000e00 0x04000fe0 
			
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
		setup_dcc_io $fEnableDCCOutput 0x08000100 dcc_netx10_50_51_52.bin arm 0x0400 0x04000e00 0x04000fe0
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_RELAXED \
			|| $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_FULL \
			|| $iChiptyp == $ROMLOADER_CHIPTYP_NETX4100_SMALL } {
		
		# debugmsgs on Cortex-R/A are not supported by openOCD 0.10.0
		# Therefore, we always disable output on netx 4000.
		setup_dcc_io false 0x0003fff0 netx4000cr7_dccout_05080000_05080e00.bin arm 0x05080000 0x05080e00 0x05080fe0
		
		#netx 4000 UART iovcectors: 0x04101445 0x04101479 0x04101449 0x0410144D 
		#puts "Debug: setting UART vectors on netx 4000"
		#mww 0x0003fff0 0x04101445 ; # Get
		#mww 0x0003fff4 0x04101479 ; # Put
		#mww 0x0003fff8 0x04101449 ; # Peek
		#mww 0x0003fffc 0x0410144D ; # Flush
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
		setup_dcc_io $fEnableDCCOutput 0x2009fff0 dcc_netx90_com.bin thumb 0x00020400 0x00020e00 0x00020fe0
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90 } {
		# No DCC on netx90
		setup_dcc_io false 0x2009fff0 dcc_netx90_com.bin thumb 0x00020400 0x00020e00 0x00020fe0
		
	} else {
		puts "Unknown chip type $iChiptyp"
		
	}
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 ||
		$iChiptyp == $ROMLOADER_CHIPTYP_NETX50 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
		arm7_9 dcc_downloads enable
		arm7_9 fast_memory_access enable
	}
	
	adapter_khz 1000
	
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


