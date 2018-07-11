cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_usb")


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(libusb REQUIRED)


#----------------------------------------------------------------------------
#
# Get the netX USB firmware binaries (in form of C data arrays) and headers.
#

SET(NETX_USB_FIRMWARE_SOURCES ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx10.c
                              ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx56.c
                              ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx500.c)

SET(NETX_USB_FIRMWARE_HEADERS ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx10.h
                              ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx56.h
                              ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx500.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#


SET(romloader_usb_SOURCES
	romloader_usb_device.cpp
	romloader_usb_device_libusb.cpp
	romloader_usb_main.cpp
	${NETX_USB_FIRMWARE_SOURCES}
)

SET(romloader_usb_HEADERS
	romloader_usb_device_libusb.h
	romloader_usb_main.h
	${NETX_USB_FIRMWARE_HEADERS}
)


ADD_LIBRARY(TARGET_romloader_usb_lib STATIC ${romloader_usb_SOURCES})
ADD_DEPENDENCIES(TARGET_romloader_usb_lib TARGET_NETX_FIRMWARE)
TARGET_LINK_LIBRARIES(TARGET_romloader_usb_lib libusb::TARGET_libusb)
TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_usb_lib
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH} ${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/)
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	SET_TARGET_PROPERTIES(TARGET_romloader_usb_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#
SET_SOURCE_FILES_PROPERTIES(romloader_usb.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_usb.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY} -I${CMAKE_BINARY_DIR}/version_information)
SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_usb_LUA lua romloader_usb.i)
SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_usb_LUA TARGET_romloader_usb_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_romloader_usb_LUA
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set the name of the output file to "romloader_usb".
SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_usb_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_usb")

IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# On minGW link all compiler libraries static.
	SET_PROPERTY(TARGET TARGET_plugins_romloader_romloader_usb_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# Install the lua module.
INSTALL(TARGETS TARGET_plugins_romloader_romloader_usb_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Add tests for this module.
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# Here are the MinGW specific tests.
	ADD_TEST(NAME romloader_usb_MinGW_DLL_dependencies
	         COMMAND "${PYTHON_EXECUTABLE}" ${TEST_SCRIPT_DIR}/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_plugins_romloader_romloader_usb_LUA>)
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
