puts "loading script jtag_detect_init.tcl"

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


proc setup_interface_nxjtag_usb {} {
	interface ftdi
	transport select jtag
	ftdi_device_desc "NXJTAG-USB"
	ftdi_vid_pid 0x1939 0x0023
	adapter_khz 100

	#ftdi_layout_init 0x0308 0x030b
	#ftdi_layout_signal nTRST -data 0x0100
	#ftdi_layout_signal nSRST -data 0x0200 

	ftdi_layout_init 0x0308 0x030b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200
}

proc setup_interface_olimex_arm_usb_tiny_h {} {
	interface ftdi
	transport select jtag
	ftdi_device_desc "Olimex OpenOCD JTAG ARM-USB-TINY-H"
	ftdi_vid_pid 0x15ba 0x002a
	adapter_khz 100
	
	ftdi_layout_init 0x0808 0x0a1b
	ftdi_layout_signal nSRST -oe 0x0200
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal LED -data 0x0800  
}

proc setup_interface_jtagkey {} {
	interface ftdi
	transport select jtag
	ftdi_device_desc "Amontec JTAGkey"
	ftdi_vid_pid 0x0403 0xcff8
	adapter_khz 100
	
	ftdi_layout_init 0x0c08 0x0f1b
	ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400
	ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800
}


# Configure an interface.
# todo: add Olimex?
proc setup_interface {strInterfaceID} {
	echo "+setup_interface $strInterfaceID"
	if {$strInterfaceID == "NXJTAG-USB" } {
		setup_interface_nxjtag_usb
		
	} elseif {$strInterfaceID == "Amontec_JTAGkey" } {
		setup_interface_jtagkey
	
	} elseif {$strInterfaceID == "NXHX50-RE" } {
		setup_interface_nxhx "NXHX50-RE"
		
	} elseif {$strInterfaceID == "NXHX_51-ETM" } {
		setup_interface_nxhx "NXHX 51-ETM"
		
	} elseif {$strInterfaceID == "NXHX_500-ETM" } {
		setup_interface_nxhx "NXHX 500-ETM"
	} 
	
	echo "-setup_interface $strInterfaceID"
}


proc probe_interface {} {
	echo "+probe_interface"
	set RESULT -1
	
	if {[ catch {jtag init} ]==0 } {
			set RESULT {OK}
	}
	echo "-probe_interface $RESULT"
	return $RESULT
}



proc probe_cpu {strCpuID} {
	echo "+probe_cpu $strCpuID"
	global SC_CFG_RESULT
	set SC_CFG_RESULT 0
	
	if { $strCpuID == "netX_ARM966" } {
		# During the detection phase_ the setup event is triggered when the JTAG chain is successfully verified.
		# Then the target is created, which is not really needed in this moment. The reset-init event is not triggered.
		# When a connection to this interface/device is created, the reset-init event is triggered.
		# The CPU is halted after reset init is called, but before the reset-init event handler is called.
		jtag newtap netX_ARM966 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x25966021
		jtag configure netX_ARM966.cpu -event setup { global SC_CFG_RESULT ; echo {Yay - setup ARM966} ; set SC_CFG_RESULT {OK} }
		echo "+jtag init"
		jtag init
		echo "-jtag init"
		
		if { $SC_CFG_RESULT=={OK} } {
			echo {creating target netX_ARM966.cpu}
			target create netX_ARM966.cpu arm966e -endian little -chain-position netX_ARM966.cpu
			#this halt might not really do anything, the cpu is halted before the message appears .
			netX_ARM966.cpu configure -event reset-init { echo {netX_ARM966.cpu  reset-init event}; halt } 
			netX_ARM966.cpu configure -work-area-phys 0x0380 -work-area-size 0x0080 -work-area-backup 1
			
			global strTarget
			set strTarget ARM966
			echo {Done creating target netX_ARM966.cpu}
		}
		
	
	} elseif { $strCpuID == "netX_ARM926" } {
		jtag newtap netX_ARM926 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x07926021
		jtag configure netX_ARM926.cpu -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }
		jtag init
		
		if { $SC_CFG_RESULT=={OK} } {
			target create netX_ARM926.cpu arm926ejs -endian little -chain-position netX_ARM926.cpu
			netX_ARM926.cpu configure -event reset-init { halt }
			netX_ARM926.cpu configure -work-area-phys 0x0380 -work-area-size 0x0080 -work-area-backup 1
			
			global strTarget
			set strTarget ARM926
		}
	
	} elseif { $strCpuID == "netX4000_R7" } {
		jtag newtap netx4000 dap -expected-id 0x4ba00477 -irlen 4
		jtag configure netx4000.dap -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }
		jtag init
		
		if { $SC_CFG_RESULT=={OK} } {
			#
			# Cortex R7 target
			#
			target create netx4000.r7 cortex_r4 -chain-position netx4000.dap -coreid 0 -dbgbase 0x80130000
			netx4000.r7 configure -work-area-phys 0x05080000 -work-area-size 0x4000 -work-area-backup 1
			
			netx4000.r7 configure -event reset-assert-post "cortex_r4 dbginit"
			
			# Dual Cortex A9s
			target create netx4000.a9_0 cortex_a -chain-position netx4000.dap -coreid 1 -dbgbase 0x80110000
			netx4000.a9_0 configure -work-area-phys 0x05000000 -work-area-size 0x4000 -work-area-backup 1
			target create netx4000.a9_1 cortex_a -chain-position netx4000.dap -coreid 2 -dbgbase 0x80112000
			netx4000.a9_1 configure -work-area-phys 0x05004000 -work-area-size 0x4000 -work-area-backup 1
			
			netx4000.a9_0 configure -event reset-assert-post "cortex_a dbginit"
			netx4000.a9_1 configure -event reset-assert-post "cortex_a dbginit"
			
			# Only reset r7 (master CPU)
			netx4000.a9_0 configure -event reset-assert ""
			netx4000.a9_1 configure -event reset-assert ""
			
			target smp netx4000.a9_0 netx4000.a9_1
			
			# Select r7 as default target
			targets netx4000.r7
			
			global strTarget
			set strTarget netX4000
		}
		
		
	}
	echo "- probe_cpu $SC_CFG_RESULT"
	return $SC_CFG_RESULT
}

