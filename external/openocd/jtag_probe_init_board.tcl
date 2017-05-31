

# Configure an NXHX JTAG interface.
proc setup_interface_nxhx {strBoardName} {
	interface ftdi
	transport select jtag
	ftdi_device_desc $strBoardName
	ftdi_vid_pid 0x0640 0x0028
	adapter_khz 100
	
	ftdi_layout_init 0x0108 0x010b
	ftdi_layout_signal nTRST -data 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200
}

# Configure an interface.
proc setup_interface {strInterfaceID}
{
	if {$strInterfaceID == "NXJTAG-USB" } {
		interface ftdi
		transport select jtag
		ftdi_device_desc "NXJTAG-USB"
		ftdi_vid_pid 0x1939 0x0023
		adapter_khz 100
	
		ftdi_layout_init 0x0308 0x030b
		ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
		ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200

	} elseif {$strInterfaceID == "Amontec_JTAGkey" } {
		interface ftdi
		transport select jtag
		ftdi_device_desc "Amontec JTAGkey"
		ftdi_vid_pid 0x0403 0xcff8
		adapter_khz 100
		
		ftdi_layout_init 0x0c08 0x0f1b
		ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400
		ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800
	
	} elseif {$strInterfaceID == "NXHX50-RE" } {
		setup_interface_nxhx "NXHX50-RE"
		
	} elseif {$strInterfaceID == "NXHX_51-ETM" } {
		setup_interface_nxhx "NXHX 51-ETM"
		
	} elseif {$strInterfaceID == "NXHX_500-ETM" } {
		setup_interface_nxhx "NXHX 500-ETM"
	} 
	
}


proc probe_interface {} {
	set RESULT -1
	
	if {[ catch {jtag init} ]==0 } {
			set RESULT {OK}
	}
	return $RESULT
}

			working_area 0 0x0380 0x0080 backup

proc probe_cpu {strCpuID} {
	global SC_CFG_RESULT
	set SC_CFG_RESULT 0
	
	if { $strCpuID == "netX_ARM966" } {
		
		jtag newtap netX_ARM966 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x25966021
		jtag configure netX_ARM966.cpu -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }
		jtag init
		
		if { $SC_CFG_RESULT=={OK} } {
			target create netX_ARM966.cpu arm966e -endian little -chain-position netX_ARM966.cpu
			netX_ARM966.cpu configure -event reset-init { halt }
			netX_ARM966.cpu configure -work-area-phys 0x0380 -work-area-size 0x0080 -work-area-backup 1
		}
		
	
	} elseif { $strCpuID == "netX_ARM926" } {
		jtag newtap netX_ARM926 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x07926021
		jtag configure netX_ARM926.cpu -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }
		jtag init
		
		if { $SC_CFG_RESULT=={OK} } {
			target create netX_ARM926.cpu arm926ejs -endian little -chain-position netX_ARM926.cpu
			netX_ARM926.cpu configure -event reset-init { halt }
			netX_ARM926.cpu configure -work-area-phys 0x0380 -work-area-size 0x0080 -work-area-backup 1
		}
	}
	
	return $SC_CFG_RESULT
}

proc reset_board {} {
	reset_config trst_and_srst
	adapter_nsrst_delay 500
	jtag_ntrst_delay 500
	
	init
	reset init
}