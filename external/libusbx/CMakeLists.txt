cmake_minimum_required(VERSION 2.8)

#----------------------------------------------------------------------------
#
# This script provides the libusbx parts to the build process.
#
# 1) It depacks the archiv.
# 2) Eventually compile the sources.
# 3) It provides all necessary variables to link the library.
#
# The provided variables are:
#
#  LIBUSBX_LIBRARIES : The libusbx library with the complete path and all
#                      required system libraries. The list must be separated
#                      with whitespace.
#
#  LIBUSBX_DEFINES : All defines for the libusbx library. This should be a
#                    list of entries starting with "-D".
#
#  LIBUSBX_INCLUDE_DIRECTORIES : The include folders for the library headers.
#                                The list must be separated with whitespace.
#

PROJECT("libusbx")

INCLUDE(ExternalProject)


IF(UNIX)
	# The linux version of libusbx is distributed as source code.
	# 1) Depack the archive.
	#    tar fxj libusbx-1.0.16.tar.bz2
	#
	# 2) Build the library.
	#     cd libusbx-1.0.16
	#     mkdir build
	#     cd build
	#     ../configure --prefix=`pwd`/install
	#     make install
	IF(CMAKE_SIZEOF_VOID_P MATCHES "4")
		# This is 32 bit.
		SET(CONFIGURE_CFLAGS "-m32")
		SET(CONFIGURE_OPTIONS "--host=i686-pc-linux")
	ELSEIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
		# This is 64 bit.
		SET(CONFIGURE_CFLAGS "-m64")
		SET(CONFIGURE_OPTIONS "--host=x86_64-pc-linux")
	ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "4")
	
	ExternalProject_Add(TARGET_libusbx
	                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16
	                    DOWNLOAD_COMMAND tar --extract --bzip2 --directory ${CMAKE_CURRENT_BINARY_DIR} --file ${CMAKE_CURRENT_SOURCE_DIR}/libusbx-1.0.16.tar.bz2
	                    CONFIGURE_COMMAND export CFLAGS=${CONFIGURE_CFLAGS} && cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && ./configure --with-pic --prefix=${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install ${CONFIGURE_OPTIONS}
	                    BUILD_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && make all
	                    INSTALL_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && make install
	)
	
	# 3) Provide all variables for the build.
	SET(LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install/lib/libusb-1.0.a -pthread -lrt -ludev)
	SET(DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
	SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install/include/libusb-1.0)
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	IF(MSVC)
		# The Microsoft Visual C version is distributed in
		# pre-compiled form.
		#
		# 1) Depack the archive.
		#7z x -olibusbx-1.0.16-win libusbx-1.0.16-win.7z
		FIND_PROGRAM(SEVENZIP_EXECUTABLE
		             NAMES 7z
		             PATH_SUFFIXES 7-Zip 7zip 
		             DOC "7zip command line client"
		)
		
		ExternalProject_Add(TARGET_libusbx
		                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16-win
		                    DOWNLOAD_COMMAND ${SEVENZIP_EXECUTABLE} x -o${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16-win ${CMAKE_CURRENT_SOURCE_DIR}/libusbx-1.0.16-win.7z
		                    CONFIGURE_COMMAND ""
		                    BUILD_COMMAND ""
		                    INSTALL_COMMAND ""
		)
		
		# Is this 32 or 64 bit?
		IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
			SET(LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16-win/MS64/static/libusb-1.0.lib)
		ELSE(CMAKE_SIZEOF_VOID_P MATCHES "8")
			SET(LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16-win/MS32/static/libusb-1.0.lib)
		ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
		SET(DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
		SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16-win/install/include/libusb-1.0)
	ELSEIF(${CMAKE_COMPILER_IS_GNUCC})
		# This is GCC on Windows. It must be minGW.
		
		# The minGW version is also build from sources to prevent
		# incompatibilities between various minGW versions. Just to
		# note: __ms_vsnprintf, __vsnprintf and _vsnprintf. I gave up to
		# write wrappers for all kinds of combinations.
		IF(CMAKE_SIZEOF_VOID_P MATCHES "4")
			# This is 32 bit.
			SET(CONFIGURE_CFLAGS "-m32")
			SET(CONFIGURE_OPTIONS "--host=i686-w64-mingw32")
		ELSEIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
			# This is 64 bit.
			SET(CONFIGURE_CFLAGS "-m64")
			SET(CONFIGURE_OPTIONS "--host=x86_64-w64-mingw32")
		ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "4")
		
		ExternalProject_Add(TARGET_libusbx
		                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16
		                    DOWNLOAD_COMMAND tar --extract --bzip2 --directory ${CMAKE_CURRENT_BINARY_DIR} --file ${CMAKE_CURRENT_SOURCE_DIR}/libusbx-1.0.16.tar.bz2
		                    CONFIGURE_COMMAND export CFLAGS=${CONFIGURE_CFLAGS} && cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && ./configure --with-pic --prefix=${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install ${CONFIGURE_OPTIONS}
		                    BUILD_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && make all
		                    INSTALL_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16 && make install
		)
		
		# Provide all variables for the build.
		SET(LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install/lib/libusb-1.0.a)
		SET(DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
		SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.16/install/include/libusb-1.0)
	ENDIF(MSVC)
ENDIF(UNIX)

SET(LIBUSBX_LIBRARIES            ${LIBRARIES}            CACHE STRING "The libusbx library with the complete path and all required system libraries. The list must be separated with whitespace.")
SET(LIBUSBX_DEFINES              ${DEFINES}              CACHE STRING "All defines for the libusbx library. This should be a list of entries starting with -D.")
SET(LIBUSBX_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  CACHE PATH   "The include folders for the library headers. The list must be separated with whitespace.")
