puts "Loading netx_swreset.tcl"

# uses mread32

proc peek { strFmt ulAddr } {
	set ulVal [ mread32 $ulAddr ]
	puts [ format $strFmt $ulVal ]
	return $ulVal
}

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


proc check_chiptyp { ulResetAddrRead ulResetAddr ulCheckAddr ulCheckVal } {
	set ret false
	if { $ulResetAddrRead == $ulResetAddr } {
		set ulCheckValRead [ mread32 $ulCheckAddr ]
		if {$ulCheckValRead == $ulCheckVal } {
			set ret true
		}
	}
	
	return $ret
}

# An implementation of romloader::get_chiptyp
# Assumes that the target is halted and memory access is possible.
proc get_chiptyp { }  {
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

	set ulResetAddrRead [ mread32 0 ]
	puts [ format "Reset vector: %x " $ulResetAddrRead ]
	if       { [ check_chiptyp $ulResetAddrRead 0xea080001 0x00200008 0x00001000 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX500               
		echo "netX 500 detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xea080002 0x00200008 0x00003002 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX100           
		echo "netX 100 detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xeac83ffc 0x08200008 0x00002001 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX50            
		echo "netX 50 detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xeafdfffa 0x08070008 0x00005003 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX10            
		echo "netX 10 detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xeafbfffa 0x080f0008 0x00006003 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX56            
		echo "netX 51/52 step A detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xeafbfffa 0x080f0008 0x00106003 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX56B           
		echo "netX 51/52 step B detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0xe59ff00c 0x04100020 0x00108004 ] } { 
		set iChiptyp $ROMLOADER_CHIPTYP_NETX4000_RELAXED  
		echo "netX 4000 detected"
	} elseif { [ check_chiptyp $ulResetAddrRead 0x2009fff0 0x00005110 0x1f13933b ] } { 
		echo "netX 90MPW detected"
		set iChiptyp $ROMLOADER_CHIPTYP_NETX90_MPW        
	} else {
		set iChiptyp $ROMLOADER_CHIPTYP_UNKNOWN
		echo "Unknown chiptyp"
	}
		
	return $iChiptyp
}

# Write the DPM reset request sequence by
# reading the requested values from the request register
# and writing them back.
# The last read/write is done by a program. 
# After the write, the program loops endlessly.
proc dpm_reset_write_back {ulDPMAddr iNumSteps ulCodeAddr} {
	for {set i 1} {$i<$iNumSteps} {incr i} {
		set val [mread32 $ulDPMAddr]
		echo [format "i=%d, reset request value=%08x" $i $val]
		mww phys $ulDPMAddr $val 
	}	
	
	# Perform the last write in software.
	mww phys $ulCodeAddr            0xE5901000; #       ldr r1, [r0]
	mww phys [expr $ulCodeAddr + 4] 0xE5801000; #       str r1, [r0]
	mww phys [expr $ulCodeAddr + 8] 0xEAFFFFFE; # loop: b loop	
	reg r0 $ulDPMAddr 
	reg pc $ulCodeAddr 
	reg cpsr 0x1f
	resume	
}


# Run the reset request sequence via DPM by 
# writing the value sequence directly (without reading)
proc dpm_reset_write_direct {ulAddr iNumSteps} {
	for {set i 1; set val 0} {$i<$iNumSteps} {incr i; set val [expr $val + $val + 1]} {
		puts "i=$i, val=$val"
		mww phys $ulAddr $val
	}	
	
	#       str r1, [r0]
	# loop: b loop	
	mww phys 0x00010000 0xE5801000
	mww phys 0x00010004 0xEAFFFFFE
	reg r0 $ulAddr 
	reg r1 $val
	reg pc 0x00010000
	reg cpsr 0x1f
	resume
}


proc reset_assert {} {
	echo "+Reset-assert"
	
	# input (must have been set previously)
	global ROMLOADER_CHIPTYP_UNKNOWN 
	global ROMLOADER_CHIPTYP_NETX500 
	global ROMLOADER_CHIPTYP_NETX100 
	global ROMLOADER_CHIPTYP_NETX50  
	global ROMLOADER_CHIPTYP_NETX10  
	global ROMLOADER_CHIPTYP_NETX56  
	global ROMLOADER_CHIPTYP_NETX56B 
	global ROMLOADER_CHIPTYP_NETX4000_RELAXED
	global ROMLOADER_CHIPTYP_NETX90_MPW

	# output (set according to chip type)
	global ADDR_ACCESS_KEY       
	global ADDR_CLOCK_ENABLE     
	global ADDR_CLOCK_ENABLE_MSK 
	global MSK_CLOCK_ENABLE_DPM  
	global ADDR_RESET_CTRL       
	global MSK_RES_REQ_FIRMWARE  
	global ADDR_DPM_RESET_REQ    
	global DPM_RESET_LEN         

	halt
	
	set iChiptyp [ get_chiptyp ]
	
	if { $iChiptyp == $ROMLOADER_CHIPTYP_NETX500 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX100} {
		set ADDR_ACCESS_KEY       0x00100070
		
		set ADDR_CLOCK_ENABLE     0x00100024
		set ADDR_CLOCK_ENABLE_MSK 0x00100028
		set MSK_CLOCK_ENABLE_DPM  0x00000100
	
		set ADDR_RESET_CTRL       0x0010000c
		set MSK_RES_REQ_FIRMWARE  0x01000000
		
		set ADDR_DPM_RESET_REQ    0x001031dc
		set DPM_RESET_LEN         8
		set ADDR_WRITE_STUB       0x00010000
	
		arm9 vector_catch reset
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX50 } {
		set ADDR_ACCESS_KEY       0x1c000070
		
		set ADDR_CLOCK_ENABLE     0x1c000024
		set ADDR_CLOCK_ENABLE_MSK 0x1c000028
		set MSK_CLOCK_ENABLE_DPM  0x00000100
	
		set ADDR_RESET_CTRL       0x1c00000c
		set MSK_RES_REQ_FIRMWARE  0x01000000
	
		set ADDR_DPM_RESET_REQ    0x1c0031dc
		set DPM_RESET_LEN         8
		set ADDR_WRITE_STUB       0x00010000
	
		arm9 vector_catch reset
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX10 } {
		set ADDR_ACCESS_KEY       0x101c0070 
		
		set ADDR_CLOCK_ENABLE     0x101c0028
		set ADDR_CLOCK_ENABLE_MSK 0x101c002c
		set MSK_CLOCK_ENABLE_DPM  0x00000100
		
		set ADDR_RESET_CTRL       0x101c000c
		set MSK_RES_REQ_FIRMWARE  0x01000000
			
		set ADDR_DPM_RESET_REQ    0x101c12dc
		set DPM_RESET_LEN         9
		set ADDR_WRITE_STUB       0x00010000
		
		arm9 vector_catch reset
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX56 || $iChiptyp == $ROMLOADER_CHIPTYP_NETX56B } {
		set ADDR_ACCESS_KEY       0x1018c17c
		
		set ADDR_CLOCK_ENABLE     0x1018c138
		set ADDR_CLOCK_ENABLE_MSK 0x1018c13c
		set MSK_CLOCK_ENABLE_DPM  0x00080000
	
		set ADDR_RESET_CTRL       0x1018c110
		set MSK_RES_REQ_FIRMWARE  0x01000000
	
		set ADDR_DPM_RESET_REQ    0x1018c0dc
		set DPM_RESET_LEN         8
		set ADDR_WRITE_STUB       0x00010000
		
		arm9 vector_catch reset
		
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX4000_RELAXED } {
		echo "netx 4000 relaxed not handled"
	} elseif { $iChiptyp == $ROMLOADER_CHIPTYP_NETX90_MPW } {
		echo "netx 90 MPW not handled"
	} else {
		echo "unknown chip type"
	}

	peek "Clock enable mask: 0x%08x" $ADDR_CLOCK_ENABLE_MSK
	peek "Clock enable:      0x%08x" $ADDR_CLOCK_ENABLE
	puts "Enable DPM clock"
	mww phys $ADDR_ACCESS_KEY [mread32 $ADDR_ACCESS_KEY]
	mww phys $ADDR_CLOCK_ENABLE [expr $MSK_CLOCK_ENABLE_DPM ]
	peek "Clock enable:       0x%08x" $ADDR_CLOCK_ENABLE

	set resetctrl [ peek "Reset flags:0x%08x" $ADDR_RESET_CTRL ]
	puts "Confirm reset flags"
	set resetctrl [mread32 $ADDR_RESET_CTRL]
	mww phys $ADDR_ACCESS_KEY [mread32 $ADDR_ACCESS_KEY]	
	mww phys $ADDR_RESET_CTRL $resetctrl
	peek "Reset flags: 0x%08x" $ADDR_RESET_CTRL
	
	#echo "Trigger reset via AsicCtrl reset request"		
	#mww phys $ADDR_ACCESS_KEY [mread32 $ADDR_ACCESS_KEY]	
	#mww phys $ADDR_RESET_CTRL $MSK_RES_REQ_FIRMWARE
	
	puts "Reset via DPM reset request"
	#dpm_reset_write_direct $ADDR_DPM_RESET_REQ $DPM_RESET_LEN
	dpm_reset_write_back $ADDR_DPM_RESET_REQ $DPM_RESET_LEN $ADDR_WRITE_STUB
	
	echo "-Reset-assert"
}

proc reset_deassert_post {} {
	echo "+reset_deassert_post"	
	
	global ADDR_ACCESS_KEY       
	global ADDR_RESET_CTRL       
	
	halt 

	set resetctrl [ peek "Reset flags:0x%08x" $ADDR_RESET_CTRL ]
	puts "Confirm reset flags"
	set resetctrl [mread32 $ADDR_RESET_CTRL]
	mww phys $ADDR_ACCESS_KEY [mread32 $ADDR_ACCESS_KEY]	
	mww phys $ADDR_RESET_CTRL $resetctrl
	peek "Reset flags: 0x%08x" $ADDR_RESET_CTRL
		
	echo "-reset_deassert_post"
}




proc reset_assert_netx4000 {} {
	echo "+reset_assert_netx4000"

	# access key
	set ADDR_NX4000_ACCESS_KEY             0xf408017c   
	                                       
	# control isolation of netx part before reset
	set ADDR_NX4000_RESET_CTRL             0xf4080110   
	                                       
	# clock enable	
	set ADDR_NX4000_CLK_ENABLE             0xf4080138   
	set ADDR_NX4000_CLK_ENABLE_MSK         0xf408013c
	set MSK_NX4000_ENABLE_DPM_CLK          0x00400000
                                           
	# DPM reset request
	set ADDR_NX4000_DPM_RES_REQ            0xf40810dc   
	set ADDR_NX4000_IDPM0_RES_REQ          0xf40812dc
	set ADDR_NX4000_IDPM1_RES_REQ          0xf40813dc

	set DPM_RESET_LEN                      8
	set ADDR_WRITE_STUB                    0x00010000
	
	# Software  Reset Request
	set ADDR_NX4000_RAP_SYSCTRL_RSTCTRL    0xf8000050   
	set MSK_NX4000_RSTCTRL_SWRESET_REQ     0x00000001
	
	# enable/disable reset sources	
	set ADDR_NX4000_RAP_SYSCTRL_RSTMASK    0xf8000054	
	# indicate reset sources	
	set ADDR_NX4000_RAP_SYSCTRL_RSTSTAT    0xf8000058   
	# clear reset indications	
	set ADDR_NX4000_RAP_SYSCTRL_RSTSTATCLR 0xf800005c   
	# flag for reset from DPM	
	set MSK_NX4000_RST_NETX_DPM            0x00000010   
	# flag for reset from SWRESET register	
	set MSK_NX4000_RST_SWRST               0x00000001   
	
	halt
		
	peek "Clock enable mask: " $ADDR_NX4000_CLK_ENABLE_MSK
	peek "Clock enable value:" $ADDR_NX4000_CLK_ENABLE	
	echo "enable DPM"
	mww phys $ADDR_NX4000_ACCESS_KEY [mread32 $ADDR_NX4000_ACCESS_KEY]
	mww phys $ADDR_NX4000_CLK_ENABLE [expr $MSK_NX4000_ENABLE_DPM_CLK ]
	peek "Clock enable value:" $ADDR_NX4000_CLK_ENABLE

	peek "Reset mask: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTMASK
	echo "Enable DPM and SWRST as reset source"
	mww phys  $ADDR_NX4000_RAP_SYSCTRL_RSTMASK [expr $MSK_NX4000_RST_NETX_DPM + $MSK_NX4000_RST_SWRST ]
	peek "Reset mask: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTMASK

	set val [ peek "Reset flags: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTSTAT ]
	echo "Confirm"
	mww phys $ADDR_NX4000_RAP_SYSCTRL_RSTSTATCLR $val
	peek "Reset flags: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTSTAT
	
	echo "Reset via DPM"
	dpm_reset_write_back $ADDR_NX4000_DPM_RES_REQ $DPM_RESET_LEN $ADDR_WRITE_STUB 
	
	#echo "Reset via AsicCtrl"
	#mww phys $ADDR_NX4000_ACCESS_KEY [mread32 $ADDR_NX4000_ACCESS_KEY]
	#mww phys $ADDR_NX4000_RESET_CTRL $MSK_NX4000_RES_REQ_FIRMWARE
	
	echo "-reset_assert_netx4000"
}

proc reset_deassert_post_netx4000 {} {
	echo "+reset_deassert_post_netx4000"
	
	# indicate reset sources	
	set ADDR_NX4000_RAP_SYSCTRL_RSTSTAT    0xf8000058   
	# clear reset indications	
	set ADDR_NX4000_RAP_SYSCTRL_RSTSTATCLR 0xf800005c   
	
	halt
	
	set val [ peek "Reset flags: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTSTAT ]
	echo "Confirm"
	mww phys $ADDR_NX4000_RAP_SYSCTRL_RSTSTATCLR $val
	peek "Reset flags: %08x" $ADDR_NX4000_RAP_SYSCTRL_RSTSTAT
	
	echo "-reset_deassert_post_netx4000"

}
