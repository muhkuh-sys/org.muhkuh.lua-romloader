cmake_minimum_required(VERSION 2.8)

PROJECT("jimtcl")

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
	SET(TCL_PLATFORM_OS "linux")
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
# Generate the TCL includes.
#

SET(SOURCES_libjim jim-subcmd.c
                   jim-interactive.c
                   jim-format.c
                   jim.c
                   utf8.c
                   jimregexp.c
                   linenoise.c
                   jim-aio.c
                   jim-array.c
                   jim-clock.c
                   jim-eventloop.c
                   jim-exec.c
                   jim-file.c
                   jim-history.c
                   jim-load.c
                   jim-namespace.c
                   jim-pack.c
                   jim-package.c
                   jim-posix.c
                   jim-readdir.c
                   jim-regexp.c
                   jim-signal.c
                   jim-syslog.c
                   jim-tclprefix.c
)



FOREACH(SOURCE binary glob initjimsh nshelper oo stdlib tclcompat tree)
	SET(SRC "${CMAKE_CURRENT_BINARY_DIR}/generated/_${SOURCE}.c")
	FILE(WRITE  ${SRC} "/* autogenerated - do not edit */\n")
	FILE(APPEND ${SRC} "#include <jim.h>\n")
	FILE(APPEND ${SRC} "int Jim_${SOURCE}Init(Jim_Interp *interp)\n")
	FILE(APPEND ${SRC} "{\n")
	FILE(APPEND ${SRC} "\tif (Jim_PackageProvide(interp, \"${SOURCE}\", \"1.0\", JIM_ERRMSG)) return JIM_ERR;\n")
	FILE(APPEND ${SRC} "\treturn Jim_EvalSource(interp, \"${SOURCE}.tcl\", 1, ")
	
	# Try to work with the complete file.
	FILE(READ ${SOURCE}.tcl LINES1)
	STRING(REPLACE "\\" "\\\\" LINES2 "${LINES1}")
	STRING(REPLACE "\"" "\\\"" LINES3 "${LINES2}")
	STRING(REGEX REPLACE "\n[ \t]*#[^\r\n]*" "\n" LINES4 "${LINES3}")
	STRING(REGEX REPLACE "^[ \t]*#[^\r\n]*" "" LINES5 "${LINES4}")
	STRING(REGEX REPLACE "([^\r\n]*)(\r?\n)" "\"\\1\\\\n\"\\2" LINES6 "${LINES5}")
	FILE(APPEND ${SRC} "${LINES6}")
	
	FILE(APPEND ${SRC} ");\n")
	FILE(APPEND ${SRC} "}\n")
	
	LIST(APPEND SOURCES_libjim "${SRC}")
ENDFOREACH(SOURCE)


# NOTE: The order of the extensions is important!
SET(EXTENSIONS_INIT "")
LIST(APPEND EXTENSIONS_INIT "stdlib")
LIST(APPEND EXTENSIONS_INIT "aio")
LIST(APPEND EXTENSIONS_INIT "array")
LIST(APPEND EXTENSIONS_INIT "clock")
LIST(APPEND EXTENSIONS_INIT "eventloop")
LIST(APPEND EXTENSIONS_INIT "exec")
LIST(APPEND EXTENSIONS_INIT "file")
LIST(APPEND EXTENSIONS_INIT "history")
LIST(APPEND EXTENSIONS_INIT "load")
LIST(APPEND EXTENSIONS_INIT "namespace")
LIST(APPEND EXTENSIONS_INIT "nshelper")
LIST(APPEND EXTENSIONS_INIT "oo")
LIST(APPEND EXTENSIONS_INIT "pack")
LIST(APPEND EXTENSIONS_INIT "package")
LIST(APPEND EXTENSIONS_INIT "posix")
LIST(APPEND EXTENSIONS_INIT "readdir")
LIST(APPEND EXTENSIONS_INIT "regexp")
LIST(APPEND EXTENSIONS_INIT "signal")
LIST(APPEND EXTENSIONS_INIT "syslog")
LIST(APPEND EXTENSIONS_INIT "tclcompat")
LIST(APPEND EXTENSIONS_INIT "tclprefix")
LIST(APPEND EXTENSIONS_INIT "binary")
LIST(APPEND EXTENSIONS_INIT "glob")
LIST(APPEND EXTENSIONS_INIT "tree")

SET(SRC "${CMAKE_CURRENT_BINARY_DIR}/generated/_load-static-exts.c")
FILE(WRITE  ${SRC} "\n")
FILE(APPEND ${SRC} "/* autogenerated - do not edit */\n")
FILE(APPEND ${SRC} "#include \"jim.h\"\n")
FILE(APPEND ${SRC} "#include \"jimautoconf.h\"\n")
FILE(APPEND ${SRC} "int Jim_InitStaticExtensions(Jim_Interp *interp)\n")
FILE(APPEND ${SRC} "\n")
FILE(APPEND ${SRC} "{\n")
FOREACH(SOURCE IN LISTS EXTENSIONS_INIT)
	FILE(APPEND ${SRC} "\textern int Jim_${SOURCE}Init(Jim_Interp *);\n")
ENDFOREACH(SOURCE)
FOREACH(SOURCE IN LISTS EXTENSIONS_INIT)
	FILE(APPEND ${SRC} "\tJim_${SOURCE}Init(interp);\n")
ENDFOREACH(SOURCE)
FILE(APPEND ${SRC} "\treturn JIM_OK;\n")
FILE(APPEND ${SRC} "}\n")

LIST(APPEND SOURCES_libjim "${SRC}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
ADD_LIBRARY(LIBRARY_jim STATIC ${SOURCES_libjim})
TARGET_INCLUDE_DIRECTORIES(LIBRARY_jim
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/config)

SET(LDFLAGS "-ldl -lm")
SET(DEFINES "")
SET(INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/config")

#-----------------------------------------------------------------------------

#SET(LIBJIM_LIBRARY              ${LIBRARY}              CACHE INTERNAL "The libusbx library with the complete path.")
SET(LIBJIM_LDFLAGS              ${LDFLAGS}              CACHE INTERNAL "All linker flags required to link the library. The list must be separated with whitespace.")
SET(LIBJIM_DEFINES              ${DEFINES}              CACHE INTERNAL "All defines for the libusbx library. This should be a list of entries starting with -D.")
SET(LIBJIM_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  CACHE INTERNAL "The include folders for the library headers. The list must be separated with whitespace.")

