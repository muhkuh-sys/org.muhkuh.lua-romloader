cmake_minimum_required(VERSION 2.8)

PROJECT("openocd")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckStructHasMember)
INCLUDE(CheckTypeSize)


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

CHECK_INCLUDE_FILES(arpa/inet.h HAVE_ARPA_INET_H)
CHECK_INCLUDE_FILES(dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(elf.h HAVE_ELF_H)
CHECK_INCLUDE_FILES(fcntl.h HAVE_FCNTL_H)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_INCLUDE_FILES(ifaddrs.h HAVE_IFADDRS_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(jtag_minidriver.h HAVE_JTAG_MINIDRIVER_H)

# We definitely have libusbx-1.x with libusb_error_name() .
SET(HAVE_LIBUSB1 1)
SET(HAVE_LIBUSB_ERROR_NAME 1)

CHECK_TYPE_SIZE("long long int" HAVE_LONG_LONG_INT)
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
SET(PARPORT_USE_PPDEV 1)

SET(PACKAGE "openocd")
SET(PACKAGE_BUGREPORT "OpenOCD Mailing List <openocd-devel@lists.sourceforge.net>")
SET(PACKAGE_NAME "openocd")
SET(PACKAGE_STRING "openocd 0.8.0-dev")
SET(PACKAGE_TARNAME "openocd")
SET(PACKAGE_URL "")
SET(PACKAGE_VERSION "0.8.0-dev")
SET(VERSION "0.8.0-dev")

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


SET(SOURCES_openocd
	src/hello.c
	src/main.c
	src/openocd.c
	src/startup_tcl.c
)


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
	src/flash/nor/at91sam7.c
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
	src/flash/nor/non_cfi.c
	src/flash/nor/nuc1x.c
	src/flash/nor/ocl.c
	src/flash/nor/pic32mx.c
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

ADD_LIBRARY(LIBRARY_libflash STATIC ${SOURCES_libflash})
TARGET_INCLUDE_DIRECTORIES(LIBRARY_libflash
                           PRIVATE src
                                   src/helper
                                   jimtcl)


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

ADD_LIBRARY(LIBRARY_libhelper STATIC ${SOURCES_libhelper})
TARGET_INCLUDE_DIRECTORIES(LIBRARY_libhelper
                           PRIVATE src
)

