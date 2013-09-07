cmake_minimum_required(VERSION 2.8)

#----------------------------------------------------------------------------
#
# This script provides the openocd libraries to the build process.
#
# 1) It depacks the archiv.
# 2) It compile the sources.
# 3) It provides all necessary variables to link the libraries.
#
# The provided variables are:
#
#  OPENOCD_LIBRARIES : The openocd libraries without path and all required
#                      system libraries. The list must be separated with
#                      whitespace.
#
#  OPENOCD_DEFINES : All defines for the openocd library. This should be a
#                    list of entries starting with "-D".
#
#  OPENOCD_INCLUDE_DIRECTORIES : The include folders for the library headers.
#                                The list must be separated with whitespace.
#

PROJECT("openocd")

INCLUDE(ExternalProject)


IF(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
	# 1) Depack the archive.
	#    tar fxj libusbx-1.0.16.tar.bz2
	#
	# 2) Build the library.
	#     cd libusbx-1.0.16
	#     mkdir build
	#     cd build
	#     ../configure --prefix=`pwd`/install
	#     make install
	IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		# Building on linux for linux.
		SET(CONFIGURE_CFLAGS "")
		SET(CONFIGURE_OPTIONS "")
	ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		# Building on linux for windows.
		SET(CONFIGURE_CFLAGS "")
		SET(CONFIGURE_OPTIONS "")
	ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	
	ExternalProject_Add(TARGET_openocd
	                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0
	                    DOWNLOAD_COMMAND tar --extract --bzip2 --directory ${CMAKE_CURRENT_BINARY_DIR} --file ${CMAKE_CURRENT_SOURCE_DIR}/openocd-0.7.0.tar.bz2
	                    CONFIGURE_COMMAND export CFLAGS=${CONFIGURE_CFLAGS} && cd ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0 && ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0/install ${CONFIGURE_OPTIONS} --enable-ftdi
	                    BUILD_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0 && make all
	                    INSTALL_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0 && make install
	)
	
	# 3) Provide all variables for the build.
	SET(LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0/install/lib/libopenocd.a ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0/jimtcl/libjim.a -lusb-1.0 -ldl)
	SET(DEFINES "")
	SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.7.0)
	SET(LINKER_FLAGS "")
ELSEIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	MESSAGE(FATAL "Building openocd on windows is not supported yet!")
ENDIF(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")

SET(OPENOCD_LIBRARIES            ${LIBRARIES}            PARENT_SCOPE)
SET(OPENICD_DEFINES              ${DEFINES}              PARENT_SCOPE)
SET(OPENOCD_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  PARENT_SCOPE)
SET(OPENOCD_LINKER_FLAGS         ${LINKER_FLAGS}         PARENT_SCOPE)
