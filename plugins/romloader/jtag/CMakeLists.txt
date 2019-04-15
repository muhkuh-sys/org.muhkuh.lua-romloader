CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

ADD_SUBDIRECTORY(openocd)


PROJECT("romloader_jtag"
        VERSION ${MUHKUH_VERSION_ALL})


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(libusb REQUIRED)

#-----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

SET(romloader_jtag_SOURCES
	romloader_jtag_main.cpp
)

SET(romloader_jtag_HEADERS
	romloader_jtag_main.h
	romloader_jtag_openocd.h
)


ADD_LIBRARY(TARGET_romloader_jtag_lib STATIC ${romloader_jtag_SOURCES})
TARGET_LINK_LIBRARIES(TARGET_romloader_jtag_lib libusb::TARGET_libusb)
TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_jtag_lib
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	SET_TARGET_PROPERTIES(TARGET_romloader_jtag_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))

# The library depends on the openocd header.
ADD_DEPENDENCIES(TARGET_romloader_jtag_lib TARGET_romloader_jtag_openocd_lib)



#----------------------------------------------------------------------------
#
# Build the plugins.
#

SET_SOURCE_FILES_PROPERTIES(romloader_jtag.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_jtag.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY} -I${CMAKE_BINARY_DIR}/version_information)
SWIG_ADD_LIBRARY(TARGET_plugins_romloader_romloader_jtag_LUA
                 TYPE MODULE
                 LANGUAGE LUA
                 SOURCES romloader_jtag.i)
SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_jtag_LUA TARGET_romloader_jtag_lib TARGET_romloader_jtag_openocd_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_romloader_jtag_LUA
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set the name of the output file to "romloader_jtag".
SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_jtag_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_jtag")

IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# On minGW link all compiler libraries static.
	SET_PROPERTY(TARGET TARGET_plugins_romloader_romloader_jtag_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# The module depends on the openocd libraries.
ADD_DEPENDENCIES(TARGET_plugins_romloader_romloader_jtag_LUA TARGET_romloader_jtag_openocd_lib)

# Install the lua module.
INSTALL(TARGETS TARGET_plugins_romloader_romloader_jtag_LUA
        DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Install the OpenOCD scripts.
INSTALL(FILES openocd/tcl/jtag_detect_init.tcl
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)
INSTALL(FILES openocd/tcl/netx_swreset.tcl
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)
INSTALL(FILES openocd/tcl/chip_init.tcl
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)

# Install the DCC binaries.
INSTALL(FILES ../../../targets/plugins/romloader/jtag/netx/netx500/dcc_netx500.bin
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)
INSTALL(FILES ../../../targets/plugins/romloader/jtag/netx/netx10_50_51_52/dcc_netx10_50_51_52.bin
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)
INSTALL(FILES ../../../targets/plugins/romloader/jtag/netx/netx90/dcc_netx90_com.bin
        DESTINATION ${INSTALL_DIR_LUA_MODULES}/openocd)

# Add tests for this module.
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# Here are the MinGW specific tests.
	ADD_TEST(NAME romloader_jtag_MinGW_DLL_dependencies
	         COMMAND "${PYTHON_EXECUTABLE}" ${TEST_SCRIPT_DIR}/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_plugins_romloader_romloader_jtag_LUA>)
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
