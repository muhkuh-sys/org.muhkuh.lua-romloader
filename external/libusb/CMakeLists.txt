cmake_minimum_required(VERSION 3.7)

#----------------------------------------------------------------------------
#
# This script provides the libusb parts to the build process.
#
# 1) It depacks the archiv.
# 2) Eventually compile the sources.
# 3) It provides all necessary variables to link the library.
#
# The provided variables are:
#
#  LIBUSB_LIBRARIES : The libusb library with the complete path and all
#                     required system libraries. The list must be separated
#                     with whitespace.
#
#  LIBUSB_DEFINES : All defines for the libusb library. This should be a
#                   list of entries starting with "-D".
#
#  LIBUSB_INCLUDE_DIRECTORIES : The include folders for the library headers.
#                               The list must be separated with whitespace.
#

PROJECT("libusb")

INCLUDE(ExternalProject)

SET(VERSION_EXTERNAL "1.0.20")

#----------------------------------------------------------------------------
#
# Build the project.
#
SET(LIBUSB_CMAKE_ARGS "")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DVERSION_EXTERNAL=${VERSION_EXTERNAL}")
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

ExternalProject_Add(TARGET_libusb
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/libusb-${VERSION_EXTERNAL}.tar.bz2
                    URL_HASH SHA1=9537243f165927bde74ad742e6b3effb0bd50cd2
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}/src/TARGET_libusb
                    CMAKE_ARGS ${LIBUSB_CMAKE_ARGS}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_CURRENT_BINARY_DIR}
)



#IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
#	SET(LDFLAGS "-pthread -lrt -ludev")
#ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
