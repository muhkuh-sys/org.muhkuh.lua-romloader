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

SET(USB_FIRMWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.c)

SET(USB_FIRMWARE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10_intram.h
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500_intram.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

IF(UNIX)
	# Use libusb-0
	SET(USB_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/linux/lib/libusb-1.0.a -pthread -lrt)
	SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/linux/include/)
ELSE(UNIX)
	IF(WIN32)
		# Use libusb-0 on Windows to get a proper reset.
		# NOTE: The current version of libusb-1.0 on windows uses the driver winusb.
		#       It does not allow a complete device reset.
		SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=0)
		INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/include/)
		# LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/lib/msvc)
		# SET(USB_LIBRARIES libusb setupapi)
	ENDIF(WIN32)
ENDIF(UNIX)


SET_SOURCE_FILES_PROPERTIES(romloader_usb.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_usb.i PROPERTY SWIG_FLAGS ${USB_DEFINES} -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


SET(romloader_usb_SOURCES
	romloader_usb_device.cpp
	romloader_usb_device_libusb.cpp
	romloader_usb_main.cpp
	${USB_FIRMWARE_SOURCES}
)

SET(romloader_usb_HEADERS
	romloader_usb_device_libusb.h
	romloader_usb_main.h
	${USB_FIRMWARE_HEADERS}
)

ADD_DEFINITIONS(${USB_DEFINES})

ADD_LIBRARY(romloader_usb_lib STATIC ${romloader_usb_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(romloader_usb_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(plugins_romloader_romloader_usb_LUA lua romloader_usb.i)
	SWIG_LINK_LIBRARIES(plugins_romloader_romloader_usb_LUA romloader_usb_lib ${LUA_LIBRARIES} ${USB_LIBRARIES} plugins_romloader_LIB)

	# Rename the output file to "romloader_usb".
	SET_PROPERTY(TARGET plugins_romloader_romloader_usb_LUA PROPERTY PREFIX "")
	SET_PROPERTY(TARGET plugins_romloader_romloader_usb_LUA PROPERTY OUTPUT_NAME "romloader_usb")

	# Install the lua module.
	INSTALL(TARGETS plugins_romloader_romloader_usb_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})
ENDIF(${LUA51_FOUND})


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

