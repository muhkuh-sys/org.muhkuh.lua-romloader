cmake_minimum_required(VERSION 3.7)

PROJECT("openocd"
        VERSION ${VERSION_EXTERNAL})

ADD_SUBDIRECTORY(jimtcl)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckStructHasMember)
INCLUDE(CheckTypeSize)


FIND_PACKAGE("libusb" REQUIRED)


#----------------------------------------------------------------------------
#
# Get all defines for config.h.in .
#
SET(BUILD_AICE 1)
SET(BUILD_FTDI 1)
SET(BUILD_HLADAPTER 1)
SET(BUILD_HLADAPTER_ICDI 1)
SET(BUILD_HLADAPTER_STLINK 1)
SET(BUILD_JLINK 1)
SET(BUILD_OPENDOUS 1)
SET(BUILD_OSBDM 1)
SET(BUILD_ULINK 1)
SET(BUILD_USB_BLASTER_2 0)
SET(BUILD_VSLLINK 0)

CHECK_INCLUDE_FILES(arpa/inet.h HAVE_ARPA_INET_H)
CHECK_INCLUDE_FILES(dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(elf.h HAVE_ELF_H)
CHECK_INCLUDE_FILES(fcntl.h HAVE_FCNTL_H)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_INCLUDE_FILES(ifaddrs.h HAVE_IFADDRS_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(jtag_minidriver.h HAVE_JTAG_MINIDRIVER_H)

# We definitely have libusb-1.x with libusb_error_name() .
SET(HAVE_LIBUSB1 1)
SET(HAVE_LIBUSB_ERROR_NAME 1)

CHECK_TYPE_SIZE("long long int" HAVE_LONG_LONG_INT)
CHECK_TYPE_SIZE("unsigned long long int" HAVE_UNSIGNED_LONG_LONG_INT)
CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(netdb.h HAVE_NETDB_H)
CHECK_INCLUDE_FILES(netinet/in.h HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES(netinet/tcp.h HAVE_NETINET_TCP_H)
CHECK_INCLUDE_FILES(net/if.h HAVE_NET_IF_H)
CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_FUNCTION_EXISTS(strndup HAVE_STRNDUP)
CHECK_FUNCTION_EXISTS(strnlen HAVE_STRNLEN)
CHECK_INCLUDE_FILES(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES(sys/poll.h HAVE_SYS_POLL_H)
CHECK_INCLUDE_FILES(sys/select.h HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILES(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_FUNCTION_EXISTS(usleep HAVE_USLEEP)
CHECK_FUNCTION_EXISTS(vasprintf HAVE_VASPRINTF)
CHECK_TYPE_SIZE("_Bool" HAVE__BOOL)

IF(CYGWIN)
	SET(IS_CYGWIN 1)
ENDIF(CYGWIN)

IF(MINGW)
	SET(IS_MINGW 1)
ENDIF(MINGW)

IF(WIN32)
	SET(IS_WIN32 1)
ENDIF(WIN32)

SET(LT_OBJDIR ".libs/")
SET(PARPORT_USE_PPDEV 0)

SET(PACKAGE "openocd")
SET(PACKAGE_BUGREPORT "OpenOCD Mailing List <openocd-devel@lists.sourceforge.net>")
SET(PACKAGE_NAME "openocd")
SET(PACKAGE_STRING "openocd 0.9.0")
SET(PACKAGE_TARNAME "openocd")
SET(PACKAGE_URL "")
SET(PACKAGE_VERSION "0.9.0")
SET(VERSION "0.9.0")

IF(${CMAKE_COMPILER_IS_GNUCC})
	SET(_GNU_SOURCE 1)
ENDIF(${CMAKE_COMPILER_IS_GNUCC})


FILE(READ cmake/compile_test_openocd_std_bool_h.c C_SOURCE_STDBOOL_H_TEST)
CHECK_C_SOURCE_COMPILES("${C_SOURCE_STDBOOL_H_TEST}" HAVE_STDBOOL_H)


FILE(READ cmake/compile_test_time_with_sys_time.c C_COURCE_TIME_WITH_SYS_TIME_TEST)
CHECK_C_SOURCE_COMPILES("${C_COURCE_TIME_WITH_SYS_TIME_TEST}" TIME_WITH_SYS_TIME)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config/config.h)

#----------------------------------------------------------------------------
#
# Build libopenocd.
#


#-----------------------------------------------------------------------------
#
# Hexdump
# TODO: hier eine Funktion draus machen.
#

FUNCTION(OPENOCD_BIN2CHAR HEXDUMP_INPUT_FILE HEXDUMP_OUTPUT_FILE)
	# Read the complete source file, convert it to a hex-dump and store
	# this in a string.
	FILE(READ "${HEXDUMP_INPUT_FILE}" HEXDUMP_CONTENTS_HEX HEX)
	
	# Collect all output lines in this list.
	SET(HEXDUMP_LINES "")
	
	# Split the string into chunks of 16 hex digits. This is 32 bytes per line.
	STRING(LENGTH "${HEXDUMP_CONTENTS_HEX}" HEXDUMP_LENGTH_HEX)
	SET(HEXDUMP_LINESIZE 32)
	SET(HEXDUMP_POSITION 0)
	WHILE(${HEXDUMP_POSITION} LESS ${HEXDUMP_LENGTH_HEX})
		# Get the size of the data chunk for the next line.
		MATH(EXPR HEXDUMP_CHUNK_SIZE "${HEXDUMP_LENGTH_HEX}-${HEXDUMP_POSITION}")
		IF(${HEXDUMP_CHUNK_SIZE} GREATER ${HEXDUMP_LINESIZE})
			SET(HEXDUMP_CHUNK_SIZE ${HEXDUMP_LINESIZE})
		ENDIF(${HEXDUMP_CHUNK_SIZE} GREATER ${HEXDUMP_LINESIZE})
		
		# Extract the line from the file data.
		STRING(SUBSTRING "${HEXDUMP_CONTENTS_HEX}" ${HEXDUMP_POSITION} ${HEXDUMP_CHUNK_SIZE} HEXDUMP_LINE_RAW)
		
		# Convert the plain hexdump in a C array style.
		STRING(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1," HEXDUMP_LINE "${HEXDUMP_LINE_RAW}")
		
		# Move to the next line.
		MATH(EXPR HEXDUMP_POSITION "${HEXDUMP_POSITION}+${HEXDUMP_CHUNK_SIZE}")
		
		LIST(APPEND HEXDUMP_LINES "${HEXDUMP_LINE}\n")
	ENDWHILE(${HEXDUMP_POSITION} LESS ${HEXDUMP_LENGTH_HEX})
	
	FILE(WRITE "${HEXDUMP_OUTPUT_FILE}" ${HEXDUMP_LINES})
ENDFUNCTION(OPENOCD_BIN2CHAR HEXDUMP_INPUT_FILE HEXDUMP_OUTPUT_FILE HEXDUMP_SYMBOL_NAME)


#-----------------------------------------------------------------------------

# Generate a hexdump from the XScale debug handler.
OPENOCD_BIN2CHAR("src/target/xscale/debug_handler.bin" "${CMAKE_CURRENT_BINARY_DIR}/config/xscale_debug.inc" "xscale_debug_handler")



SET(CONCAT_LIST "")
FOREACH(CONCAT_FILENAME IN ITEMS src/helper/startup.tcl src/jtag/startup.tcl src/target/startup.tcl src/flash/startup.tcl src/server/startup.tcl)
	FILE(READ "${CONCAT_FILENAME}" CONCAT_FILE_CONTENTS)
	LIST(APPEND CONCAT_LIST "${CONCAT_FILE_CONTENTS}")
ENDFOREACH(CONCAT_FILENAME)
FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config/startup.tcl" ${CONCAT_LIST})

OPENOCD_BIN2CHAR("${CMAKE_CURRENT_BINARY_DIR}/config/startup.tcl" "${CMAKE_CURRENT_BINARY_DIR}/config/startup_tcl.inc" "openocd_startup_tcl")



# Copy the minidriver to the source folder.
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/jtag/drivers/minidriver_imp.h
     DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/src/jtag)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

SET(SOURCES_libflash
	src/flash/
	src/flash/common.c
	src/flash/mflash.c
	src/flash/nand/arm_io.c
	src/flash/nand/at91sam9.c
	src/flash/nand/core.c
	src/flash/nand/davinci.c
	src/flash/nand/driver.c
	src/flash/nand/ecc.c
	src/flash/nand/ecc_kw.c
	src/flash/nand/fileio.c
	src/flash/nand/lpc3180.c
	src/flash/nand/lpc32xx.c
	src/flash/nand/mx3.c
	src/flash/nand/mxc.c
	src/flash/nand/nonce.c
	src/flash/nand/nuc910.c
	src/flash/nand/orion.c
	src/flash/nand/s3c2410.c
	src/flash/nand/s3c2412.c
	src/flash/nand/s3c2440.c
	src/flash/nand/s3c2443.c
	src/flash/nand/s3c24xx.c
	src/flash/nand/s3c6400.c
	src/flash/nand/tcl.c
	src/flash/nor/aduc702x.c
	src/flash/nor/at91sam3.c
	src/flash/nor/at91sam4.c
	src/flash/nor/at91sam4l.c
	src/flash/nor/at91sam7.c
	src/flash/nor/at91samd.c
	src/flash/nor/avrf.c
	src/flash/nor/cfi.c
	src/flash/nor/core.c
	src/flash/nor/drivers.c
	src/flash/nor/dsp5680xx_flash.c
	src/flash/nor/efm32.c
	src/flash/nor/em357.c
	src/flash/nor/faux.c
	src/flash/nor/fm3.c
	src/flash/nor/kinetis.c
	src/flash/nor/lpc2000.c
	src/flash/nor/lpc288x.c
	src/flash/nor/lpc2900.c
	src/flash/nor/lpcspifi.c
	src/flash/nor/mdr.c
	src/flash/nor/mini51.c
	src/flash/nor/mrvlqspi.c
	src/flash/nor/non_cfi.c
	src/flash/nor/nrf51.c
	src/flash/nor/nuc1x.c
	src/flash/nor/ocl.c
	src/flash/nor/pic32mx.c
	src/flash/nor/psoc4.c
	src/flash/nor/sim3x.c
	src/flash/nor/spi.c
	src/flash/nor/stellaris.c
	src/flash/nor/stm32f1x.c
	src/flash/nor/stm32f2x.c
	src/flash/nor/stm32lx.c
	src/flash/nor/stmsmi.c
	src/flash/nor/str7x.c
	src/flash/nor/str9x.c
	src/flash/nor/str9xpec.c
	src/flash/nor/tcl.c
	src/flash/nor/tms470.c
	src/flash/nor/virtual.c
)


SET(SOURCES_libhelper
	src/helper/binarybuffer.c
	src/helper/command.c
	src/helper/configuration.c
	src/helper/fileio.c
	src/helper/ioutil_stubs.c
	src/helper/jim-nvp.c
	src/helper/log.c
	src/helper/options.c
	src/helper/replacements.c
	src/helper/time_support.c
	src/helper/time_support_common.c
	src/helper/util.c
)


SET(SOURCES_libjtag
	src/jtag/adapter.c
	src/jtag/commands.c
	src/jtag/core.c
	src/jtag/interface.c
	src/jtag/interfaces.c
	src/jtag/tcl.c
	src/jtag/aice/aice_interface.c
	src/jtag/aice/aice_pipe.c
	src/jtag/aice/aice_port.c
	src/jtag/aice/aice_transport.c
	src/jtag/aice/aice_usb.c
	src/jtag/drivers/bitbang.c
	src/jtag/drivers/bitq.c
	src/jtag/drivers/driver.c
	src/jtag/drivers/dummy.c
	src/jtag/drivers/ftdi.c
	src/jtag/drivers/jlink.c
	src/jtag/drivers/jtag_vpi.c
	src/jtag/drivers/libusb1_common.c
	src/jtag/drivers/mpsse.c
	src/jtag/drivers/opendous.c
	src/jtag/drivers/osbdm.c
	src/jtag/drivers/remote_bitbang.c
	src/jtag/drivers/stlink_usb.c
	src/jtag/drivers/ti_icdi_usb.c
	src/jtag/drivers/ulink.c
	src/jtag/hla/hla_interface.c
	src/jtag/hla/hla_layout.c
	src/jtag/hla/hla_tcl.c
	src/jtag/hla/hla_transport.c
)


SET(SOURCES_libpld
	src/pld/pld.c
	src/pld/virtex2.c
	src/pld/xilinx_bit.c
)


SET(SOURCES_librtos
	src/rtos/
	src/rtos/ChibiOS.c
	src/rtos/FreeRTOS.c
	src/rtos/ThreadX.c
	src/rtos/eCos.c
	src/rtos/embKernel.c
	src/rtos/linux.c
	src/rtos/mqx.c
	src/rtos/rtos.c
	src/rtos/rtos_chibios_stackings.c
	src/rtos/rtos_ecos_stackings.c
	src/rtos/rtos_embkernel_stackings.c
	src/rtos/rtos_mqx_stackings.c
	src/rtos/rtos_standard_stackings.c
)


SET(SOURCES_libserver
	src/server/gdb_server.c
	src/server/server.c
	src/server/server_stubs.c
	src/server/tcl_server.c
	src/server/telnet_server.c
)


SET(SOURCES_libsvf
	src/svf/svf.c
)


SET(SOURCES_libtarget
	src/target/adi_v5_jtag.c
	src/target/adi_v5_swd.c
	src/target/algorithm.c
	src/target/arm11.c
	src/target/arm11_dbgtap.c
	src/target/arm720t.c
	src/target/arm7_9_common.c
	src/target/arm7tdmi.c
	src/target/arm920t.c
	src/target/arm926ejs.c
	src/target/arm946e.c
	src/target/arm966e.c
	src/target/arm9tdmi.c
	src/target/arm_adi_v5.c
	src/target/arm_disassembler.c
	src/target/arm_dpm.c
	src/target/arm_jtag.c
	src/target/arm_semihosting.c
	src/target/arm_simulator.c
	src/target/armv4_5.c
	src/target/armv4_5_cache.c
	src/target/armv4_5_mmu.c
	src/target/armv7a.c
	src/target/armv7m.c
	src/target/armv7m_trace.c
	src/target/avr32_ap7k.c
	src/target/avr32_jtag.c
	src/target/avr32_mem.c
	src/target/avr32_regs.c
	src/target/avrt.c
	src/target/breakpoints.c
	src/target/cortex_a.c
	src/target/cortex_m.c
	src/target/dsp563xx.c
	src/target/dsp563xx_once.c
	src/target/dsp5680xx.c
	src/target/embeddedice.c
	src/target/etb.c
	src/target/etm.c
	src/target/etm_dummy.c
	src/target/fa526.c
	src/target/feroceon.c
	src/target/hla_target.c
	src/target/image.c
	src/target/lakemont.c
	src/target/mips32.c
	src/target/mips32_dmaacc.c
	src/target/mips32_pracc.c
	src/target/mips_ejtag.c
	src/target/mips_m4k.c
	src/target/nds32.c
	src/target/nds32_aice.c
	src/target/nds32_cmd.c
	src/target/nds32_disassembler.c
	src/target/nds32_reg.c
	src/target/nds32_tlb.c
	src/target/nds32_v2.c
	src/target/nds32_v3.c
	src/target/nds32_v3_common.c
	src/target/nds32_v3m.c
	src/target/quark_x10xx.c
	src/target/register.c
	src/target/smp.c
	src/target/target.c
	src/target/target_request.c
	src/target/testee.c
	src/target/trace.c
	src/target/x86_32_common.c
	src/target/xscale.c
	src/target/openrisc/jsp_server.c
	src/target/openrisc/or1k.c
	src/target/openrisc/or1k_du_adv.c
	src/target/openrisc/or1k_tap_mohor.c
	src/target/openrisc/or1k_tap_vjtag.c
	src/target/openrisc/or1k_tap_xilinx_bscan.c
)
IF(BUILD_OOCD_TRACE)
	LIST(APPEND SOURCES_libtarget src/target/oocd_trace.c)
ENDIF(BUILD_OOCD_TRACE)


SET(SOURCES_libtransport
	src/transport/transport.c
)


SET(SOURCES_libxsvf
	src/xsvf/xsvf.c
)


# muhkuh_openocd contains the wrapper functions.
# The hello command handler is used in some sources.
SET(SOURCES_misc
	src/muhkuh_openocd.c
	src/hello.c
)

ADD_LIBRARY(LIBRARY_libopenocd SHARED ${SOURCES_libflash}
                                      ${SOURCES_libhelper}
                                      ${SOURCES_libjtag}
                                      ${SOURCES_libpld}
                                      ${SOURCES_librtos}
                                      ${SOURCES_libserver}
                                      ${SOURCES_libsvf}
                                      ${SOURCES_libtarget}
                                      ${SOURCES_libtransport}
                                      ${SOURCES_libxsvf}
                                      ${SOURCES_misc})
TARGET_INCLUDE_DIRECTORIES(LIBRARY_libopenocd
                           PRIVATE src
                                   src/helper
                                   src/jtag/drivers
                                   ${LIBJIM_INCLUDE_DIRECTORIES}
                                   ${CMAKE_CURRENT_BINARY_DIR}/config)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBOPENOCD_LDFLAGS -lws2_32)
ELSE(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBOPENOCD_LDFLAGS "")
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
TARGET_LINK_LIBRARIES(LIBRARY_libopenocd
                      PRIVATE LIBRARY_jim libusb::TARGET_libusb "${LIBOPENOCD_LDFLAGS}" "${LIBJIM_LDFLAGS}")

SET(LIBRARY_libopenocd_defines "")
LIST(APPEND LIBRARY_libopenocd_defines "HAVE_CONFIG_H")
LIST(APPEND LIBRARY_libopenocd_defines "PKGDATADIR=\"/usr/local/share/openocd\"")
LIST(APPEND LIBRARY_libopenocd_defines "RELSTR=\"-local\"")
LIST(APPEND LIBRARY_libopenocd_defines "GITVERSION=\"0\"")
LIST(APPEND LIBRARY_libopenocd_defines "PKGBLDDATE=\"2013-12-22-15:10\"")
LIST(APPEND LIBRARY_libopenocd_defines "BINDIR=\"/usr/this/is/the/bin/dir/define/changeme/baccy\"")
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	LIST(APPEND LIBRARY_libopenocd_defines "__USE_MINGW_ANSI_STDIO")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

TARGET_COMPILE_DEFINITIONS(LIBRARY_libopenocd
                           PRIVATE ${LIBRARY_libopenocd_defines})

# Set the name of the output file to "openocd".
SET_TARGET_PROPERTIES(LIBRARY_libopenocd PROPERTIES OUTPUT_NAME "openocd")


#-----------------------------------------------------------------------------
#
# Install all components of the library.
# This is the library itself and the public headers.
#

SET(CMAKE_INSTALL_PREFIX /install)
INSTALL(TARGETS LIBRARY_libopenocd
        EXPORT EXPORT_package
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

INSTALL(FILES src/muhkuh_openocd.h
        DESTINATION include)


#-----------------------------------------------------------------------------
#
# This is just a test for the library.
#

SET(SOURCES_openocd
	src/main.c
	src/openocd.c
)

ADD_EXECUTABLE(TARGET_openocd ${SOURCES_openocd})
TARGET_LINK_LIBRARIES(TARGET_openocd LIBRARY_libopenocd)
TARGET_INCLUDE_DIRECTORIES(TARGET_openocd
                           PRIVATE src
                                   src/helper
                                   ${CMAKE_CURRENT_BINARY_DIR}/config
                                   ${LIBJIM_INCLUDE_DIRECTORIES})
TARGET_COMPILE_DEFINITIONS(TARGET_openocd
                           PRIVATE ${LIBRARY_libopenocd_defines})


#----------------------------------------------------------------------------
#
# Build the CMAKE package.
#
INCLUDE(CMakePackageConfigHelpers)

SET(INCLUDE_INSTALL_DIR include/)
CONFIGURE_PACKAGE_CONFIG_FILE(cmake/config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/package/${PROJECT_NAME}-config.cmake"
                              INSTALL_DESTINATION cmake
                              PATH_VARS INCLUDE_INSTALL_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE("${CMAKE_CURRENT_BINARY_DIR}/package/${PROJECT_NAME}-version.cmake"
                                 COMPATIBILITY ExactVersion)

EXPORT(EXPORT EXPORT_package
       FILE "${CMAKE_CURRENT_BINARY_DIR}/package/${PROJECT_NAME}-targets.cmake"
       NAMESPACE ${PROJECT_NAME}::)

INSTALL(EXPORT EXPORT_package
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION cmake)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/package/${PROJECT_NAME}-config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/package/${PROJECT_NAME}-version.cmake"
        DESTINATION cmake
        COMPONENT Devel)
