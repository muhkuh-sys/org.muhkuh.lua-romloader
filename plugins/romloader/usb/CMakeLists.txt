cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_usb")

SET(romloader_usb_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


#----------------------------------------------------------------------------
#
# Compile the netX USB firmware.
#

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.h
                   COMMAND ${PYTHON_EXECUTABLE} setup.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/netx
                   COMMENT "Build the netx usb firmware"
                   VERBATIM)

ADD_CUSTOM_TARGET(netx_usb_firmware
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.h)


#----------------------------------------------------------------------------
#
# Set the libusb include paths.
#

IF(UNIX)
	# Use libusb-1.0
	SET(USB_LIBRARIES usb-1.0)
	SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
ELSE(UNIX)
	IF(WIN32)
		# Use libusb-0 on Windows to get a proper reset.
		# NOTE: The current version of libusb-1.0 on windows uses the driver winusb.
		#       It does not allow a complete device reset.
		SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=0)
		INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/include/)
		LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/lib/msvc)
		SET(USB_LIBRARIES libusb setupapi)
	ENDIF(WIN32)
ENDIF(UNIX)


set_source_files_properties(romloader_usb.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader_usb.i PROPERTY SWIG_FLAGS ${USB_DEFINES} -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


SET(romloader_usb_SOURCES
	romloader_usb_device.cpp
	romloader_usb_device_libusb.cpp
	romloader_usb_main.cpp
	uuencoder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.c
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.c
)

SET(romloader_usb_HEADERS
	romloader_usb_device_libusb.h
	romloader_usb_main.h
	uuencoder.h
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.h
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.h
)

add_definitions(${USB_DEFINES})

add_library(romloader_usb_lib STATIC ${romloader_usb_SOURCES})
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.h
                            PROPERTIES GENERATED 1)

ADD_DEPENDENCIES(romloader_usb_lib netx_usb_firmware)


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(romloader_usb lua romloader_usb.i)
	SWIG_LINK_LIBRARIES(romloader_usb romloader_usb_lib ${LUA_LIBRARIES} ${USB_LIBRARIES} romloader_lib)
	set_property(TARGET romloader_usb PROPERTY PREFIX "")

	# Install the targets.
	INSTALL(TARGETS romloader_usb DESTINATION ${CMAKE_BINARY_DIR}/build/)
endif(${LUA51_FOUND})


#----------------------------------------------------------------------------
#
# Build the python plugin.
#

#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(romloader_usbmodule python romloader_usb.i)
#	swig_link_libraries(romloader_usbmodule romloader_usb_lib ${PYTHON_LIBRARIES} ${USB_LIBRARIES} romloader)
#	set_target_properties(_romloader_usbmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

