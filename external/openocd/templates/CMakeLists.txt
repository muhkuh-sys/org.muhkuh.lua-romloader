cmake_minimum_required(VERSION 2.8)

PROJECT("openocd")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckStructHasMember)
INCLUDE(CheckTypeSize)


#----------------------------------------------------------------------------
#
# Get all defines for jim-config.h.in .
#
CHECK_TYPE_SIZE("long long" HAVE_LONG_LONG)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/jim-config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config/jim-config.h)



#----------------------------------------------------------------------------
#
# Get all defines for jimautoconf.h.in .
#

CHECK_INCLUDE_FILES(arpa/inet.h HAVE_ARPA_INET_H)
CHECK_FUNCTION_EXISTS(backtrace HAVE_BACKTRACE)
CHECK_INCLUDE_FILES(dirent.h HAVE_DIRENT_H)
CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)

# NOTE: This does not work as dlopen comes from the dl library.
#CHECK_FUNCTION_EXISTS("dlopen" HAVE_DLOPEN)
CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_DLOPEN)

CHECK_FUNCTION_EXISTS(execvpe HAVE_EXECVPE)
CHECK_FUNCTION_EXISTS(dlopen HAVE_DLOPEN)
CHECK_FUNCTION_EXISTS(fork HAVE_FORK)
CHECK_FUNCTION_EXISTS(fseeko HAVE_FSEEKO)
CHECK_FUNCTION_EXISTS(ftello HAVE_FTELLO)
CHECK_FUNCTION_EXISTS(getaddrinfo HAVE_GETADDRINFO)
CHECK_FUNCTION_EXISTS(geteuid HAVE_GETEUID)
CHECK_FUNCTION_EXISTS(inet_ntop HAVE_INET_NTOP)
CHECK_FUNCTION_EXISTS(isatty HAVE_ISATTY)
CHECK_FUNCTION_EXISTS(lfs HAVE_LFS)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
CHECK_TYPE_SIZE("long long" HAVE_LONG_LONG)
CHECK_FUNCTION_EXISTS(lstat HAVE_LSTAT)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)
CHECK_INCLUDE_FILES(netdb.h HAVE_NETDB_H)
CHECK_INCLUDE_FILES(netinet/in.h HAVE_NETINET_IN_H)
CHECK_FUNCTION_EXISTS(opendir HAVE_OPENDIR)
CHECK_FUNCTION_EXISTS(pipe HAVE_PIPE)
CHECK_LIBRARY_EXISTS(readline readline "" HAVE_READLINE)
CHECK_FUNCTION_EXISTS(readlink HAVE_READLINK)
CHECK_FUNCTION_EXISTS(realpath HAVE_REALPATH)
CHECK_FUNCTION_EXISTS(regcomp HAVE_REGCOMP)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(sigaction HAVE_SIGACTION)
CHECK_FUNCTION_EXISTS(sleep HAVE_SLEEP)
CHECK_FUNCTION_EXISTS(socket HAVE_SOCKET)
CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_FUNCTION_EXISTS(strptime HAVE_STRPTIME)
CHECK_STRUCT_HAS_MEMBER("struct sysinfo" uptime sys/sysinfo.h HAVE_STRUCT_SYSINFO_UPTIME)
CHECK_FUNCTION_EXISTS(sysinfo HAVE_SYSINFO)
CHECK_FUNCTION_EXISTS(syslog HAVE_SYSLOG)
CHECK_FUNCTION_EXISTS(sys_siglist HAVE_SYS_SIGLIST)
CHECK_FUNCTION_EXISTS(sys_signame HAVE_SYS_SIGNAME)
CHECK_INCLUDE_FILES(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_FUNCTION_EXISTS(sys_signame HAVE_SYS_SIGNAME)
CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(sys/sysinfo.h HAVE_SYS_SYSINFO_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(sys/un.h HAVE_SYS_UN_H)
CHECK_INCLUDE_FILES(termios.h HAVE_TERMIOS_H)
CHECK_FUNCTION_EXISTS(ualarm HAVE_UALARM)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)
CHECK_FUNCTION_EXISTS(usleep HAVE_USLEEP)
CHECK_FUNCTION_EXISTS(utimes HAVE_UTIMES)
CHECK_FUNCTION_EXISTS(vfork HAVE_VFORK)
CHECK_FUNCTION_EXISTS(waitpid HAVE_WAITPID)

# Check if large file sizes are available and if the define '_FILE_OFFSET_BITS=64' is necessary for it.
CHECK_TYPE_SIZE(off_t SIZEOF_OFF_T_1)
IF(${SIZEOF_OFF_T_1} EQUAL 8)
	SET(HAVE_LFS 1)
	SET(SIZEOF_OFF_T 8)
ELSE(${SIZEOF_OFF_T_1} EQUAL 8)
	SET(CMAKE_REQUIRED_DEFINITIONS -D_FILE_OFFSET_BITS=64)
	CHECK_TYPE_SIZE(off_t SIZEOF_OFF_T_2)
	IF(${SIZEOF_OFF_T_2} EQUAL 8)
		SET(HAVE_LFS 1)
		SET(SIZEOF_OFF_T 8)
		SET(_FILE_OFFSET_BITS 64)
	ENDIF(${SIZEOF_OFF_T_2} EQUAL 8)
	UNSET(CMAKE_REQUIRED_DEFINITIONS)
ENDIF(${SIZEOF_OFF_T_1} EQUAL 8)


IF(HAVE_TERMIOS_H AND HAVE_ISATTY)
	SET(USE_LINENOISE 1)
ENDIF(HAVE_TERMIOS_H AND HAVE_ISATTY)


IF(${CMAKE_COMPILER_IS_GNUCC})
	SET(STDC_HEADERS 1)
ENDIF(${CMAKE_COMPILER_IS_GNUCC})


IF(MSVC)
	SET(TCL_PLATFORM_OS "windows")
	SET(TCL_PLATFORM_PLATFORM "windows")
	SET(TCL_PLATFORM_PATH_SEPARATOR ";")
	SET(HAVE_MKDIR_ONE_ARG 1)
	SET(HAVE_SYSTEM 1)

ELIF(MINGW)
	# GNU compiler on windows - this must be minGW.
	SET(TCL_PLATFORM_OS "mingw")
	SET(TCL_PLATFORM_PLATFORM "windows")
	SET(TCL_PLATFORM_PATH_SEPARATOR ";")
	SET(HAVE_MKDIR_ONE_ARG 1)
	SET(HAVE_SYSTEM 1)
ELSE(MSVC)
	# Default to GNU compiler on unix.
	SET(TCL_PLATFORM_OS "unknown")
	SET(TCL_PLATFORM_PLATFORM "unix")
	SET(TCL_PLATFORM_PATH_SEPARATOR ":")
	SET(HAVE_SYSTEM 1)
ENDIF(MSVC)


SET(JIM_REFERENCES 1)
SET(JIM_REGEXP 1)
SET(JIM_STATICLIB 1)

SET(jim_ext_bootstrap 1)
SET(jim_ext_aio 1)
SET(jim_ext_readdir 1)
SET(jim_ext_glob 1)
SET(jim_ext_regexp 1)
SET(jim_ext_file 1)
SET(jim_ext_exec 1)
SET(jim_ext_clock 1)
SET(jim_ext_array 1)
SET(jim_ext_stdlib 1)
SET(jim_ext_tclcompat 1)
SET(jim_ext_binary 1)
SET(jim_ext_eventloop 1)
SET(jim_ext_history 1)
SET(jim_ext_load 1)
SET(jim_ext_namespace 1)
SET(jim_ext_nshelper 1)
SET(jim_ext_oo 1)
SET(jim_ext_pack 1)
SET(jim_ext_package 1)
SET(jim_ext_posix 1)
SET(jim_ext_signal 1)
SET(jim_ext_syslog 1)
SET(jim_ext_tclprefix 1)
SET(jim_ext_tree 1)


SET(TCL_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/jim)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/jimautoconf.h.in ${CMAKE_CURRENT_BINARY_DIR}/config/jimautoconf.h)


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

