cmake_minimum_required(VERSION 2.8)

MESSAGE("OPENOCD_LIBRARIES: ${OPENOCD_LIBRARIES}")
MESSAGE("OPENOCD_DEFINES: ${OPENOCD_DEFINES}")
MESSAGE("OPENOCD_INCLUDE_DIRECTORIES: ${OPENOCD_INCLUDE_DIRECTORIES}")


PROJECT("romloader_jtag_openocd")


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


#-----------------------------------------------------------------------------
#
# Build the openocd adaptor.
#

# Append the include directories for openocd.
INCLUDE_DIRECTORIES(${OPENOCD_INCLUDE_DIRECTORIES})

# Append the openocd defines.
ADD_DEFINITIONS(${OPENOCD_DEFINES})


SET(romloader_jtag_openocd_SOURCES
	romloader_jtag_openocd.c
)

SET(romloader_jtag_openocd_HEADERS
	romloader_jtag_openocd.h
)


ADD_LIBRARY(TARGET_romloader_jtag_openocd_lib STATIC ${romloader_jtag_openocd_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_jtag_openocd_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# The library depends on the openocd header.
ADD_DEPENDENCIES(TARGET_romloader_jtag_openocd_lib TARGET_openocd)

