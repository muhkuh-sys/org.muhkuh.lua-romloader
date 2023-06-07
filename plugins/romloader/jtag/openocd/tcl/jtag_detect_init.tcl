puts "loading script jtag_detect_init.tcl"

global _NETX90_EARLY_BP_
set _NETX90_EARLY_BP_ false
# Uncomment the following line in order to halt the netx 90 at 0x170 after a reset.
# set _NETX90_EARLY_BP_ true

puts "_NETX90_EARLY_BP_: $_NETX90_EARLY_BP_"

# Set the adapter speed.
# Use the value of __JTAG_FREQUENCY_KHZ__, if it is different from 0,
# otherwise use speed_khz.
proc set_adapter_speed_khz {speed_khz} {
	global __JTAG_FREQUENCY_KHZ__
	if {$__JTAG_FREQUENCY_KHZ__ == 0} {
		adapter speed $speed_khz
	} else {
		adapter speed $__JTAG_FREQUENCY_KHZ__
	}
}


# 0: HardReset
# 1: SoftReset
# 2: Attach

global __JTAG_RESET__

if { $__JTAG_RESET__ == 0 } { 
	puts "__JTAG_RESET__: $__JTAG_RESET__ (Hardware reset)"
} elseif { $__JTAG_RESET__ == 1} {
	puts "__JTAG_RESET__: $__JTAG_RESET__ (Software reset)"
} elseif { $__JTAG_RESET__ == 2} {
	puts "__JTAG_RESET__: $__JTAG_RESET__ (Attach and halt)"
} else {
	puts "__JTAG_RESET__: $__JTAG_RESET__"
	puts "WARNING: __JTAG_RESET__ has an unknown value"
}

source [find netx_swreset.tcl]


# todo/wishlist:
# Get the list of known interfaces from the script. Currently, it's hardcoded.
# Get the list of known targets for an NXHX interface. Currently, all possible targets are tried on each interface found.
# Is it possible to recognize the NXHX boards by their description string directly?

# ###################################################################
#   Init/probe for JTAG interfaces
# ###################################################################

# By default, configure the adapter to 6 MHz eventually.
set ADAPTER_MAX_KHZ 6000

proc setup_interface_nxjtag_4000_usb {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NXJTAG-4000-USB"
	ftdi_vid_pid 0x1939 0x0301
	set_adapter_speed_khz 1000

	ftdi_layout_init 0x1B08 0x1C0B
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200
	ftdi_layout_signal JSEL1 -data 0x0400 -oe 0x0400
	ftdi_layout_signal VODIS -data 0x0800 -oe 0x0800
	ftdi_layout_signal VOSWI -data 0x1000 -oe 0x1000
}

proc setup_interface_nxhx_generic {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_vid_pid 0x0640 0x0028
	set_adapter_speed_khz 1000
	
	global ADAPTER_MAX_KHZ
	set ADAPTER_MAX_KHZ 2000
	
	ftdi_layout_init 0x0108 0x010b
	ftdi_layout_signal nTRST -data 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200
}

# hilscher_nxhx_onboard.tcl:
#interface ftdi
#ftdi_vid_pid 0x0640 0x0028
#
#ftdi_layout_init 0x0308 0x030b
#ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
#ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200


proc setup_interface_nxhx90_jtag {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NXHX 90-JTAG"
	ftdi_vid_pid 0x1939 0x002C
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200 
}

proc setup_interface_nxhx90_mc {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	# Skip the device_desc check. It is not necessary
	# and this way, exisiting boards with the string 
	# "NXHX 90-DRIVE" will be recognized as well.
	# ftdi_device_desc "NXHX 90-MC"
	ftdi_vid_pid 0x1939 0x0031
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200 
}

proc setup_interface_nxeb90_spe {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NXEB 90-SPE"
	ftdi_vid_pid 0x1939 0x0032
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200 
}

proc setup_interface_nshield90 {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NSHIELD 90"
	ftdi_vid_pid 0x1939 0x0034
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200 
}

proc setup_interface_nrpeb_h90_re {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NRPEB H90-RE"
	ftdi_vid_pid 0x1939 0x0029
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200 
}

proc setup_interface_nxjtag_usb {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "NXJTAG-USB"
	ftdi_vid_pid 0x1939 0x0023
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0308 0x000b
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200
}