# todo: pass target name from plugin 
proc reset_board {} {
	global strTarget
	echo "+ reset_board  target: $strTarget"
	if { $strTarget == "netX4000" } then {
		reset_netx4000
	} else {
		reset_netX_ARM926_ARM966
	}
	echo "- reset_board "
}

proc reset_netX_ARM926_ARM966 {} {
	reset_config trst_and_srst
	#reset_config none separate
	adapter_nsrst_delay 500
	jtag_ntrst_delay 500
	
	echo {+init}
	init
	echo {-init}
	echo {+reset init}
	reset init
	echo {-reset init}
}




proc mread32 {addr} {
  set value(0) 0
  mem2array value 32 $addr 1
  return $value(0)
}

proc set_firewall {addr value} {
	puts $addr
  set accesskey [mread32 0xF408017C]
  mww 0xF408017C [expr $accesskey]
  mww [expr $addr] [expr $value]

  #puts [format "Access key %x" $accesskey]
  #puts [format "Writing %x to %x" $value $addr]
}

# A9 cannot access netX part per default, this firewalls need to be disabled from 
# R7 side
proc disable_firewalls { } {
	puts "Disabling firewalls"
  # Setup DAP to use CPU interface and not APB
  dap apsel 1

  set_firewall 0xf40801b0 0xFF
  set_firewall 0xf40801b4 0xFF
  set_firewall 0xf40801b8 0xFF
  set_firewall 0xf40801bc 0xFF
  set_firewall 0xf40801c0 0xFF
  set_firewall 0xf40801c4 0xFF
  set_firewall 0xf40801c8 0xFF
  set_firewall 0xf40801cc 0xFF
  set_firewall 0xf40801d0 0xFF
  set_firewall 0xf40801d4 0xFF

  puts "Firewalls disabled!"
}

proc reset_netx4000 {} {

	puts "+reset_netx4000"
	
	reset_config srst_only separate
	#reset_config trst_and_srst separate
	adapter_nsrst_delay 500
	adapter_nsrst_assert_width 50
	
	netx4000.r7 configure -event reset-init {
		puts "Halt R7"
		halt
		# firewalls must be disabled, otherwise reading address 0 (chip type identification) will fail.
		disable_firewalls
	}
	
	netx4000.a9_0 configure -event reset-init {
		puts "Halt A9/0"
		halt
	}
	
	netx4000.a9_0 configure -event reset-init {
		puts "Halt A9/1"
		halt
	}
	
	puts "init"
	init
	
	puts "dap apid 0"
	dap apid 0
	puts "dap apid 1"
	dap apid 1
	puts "dap apid 2"
	dap apid 2
	
	puts "dap info"
	dap info
	puts "dap memaccess"
	dap memaccess
	puts "dap apcsw 1"
	dap apcsw 1

	
	puts "reset init"
	reset init
	
	targets netx4000.r7
	
	puts "-reset_netx4000"
}

echo "Done loading jtag_detect_init.tcl"