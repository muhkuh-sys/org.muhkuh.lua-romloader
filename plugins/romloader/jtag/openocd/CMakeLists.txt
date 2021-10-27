cmake_minimum_required(VERSION 3.7)

PROJECT("romloader_jtag_openocd")

# SWIG is required because we include romloader_jtag_main.h, 
# which causes muhkuh_plugin_interface.h to be íncluded.
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})


#-----------------------------------------------------------------------------
#
# Build the openocd adaptor.
#

SET(romloader_jtag_openocd_SOURCES
	romloader_jtag_openocd.cpp
	shared_library.cpp
)

SET(romloader_jtag_openocd_HEADERS
	romloader_jtag_openocd.h
	shared_library.h
)


ADD_LIBRARY(TARGET_romloader_jtag_openocd_lib STATIC ${romloader_jtag_openocd_SOURCES})
TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_jtag_openocd_lib
                           PRIVATE ${LUA_INCLUDE_DIR} ${LIBUSB_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_jtag_openocd_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