# Display name/device description:
# Olimex OpenOCD JTAG TINY
# Device description from bus:
# Olimex OpenOCD JTAG ARM-USB-TINY-H
proc setup_interface_olimex_arm_usb_tiny_h {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "Olimex OpenOCD JTAG ARM-USB-TINY-H"
	ftdi_vid_pid 0x15ba 0x002a
	set_adapter_speed_khz 1000
	
	ftdi_layout_init 0x0808 0x0a1b
	ftdi_layout_signal nSRST -oe 0x0200
	ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100
	ftdi_layout_signal LED -data 0x0800  
}

# Amontec_JTAGkey
proc setup_interface_jtagkey {strLocation} {
	adapter driver ftdi
	adapter usb location $strLocation
	transport select jtag
	ftdi_device_desc "Amontec JTAGkey"
	ftdi_vid_pid 0x0403 0xcff8
	set_adapter_speed_khz 1000
	
	global ADAPTER_MAX_KHZ
	set ADAPTER_MAX_KHZ 2000
	
	ftdi_layout_init 0x0c08 0x0f1b
	ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400
	ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800
}


# Segger J-Link
proc setup_interface_jlink {} {
	adapter driver jlink
	transport select jtag
	set_adapter_speed_khz 1000
}


# Configure an interface.
proc setup_interface {strInterfaceID strLocation} {
	echo "+setup_interface $strInterfaceID $strLocation"

	# Disable all servers.
	gdb_port disabled
	tcl_port disabled
	telnet_port disabled

	if       {$strInterfaceID == "NXJTAG-USB"}            {setup_interface_nxjtag_usb $strLocation
	} elseif {$strInterfaceID == "Olimex_ARM_USB_TINY_H"} {setup_interface_olimex_arm_usb_tiny_h $strLocation
	} elseif {$strInterfaceID == "Amontec_JTAGkey"}       {setup_interface_jtagkey $strLocation
	} elseif {$strInterfaceID == "NXHX_500_50_51_10"}     {setup_interface_nxhx_generic $strLocation
	} elseif {$strInterfaceID == "NXHX_90-JTAG"}          {setup_interface_nxhx90_jtag $strLocation
	} elseif {$strInterfaceID == "NXHX_90-MC"}            {setup_interface_nxhx90_mc $strLocation
	} elseif {$strInterfaceID == "NXEB_90-SPE"}           {setup_interface_nxeb90_spe $strLocation
	} elseif {$strInterfaceID == "NSHIELD_90"}            {setup_interface_nshield90 $strLocation
	} elseif {$strInterfaceID == "NXJTAG-4000-USB"}       {setup_interface_nxjtag_4000_usb $strLocation
	} elseif {$strInterfaceID == "NRPEB_H90-RE"}          {setup_interface_nrpeb_h90_re $strLocation
	} elseif {$strInterfaceID == "J-Link"}                {setup_interface_jlink
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


# ###################################################################
#   Setup/Probe CPU
# ###################################################################

proc probe_cpu {strCpuID} {
	echo "+probe_cpu $strCpuID"
	global SC_CFG_RESULT
	global __JTAG_RESET__
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
			if { $__JTAG_RESET__==1 } {
				netX_ARM966.cpu configure -event reset-assert { reset_assert }
				netX_ARM966.cpu configure -event reset-deassert-post { reset_deassert_post }
			}
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
			if { $__JTAG_RESET__==1 } {
				netX_ARM926.cpu configure -event reset-assert { reset_assert }
				netX_ARM926.cpu configure -event reset-deassert-post { reset_deassert_post }
			}
			netX_ARM926.cpu configure -event reset-init { echo {netX_ARM926.cpu  reset-init event}; halt }
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
			dap create dap_netx4000r7 -chain-position netx4000.dap
			target create netx4000.r7 cortex_r4 -dap dap_netx4000r7 -coreid 0 -dbgbase 0x80130000
			netx4000.r7 configure -work-area-phys 0x05080000 -work-area-size 0x4000 -work-area-backup 1
			netx4000.r7 configure -event reset-assert-post "cortex_r4 dbginit"
			if { $__JTAG_RESET__==1 } {
				netx4000.r7 configure -event reset-assert { reset_assert_netx4000 }
				netx4000.r7 configure -event reset-deassert-post { reset_deassert_post_netx4000 }
			}			
			netx4000.r7 configure -event reset-init { echo {netx 4000 R7 reset-init event}; halt }
			
			# Dual Cortex A9s
			# target create netx4000.a9_0 cortex_a -chain-position netx4000.dap -coreid 1 -dbgbase 0x80110000
			# netx4000.a9_0 configure -work-area-phys 0x05000000 -work-area-size 0x4000 -work-area-backup 1
			# target create netx4000.a9_1 cortex_a -chain-position netx4000.dap -coreid 2 -dbgbase 0x80112000
			# netx4000.a9_1 configure -work-area-phys 0x05004000 -work-area-size 0x4000 -work-area-backup 1
			# 
			# netx4000.a9_0 configure -event reset-assert-post "cortex_a dbginit"
			# netx4000.a9_1 configure -event reset-assert-post "cortex_a dbginit"
			
			# Only reset r7 (master CPU)
			# netx4000.a9_0 configure -event reset-assert ""
			# netx4000.a9_1 configure -event reset-assert ""
			
			# target smp netx4000.a9_0 netx4000.a9_1
			
			# Select r7 as default target
			targets netx4000.r7
			
			global strTarget
			set strTarget netX4000
		}
		
	# netx 90 MPW 
	} elseif { $strCpuID == "netX90_MPW_COM" } {
		echo "+ probe_cpu netX90_MPW_COM"
		jtag newtap netx90 dap -expected-id 0x6ba00477 -irlen 4
		jtag newtap netx90 tap -expected-id 0x102046ad -irlen 4
		jtag configure netx90.dap -event setup { global SC_CFG_RESULT ; echo {Yay - setup netx 90} ; set SC_CFG_RESULT {OK} }
		jtag init
	
		if { $SC_CFG_RESULT=={OK} } {
			dap create netx90com -chain-position netx90.dap
			target create netx90.comm cortex_m -dap netx90com -coreid 0 -ap-num 2
			netx90.comm configure -event reset-init { halt }
			netx90.comm configure -work-area-phys 0x00040000 -work-area-size 0x4000 -work-area-backup 1
			
			global strTarget
			set strTarget netx90_MPW_COM
		}
		
	# netx 90
	} elseif { $strCpuID == "netX90_COM" } {
		echo "+ probe_cpu netX90_COM"
		adapter speed 50
		jtag newtap netx90 dap -expected-id 0x6ba00477 -irlen 4
		jtag newtap netx90 tap -expected-id 0x10a046ad -irlen 4
		jtag configure netx90.dap -event setup { global SC_CFG_RESULT ; echo {Yay - setup netx 90} ; set SC_CFG_RESULT {OK} }
		jtag init

		if { $SC_CFG_RESULT=={OK} } {
			dap create netx90com -chain-position netx90.dap
			target create netx90.comm cortex_m -dap netx90com -coreid 0 -ap-num 2
			netx90.comm configure -event reset-init { halt }
			netx90.comm configure -work-area-phys 0x00040000 -work-area-size 0x4000 -work-area-backup 1

			global strTarget
			set strTarget netx90_COM
		}
	
	# netIOL
	} elseif { $strCpuID == "netIOL"} {
		echo "+ probe_cpu netIOL"
		set_adapter_speed_khz 1000
		
		jtag newtap netIOL cpu -expected-id 0x101026ad -irlen 4 -ircapture 0x1 -irmask 0xf 
		jtag configure netIOL.cpu -event setup { global SC_CFG_RESULT ; echo {Yay - setup netIOL} ; set SC_CFG_RESULT {OK} }
		jtag init

		if { $SC_CFG_RESULT=={OK} } {
			target create netIOL.cpu hinetiol -endian little -chain-position netIOL.cpu
			netIOL.cpu configure -event reset-init { halt }
			# netIOL.cpu configure -work-area-phys 0x00040000 -work-area-size 0x4000 -work-area-backup 1
			# netiol.cpu configure -work-area-virt 0x6000 -work-area-phys 0x6000 -work-area-size 0x2000 -work-area-backup 1
			
			global strTarget
			set strTarget netIOL
		}
	}
	
	echo "- probe_cpu $SC_CFG_RESULT"
	return $SC_CFG_RESULT
}

