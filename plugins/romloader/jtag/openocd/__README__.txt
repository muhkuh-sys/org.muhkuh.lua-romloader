This is a small notepad for my plans to update the stone-old openocd copy in this folder.

Years ago I copied a snapshot of openocd here, this was something 0.2.0 like. I
applied some modifications and attached it to the muhkuh plugin routines. It
worked somehow, but it would be great to get this more flexible in several points:

 * Use the standard static openocd library.
 * Use the official openocd source distribution.
   NOTE: It is not possible to use the pre-build binaries from http://www.freddiechopin.info for windows. This does not provide the static library.


Building under linux:

Our main target is a FTDI based device. OpenOCD provides 3 ways to access the chip (taken from configure output):

  --enable-ft2232_libftdi Enable building support for FT2232 based devices
                          using the libftdi driver, opensource alternate of
                          FTD2XX
  --enable-ft2232_ftd2xx  Enable building support for FT2232 based devices
                          using the FTD2XX driver from ftdichip.com
  --enable-ftdi           Enable building support for the MPSSE mode of FTDI
                          based devices, using libusb-1.0 in asynchronous mode

I'm not quite sure which driver to pick.

The commercial version (--enable-ft2232_ftd2xx) showed good performance and stability in the past.
The open source alternative (--enable-ft2232_libftdi) had a not-so-good performance, but the last check was some time ago.
The MPSSE driver (--enable-ftdi) is completely new for me. Maybe it can even run with libusbx. This would reduce the number of dependencies. libusbx is already used for the usb plugin.


Test 1: Try to build openocd with --enable-ftdi and run some upload/download tests.

Build commands:

 ./configure --enable-ftdi
 make
 sudo make install

Connect to netX500 with:

 openocd --file interface/ftdi/jtagkey.cfg --file board/hilscher_nxhx500.cfg
 
 Problem 1:
   Error: unable to open ftdi device with vid 0403, pid cff8, description 'Amontec JTAGkey' and serial '*'
  Maybe the permissions? Yes:
   ls -l /dev/bus/usb/003/004
   crw-rw-r-- 1 root root 189, 259 Sep  7 12:13 /dev/bus/usb/003/004

  Solution: install the UDEV rule:
    In the source archive are predefined udev rules. Copy the file contrib/openocd.udev to /etc/udev/rules.d:
      sudo install --mode=644 contrib/openocd.udev /etc/udev/rules.d/90-openocd.rules

 Problem 2:
   Error: An adapter speed is not selected in the init script. Insert a call to adapter_khz or jtag_rclk to proceed.
   
   Solution: update the hilscher board file:
--- hilscher_nxhx500.cfg.org 2013-09-07 12:34:30.902970792 +0200
+++ hilscher_nxhx500.cfg 2013-09-07 12:35:12.275175940 +0200
@@ -5,6 +5,7 @@
 source [find target/hilscher_netx500.cfg]
 
 reset_config trst_and_srst
+adapter_khz 6000
 adapter_nsrst_delay 500
 jtag_ntrst_delay 500
 
@@ -17,26 +18,7 @@
   arm7_9 dcc_downloads enable
 
   sleep 100
-
-  sdram_fix
-
-  puts "Configuring SDRAM controller for MT48LC2M32 (8MB) "
-  mww 0x00100140 0
-  mww 0x00100144 0x03C23251
-  mww 0x00100140 0x030D0001
-
-  puts "Configuring SRAM nCS0 for 90ns Par. Flash (x16)"
-  mww 0x00100100 0x01010008
-
-  flash probe 0
 }
 
-#####################
-# Flash configuration
-#####################
-
-#flash bank <name> <driver> <base> <size> <chip width> <bus width> <target#>
-flash bank parflash cfi 0xC0000000 0x01000000 2 2 $_TARGETNAME
-
 init
 reset init


 Now it works!
 Connect with telnet:
 
   telnet 127.0.0.1 4444
 