#CHECK_LIBRARY_EXISTS(readline readline "" HAVE_READLINE)
#/* Define if you have libusb-1.x */
# We definitely have libusbx-1.x .
#SET(HAVE_LIBUSB1 1)

#/* Define if your libusb has libusb_error_name() */
##define HAVE_LIBUSB_ERROR_NAME 1

CHECK_TYPE_SIZE("long long int" HAVE_LONG_LONG_INT)
CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(netdb.h HAVE_NETDB_H)
CHECK_INCLUDE_FILES(netinet/in.h HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES(netinet/tcp.h HAVE_NETINET_TCP_H)
CHECK_INCLUDE_FILES(net/if.h HAVE_NET_IF_H)
CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)

#/* Define to 1 if stdbool.h conforms to C99. */
##define HAVE_STDBOOL_H 1

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


IF(_GNU_SOURCE)
	ADD_DEFINITIONS(-D_GNU_SOURCE)
ENDIF(_GNU_SOURCE)


# Build a bootstrap version of jimsh.
SET(SOURCE_jimsh_bootstrap ${CMAKE_CURRENT_BINARY_DIR}/jimtcl/jimsh0.c)

FILE(WRITE ${SOURCE_jimsh_bootstrap} "/* This is single source file, bootstrap version of Jim Tcl. See http://jim.berlios.de/ */\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define _GNU_SOURCE\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define JIM_TCL_COMPAT\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define JIM_REFERENCES\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define JIM_ANSIC\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define JIM_REGEXP\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_NO_AUTOCONF\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define _JIMAUTOCONF_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_LIBRARY \".\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_bootstrap\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_aio\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_readdir\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_glob\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_regexp\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_file\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_exec\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_clock\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_array\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_stdlib\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define jim_ext_tclcompat\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#if defined(_MSC_VER)\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_OS \"windows\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PLATFORM \"windows\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PATH_SEPARATOR \";\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_MKDIR_ONE_ARG\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_SYSTEM\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#elif defined(__MINGW32__)\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_OS \"mingw\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PLATFORM \"windows\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PATH_SEPARATOR \";\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_MKDIR_ONE_ARG\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_SYSTEM\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_SYS_TIME_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_DIRENT_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_UNISTD_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#else\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_OS \"unknown\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PLATFORM \"unix\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define TCL_PLATFORM_PATH_SEPARATOR \":\"\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_VFORK\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_WAITPID\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_ISATTY\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_SYS_TIME_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_DIRENT_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#define HAVE_UNISTD_H\n")
FILE(APPEND ${SOURCE_jimsh_bootstrap} "#endif\n")





SET(SOURCES_libjim jimtcl/jim-subcmd.c
                   jimtcl/jim-interactive.c
                   jimtcl/jim-format.c
                   jimtcl/jim.c
                   jimtcl/utf8.c
                   jimtcl/jimregexp.c
                   jimtcl/linenoise.c
                   jimtcl/jim-aio.c
                   jimtcl/jim-array.c
                   jimtcl/jim-clock.c
                   jimtcl/jim-eventloop.c
                   jimtcl/jim-exec.c
                   jimtcl/jim-file.c
                   jimtcl/jim-history.c
                   jimtcl/jim-load.c
                   jimtcl/jim-namespace.c
                   jimtcl/jim-pack.c
                   jimtcl/jim-package.c
                   jimtcl/jim-posix.c
                   jimtcl/jim-readdir.c
                   jimtcl/jim-regexp.c
                   jimtcl/jim-signal.c
                   jimtcl/jim-syslog.c
                   jimtcl/jim-tclprefix.c
)