# ###################################################################
#    Reset board
# ###################################################################

proc mread32 {addr} {
  set value(0) 0
  mem2array value 32 $addr 1
  return $value(0)
}

proc peek { strFmt ulAddr } {
	set ulVal [ mread32 $ulAddr ]
	puts [ format $strFmt $ulVal ]
	return $ulVal
}


# todo: pass target name from plugin 
proc reset_board {} {
	global strTarget
	echo "+ reset_board  target: $strTarget"
	if { $strTarget == "netX4000" } then {
		reset_netx4000
	} elseif { $strTarget == "netx90_MPW_COM" } then {
		reset_netx90_MPW_COM
	} elseif { $strTarget == "netx90_COM" } then {
		reset_netx90_COM
	} elseif { $strTarget == "netIOL" } then {
		reset_netIOL
	} else {
		reset_netX_ARM926_ARM966
	}
	echo "- reset_board "
}

proc reset_netIOL {} {
	reset_config none
	init 
	halt
}

proc reset_netx90_MPW_COM {} {
	global __JTAG_RESET__


	if { $__JTAG_RESET__==2 } {
		init
		halt
	} else {
		# if srst is not fitted use SYSRESETREQ to perform a soft reset
		cortex_m reset_config sysresetreq

		init
		reset init
	}
}


