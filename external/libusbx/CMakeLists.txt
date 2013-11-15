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


#----------------------------------------------------------------------------
#
# Build the project.
#
ExternalProject_Add(TARGET_libusbx_depack
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.17
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/libusbx-1.0.17.tar.bz2
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.17/src/TARGET_libusbx_depack
                    CMAKE_ARGS -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                               -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                               -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
                    INSTALL_COMMAND make install DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.17
)



# 3) Provide all variables for the build.
SET(LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.17/install/lib/libusbx.a)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET(LDFLAGS "-pthread -lrt -ludev")
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

SET(DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/libusbx-1.0.17/install/include/libusb-1.0)

#----------------------------------------------------------------------------
#
# Store all information about the library in the cache.
#
SET(LIBUSBX_LIBRARY              ${LIBRARY}              CACHE INTERNAL "The libusbx library with the complete path.")
SET(LIBUSBX_LDFLAGS              ${LDFLAGS}              CACHE INTERNAL "All linker flags required to link the library. The list must be separated with whitespace.")
SET(LIBUSBX_DEFINES              ${DEFINES}              CACHE INTERNAL "All defines for the libusbx library. This should be a list of entries starting with -D.")
SET(LIBUSBX_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  CACHE INTERNAL "The include folders for the library headers. The list must be separated with whitespace.")