proc reset_netX_ARM926_ARM966 {} {
	global __JTAG_RESET__

	switch $__JTAG_RESET__ {
		0 {
			echo "Using hardware reset (SRST)"
			reset_config trst_and_srst
			adapter srst delay 500
			jtag_ntrst_delay 500

			init
			reset init
		}
		1 {
			echo "Using software reset"
			reset_config trst_only
			jtag_ntrst_delay 500

			init
			reset init
		}
		2 {
			init
			halt
		}
	}
}

# ###################################################################
#    Init/reset netX 90
# ###################################################################

proc netx90_unlock_write_reg {addr value} {
  set accesskey [mread32 0xff4012c0]
  mww 0xff4012c0 [expr $accesskey]
  mww [expr $addr] [expr $value]
}

# Set the analog parameters to default values.
proc netx90_setup_analog {}  {
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

	# Lock the analog parameters.
	mww 0xff0016cc 0x00000004
}

# Stop xPEC/xPIC units.
proc netx90_stop_xpec_xpic {} {
	puts "Stopping xPEC/xPIC"
	mww 0xff111d70 0x00ff0000 ;# xc_start_stop_ctrl
	mww 0xff184080 1          ;# xpic_hold_pc (com)
	# mww 0xff884080 1          ;# xpic_hold_pc (app) - not reachable via COM CPU
}


proc netX90_COM_disable_irqs {} {
	# disable all IRQs (NXTFLASHER-490)
	
	echo "Disable all IRQs"
	# more information about the cm4 register here: https://developer.arm.com/documentation/dui0553/latest/
	
	# Interrupt Active Bit Registers (see documentation 4.2.6)							  						  
	set ADR_cm4_scs_nvic_iabr0 0xe000e300
	set ADR_cm4_scs_nvic_iabr1 0xe000e304
	set ADR_cm4_scs_nvic_iabr2 0xe000e308
	# interrupt clear enable register (see documentation 4.2.3)							  						  
	set ADR_cm4_scs_nvic_icer0 0xe000e180
	set ADR_cm4_scs_nvic_icer1 0xe000e184
	set ADR_cm4_scs_nvic_icer2 0xe000e188
	# interrupt clear pending register (see documentation 4.2.5)
	set ADR_cm4_scs_nvic_icpr0 0xe000e280
	set ADR_cm4_scs_nvic_icpr1 0xe000e284
	set ADR_cm4_scs_nvic_icpr2 0xe000e288
	# interrupt control and state register (see documentation 4.3.3)										
	set ADR_cm4_scs_icsr 0xe000ed04
	# Application Interrupt and Reset Control Register (see documentation 4.3.5)	
	set ADR_cm4_scs_aircr 0xe000ed0c
	
	
	# read the state of the nvic irqs before disabling them
	puts "active IRQ register before clear"
	puts "--------------------------------"
	
	set ulVal_cm4_scs_nvic_iabr0 [ mread32 $ADR_cm4_scs_nvic_iabr0 ]
	set ulVal_cm4_scs_nvic_iabr1 [ mread32 $ADR_cm4_scs_nvic_iabr1 ]
	set ulVal_cm4_scs_nvic_iabr2 [ mread32 $ADR_cm4_scs_nvic_iabr2 ]
	
	puts [format "cm4_scs_nvic_iabr0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_iabr0 $ulVal_cm4_scs_nvic_iabr1 $ulVal_cm4_scs_nvic_iabr2]

	set ulVal_cm4_scs_nvic_icer0 [ mread32 $ADR_cm4_scs_nvic_icer0 ]
	set ulVal_cm4_scs_nvic_icer1 [ mread32 $ADR_cm4_scs_nvic_icer1 ]
	set ulVal_cm4_scs_nvic_icer2 [ mread32 $ADR_cm4_scs_nvic_icer2 ]

	puts [format "cm4_scs_nvic_icer0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_icer0 $ulVal_cm4_scs_nvic_icer1 $ulVal_cm4_scs_nvic_icer2]

	set ulVal_cm4_scs_nvic_icpr0 [ mread32 $ADR_cm4_scs_nvic_icpr0 ]
	set ulVal_cm4_scs_nvic_icpr1 [ mread32 $ADR_cm4_scs_nvic_icpr1 ]
	set ulVal_cm4_scs_nvic_icpr2 [ mread32 $ADR_cm4_scs_nvic_icpr2 ]

	puts [format "cm4_scs_nvic_icpr0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_icpr0 $ulVal_cm4_scs_nvic_icpr1 $ulVal_cm4_scs_nvic_icpr2]

	peek "ADR_cm4_scs_icsr:        0x%08x" $ADR_cm4_scs_icsr
	peek "ADR_cm4_scs_aircr:       0x%08x" $ADR_cm4_scs_aircr
	puts ""

	puts "clear all enable irq register (disable all)"
	# clear all enable irq-register (disable all)
	mww $ADR_cm4_scs_nvic_icer0 0xFFFFFFFF
	mww $ADR_cm4_scs_nvic_icer1 0xFFFFFFFF
	mww $ADR_cm4_scs_nvic_icer2 0xFFFFFFFF
	
	puts "clear all pending irq register"
	# clear all pending irq-register
	mww $ADR_cm4_scs_nvic_icpr0 0xFFFFFFFF
	mww $ADR_cm4_scs_nvic_icpr1 0xFFFFFFFF
	mww $ADR_cm4_scs_nvic_icpr2 0xFFFFFFFF
	
	puts ""
	
	# read the state of the nvic irqs after disabling them									 												 														
	puts "active IRQ register after clear"
	puts "-------------------------------"
	set ulVal_cm4_scs_nvic_iabr0 [ mread32 $ADR_cm4_scs_nvic_iabr0 ]
	set ulVal_cm4_scs_nvic_iabr1 [ mread32 $ADR_cm4_scs_nvic_iabr1 ]
	set ulVal_cm4_scs_nvic_iabr2 [ mread32 $ADR_cm4_scs_nvic_iabr2 ]
	
	puts [format "cm4_scs_nvic_iabr0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_iabr0 $ulVal_cm4_scs_nvic_iabr1 $ulVal_cm4_scs_nvic_iabr2]

	set ulVal_cm4_scs_nvic_icer0 [ mread32 $ADR_cm4_scs_nvic_icer0 ]
	set ulVal_cm4_scs_nvic_icer1 [ mread32 $ADR_cm4_scs_nvic_icer1 ]
	set ulVal_cm4_scs_nvic_icer2 [ mread32 $ADR_cm4_scs_nvic_icer2 ]

	puts [format "cm4_scs_nvic_icer0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_icer0 $ulVal_cm4_scs_nvic_icer1 $ulVal_cm4_scs_nvic_icer2]

	set ulVal_cm4_scs_nvic_icpr0 [ mread32 $ADR_cm4_scs_nvic_icpr0 ]
	set ulVal_cm4_scs_nvic_icpr1 [ mread32 $ADR_cm4_scs_nvic_icpr1 ]
	set ulVal_cm4_scs_nvic_icpr2 [ mread32 $ADR_cm4_scs_nvic_icpr2 ]

	puts [format "cm4_scs_nvic_icpr0|1|2:  0x%08x|0x%08x|0x%08x" $ulVal_cm4_scs_nvic_icpr0 $ulVal_cm4_scs_nvic_icpr1 $ulVal_cm4_scs_nvic_icpr2]

	peek "ADR_cm4_scs_icsr:        0x%08x" $ADR_cm4_scs_icsr
	peek "ADR_cm4_scs_aircr:       0x%08x" $ADR_cm4_scs_aircr
	puts ""
				
}

proc reset_netx90_COM {} {
    global __JTAG_RESET__
    global _NETX90_EARLY_BP_

    # Wait up to 2000 ms for the netx to halt after a 
    # halt request or to hit a breakpoint.
    set HALT_TIMEOUT 2000
		
    if { $__JTAG_RESET__==2 } {
        # Allow the ROM code to copy the UUID from the info page
        # to the UUID registers.
        sleep 100
        
        init
        
        puts "Trying to halt the CPU."
        if {[catch {halt $HALT_TIMEOUT} err] == 0} {

            # The COM CPU is now halted.
            # Initialize the chip.
            # We assume that
            # - The analog parameters have been set up 
            # - some xpec/xPIC units may be running, so we stop them.
            puts "===================================================================="
            puts "Warning: CPU halted in undefined state. This is not reliable."
            puts "       Notice: Use console mode if possible."
            puts "===================================================================="

            set CPU_HALTED true

            netx90_stop_xpec_xpic
            # netx90_setup_analog
        
        	# disable all irqs if the netX is in a undefined state to prevent execution of irqs
			netX90_COM_disable_irqs
			
            # We assume that the PLL is configured. Set the JTAG frequency to 1 MHz.
            set_adapter_speed_khz 1000

        } else {
            # The COM CPU wasn't halted.
            puts "===================================================================="
            puts "Timed out while waiting for halt."
            puts "ERROR: Failed to halt the CPU"
            puts "===================================================================="

            shutdown error
        }

    } else {
        puts "+reset_netx90_COM"
        if { $__JTAG_RESET__==1 } {
            echo "Using software reset"
            reset_config none
            #reset_config trst_only
            jtag_ntrst_delay 500
            cortex_m reset_config sysresetreq
        } else {
            echo "Using hardware reset"
            #reset_config trst_and_srst
            reset_config srst_only 
            #reset_config trst_only
            adapter srst delay 500
            jtag_ntrst_delay 500
        }
    

        # This breakpoint should be hit when the ROM code enables debugging, 
        # either when processing an exec chunk or when entering the console mode.
        set BP_ADDR_APP_JTAG_ENABLED_RC5 0x1729e
        set BP_ADDR_APP_JTAG_ENABLED_RC6 0x170a2

        # The ROM code always hits this breakpoint, but it is before any initializations have been made. 
        # We want to use this one only when the netX 90 is in test mode.
        set BP_ADDR_ROM_START 0x170;  # Same for RC5 and RC6

        init
    
        global ROMLOADER_CHIPTYP_NETX90
        global ROMLOADER_CHIPTYP_NETX90B
        global ROMLOADER_CHIPTYP_NETX90C
        global ROMLOADER_CHIPTYP_NETX90D

        set iChiptyp [ get_chiptyp ]
        

        # This is a breakpoint that is hit when the CPU enables debugging
        # when entering the console or when processing an EXEC chunk.
        if {$iChiptyp == $ROMLOADER_CHIPTYP_NETX90} {
            set BP_ADDR_APP_JTAG_ENABLED $BP_ADDR_APP_JTAG_ENABLED_RC5
        } elseif {$iChiptyp == $ROMLOADER_CHIPTYP_NETX90B 
                  || $iChiptyp == $ROMLOADER_CHIPTYP_NETX90C 
                  || $iChiptyp == $ROMLOADER_CHIPTYP_NETX90D} {
            set BP_ADDR_APP_JTAG_ENABLED $BP_ADDR_APP_JTAG_ENABLED_RC6
        } else {
            puts "===================================================================="
            puts "Error: Could not detect the chip type."
            puts "The debug access port might be disabled."
            puts "===================================================================="

            shutdown error
        }

        set ADDR_RESET_CTRL 0xff0016b0
    
        if {[catch {halt $HALT_TIMEOUT} err] == 0} {
            peek "reset_ctrl: 0x%08x" $ADDR_RESET_CTRL
            puts "Clear reset_ctrl"
            netx90_unlock_write_reg 0xff0016b0 0x1ff
            peek "reset_ctrl: 0x%08x" $ADDR_RESET_CTRL
            resume

        } else {
            # The COM CPU wasn't halted.
            puts "===================================================================="
            puts "Timed out while waiting for halt."
            puts "ERROR: Failed to halt the CPU"
            puts "===================================================================="
            shutdown error
        }

        set CPU_HALTED false

        if {$_NETX90_EARLY_BP_ == false} {
            puts "Trying to halt the CPU in ROM breakpoint 1"

            bp $BP_ADDR_APP_JTAG_ENABLED 2 hw
            reset
        
            if {[catch {wait_halt $HALT_TIMEOUT} err] == 0} {
                # The COM CPU is now halted.
                # Remove the breakpoint.
                # We assume that 
                # - the chip has been reset and all other masters (xPEC, xPIC...) are stopped
                # - the analog parameters have been configured by the ROM code.
                # Therefore, we don't need to do any further initialization here.
                puts "CPU halted in ROM breakpoint 1"

                set CPU_HALTED true
            
                # Remove the breakpoint.
                rbp $BP_ADDR_APP_JTAG_ENABLED

                # The PLL is configured. Set the JTAG frequency to 1 MHz.
                set_adapter_speed_khz 1000
    
            } else {
      
                # The COM CPU wasn't halted.
                puts "Timed out while waiting for ROM breakpoint 1."

                # Remove the breakpoint.
                rbp $BP_ADDR_APP_JTAG_ENABLED
            }
        } else {

            puts "Trying to halt the CPU in ROM breakpoint 2 (early startup)."
        
            # Set a breakpoint to be hit when the chip is in test mode and reset.
            bp $BP_ADDR_ROM_START 2 hw
        
            reset
        
            if {[catch {wait_halt $HALT_TIMEOUT} err] == 0} {
    
                # The COM CPU is now halted.
                # Remove the breakpoint and initialize the chip.
                # We assume that
                # - the chip has been reset and all other masters (xPEC, xPIC...) are stopped
                # - However, the analog parameters have not been set, so we do that here.
                puts "CPU halted in ROM breakpoint 2"

                set CPU_HALTED true

                rbp $BP_ADDR_ROM_START

                netx90_setup_analog
    
                # The PLL is configured now. Set the JTAG frequency to 1 MHz.
                set_adapter_speed_khz 1000
            
            } else {
    
                puts "===================================================================="
                puts "WARNING: Timed out while waiting for ROM breakpoint 2 (early startup)."
                puts "===================================================================="

                # Remove the breakpoint.
                rbp $BP_ADDR_ROM_START
            }
        }

        if {$CPU_HALTED == false} {
            puts "Trying to halt the CPU."
            if {[catch {halt $HALT_TIMEOUT} err] == 0} {

                # The COM CPU is now halted.
                # Initialize the chip.
                # We assume that
                # - The analog parameters have been set up 
                # - some xpec/xPIC units may be running, so we stop them.
                puts "===================================================================="
                puts "ERROR: CPU halted in undefined state. This is not reliable."
                puts "       Notice: Use console mode if possible."
                puts "===================================================================="

                set CPU_HALTED true

                netx90_stop_xpec_xpic
                # netx90_setup_analog
                
                # disable all irqs if the netX is in a undefined state to prevent execution of irqs
                netX90_COM_disable_irqs
            
                # We assume that the PLL is configured. Set the JTAG frequency to 1 MHz.
                set_adapter_speed_khz 1000

            } else {
                # The COM CPU wasn't halted.
                puts "===================================================================="
                puts "Timed out while waiting for halt."
                puts "ERROR: Failed to halt the CPU"
                puts "===================================================================="

                shutdown error
            }
        }
        peek "reset_ctrl: 0x%08x" $ADDR_RESET_CTRL
        puts "-reset_netx90_COM"
    }
}




# ###################################################################
#    Init/reset netx 4000
# ###################################################################

proc set_firewall {addr value} {
  set accesskey [mread32 0xF408017C]
  mww 0xF408017C [expr $accesskey]
  mww [expr $addr] [expr $value]
}

# A9 cannot access netX part per default, this firewalls need to be disabled from 
# R7 side
proc disable_firewalls { } {
	puts "Disabling firewalls"
  # Setup DAP to use CPU interface and not APB
  dap_netx4000r7 apsel 1

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
	global __JTAG_RESET__
	if { $__JTAG_RESET__==2 } {
		init
		halt

		dap_netx4000r7 memaccess
		dap_netx4000r7 apcsw 0x40000000
	} else {
		puts "+reset_netx4000"
	
		if { $__JTAG_RESET__==1 } {
			echo "Using software reset"
			reset_config none
		} else {
			echo "Using hardware reset (SRST)"
			reset_config srst_only separate
			adapter srst delay 500
			adapter srst pulse_width 50
		}


		# netx4000.r7 configure -event reset-init {
		# 	puts "Halt R7"
		# 	halt
		# 	# firewalls must be disabled, otherwise reading address 0 (chip type identification) will fail.
		# 	disable_firewalls
		# }
	
		# netx4000.a9_0 configure -event reset-init {
		# 	puts "Halt A9/0"
		# 	halt
		# }
	
		# netx4000.a9_1 configure -event reset-init {
		# 	puts "Halt A9/1"
		# 	halt
		# }
	
		puts "init"
		init
	
		# puts "dap apid 0"
		# dap apid 0
		# puts "dap apid 1"
		# dap apid 1
		# puts "dap apid 2"
		# dap apid 2
	
		# puts "dap info"
		# dap info
		puts "dap memaccess"
		dap_netx4000r7 memaccess

		puts "dap apcsw"
		dap_netx4000r7 apcsw 0x40000000
	
		# The CPU cannot be stopped at the first reset vector catch after a reset. 
		puts "reset run" 
		reset run
	}

	targets netx4000.r7
	
	puts "Halt R7"
	halt
	# firewalls must be disabled, otherwise reading address 0 (chip type identification) will fail.
	disable_firewalls
	
	netx4000_enable_tcm	
	
	puts "-reset_netx4000"
}

# enable ITCM+DTCM and set reset vector at start of ITCM
# TRM chapter 4.3.13
# 
# MRC    p15,    0,   <Rd>, c9,  c1,  0
# MRC coproc,  op1, <Rd>, CRn, CRm, op2
# -> arm mrc coproc op1 CRn CRm op2

# MCR p15,      0, <Rd>, c9,  c1,  0
# MCR coproc, op1, <Rd>, CRn, CRm, op2
# -> arm mcr coproc op1 CRn CRm op2 value

# http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0458c/CHDEFBFI.html

proc netx4000_enable_tcm {} {
	set __ITCM_START_ADDRESS__         0x00000000
	set __DTCM_START_ADDRESS__         0x00020000
			
	set MSK_CR7_CP15_ITCMRR_Enable     0x00000001
	set SRT_CR7_CP15_ITCMRR_Enable     0
	
	set MSK_CR7_CP15_ITCMRR_Size        0x0000003c
	set SRT_CR7_CP15_ITCMRR_Size        2
	set VAL_CR7_CP15_ITCMRR_Size_128KB  8
	
	set MSK_CR7_CP15_DTCMRR_Enable     0x00000001
	set SRT_CR7_CP15_DTCMRR_Enable     0
	
	set MSK_CR7_CP15_DTCMRR_Size       0x0000003c
	set SRT_CR7_CP15_DTCMRR_Size       2
	set VAL_CR7_CP15_DTCMRR_Size_128KB 8
	
	set ulItcm [expr $__ITCM_START_ADDRESS__  | $MSK_CR7_CP15_ITCMRR_Enable | ( $VAL_CR7_CP15_ITCMRR_Size_128KB << $SRT_CR7_CP15_ITCMRR_Size ) ]
	set ulDtcm [expr $__DTCM_START_ADDRESS__  | $MSK_CR7_CP15_DTCMRR_Enable | ( $VAL_CR7_CP15_DTCMRR_Size_128KB << $SRT_CR7_CP15_DTCMRR_Size ) ]

	puts "netx 4000 Enable ITCM/DTCM"
	puts [ format "ulItcm: %08x" $ulItcm ]
	puts [ format "ulDtcm: %08x" $ulDtcm ]
	
	arm mcr 15 0 9 1 1 $ulItcm
	arm mcr 15 0 9 1 0 $ulDtcm
	
	puts "Set reset vector in ITCM"
	mww 0 0xE59FF00C
	mdw 0
}


echo "Done loading jtag_detect_init.tcl"

