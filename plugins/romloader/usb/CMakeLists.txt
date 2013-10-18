cmake_minimum_required(VERSION 2.8)

ADD_SUBDIRECTORY(libusbx)

MESSAGE("LIBUSBX_LIBRARIES: ${LIBUSBX_LIBRARIES}")
MESSAGE("LIBUSBX_DEFINES: ${LIBUSBX_DEFINES}")
MESSAGE("LIBUSBX_INCLUDE_DIRECTORIES: ${LIBUSBX_INCLUDE_DIRECTORIES}")
MESSAGE("LIBUSBX_LINKER_FLAGS: ${LIBUSBX_LINKER_FLAGS}")


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
# Get the netX USB firmware binaries (in form of C data arrays) and headers.
#

SET(NETX_USB_FIRMWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx56.c
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500.c)

SET(NETX_USB_FIRMWARE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx10.h
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx56.h
                         ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/usbmon_netx500.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

# Append the include directories for libusbx.
INCLUDE_DIRECTORIES(${LIBUSBX_INCLUDE_DIRECTORIES})

# Append the libusbx defines.
ADD_DEFINITIONS(${LIBUSBX_DEFINES})


SET_SOURCE_FILES_PROPERTIES(romloader_usb.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_usb.i PROPERTY SWIG_FLAGS ${LIBUSBX_DEFINES} -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


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
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_usb_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# The library depends on the libusbx header.
ADD_DEPENDENCIES(TARGET_romloader_usb_lib TARGET_libusbx)



#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_usb_LUA lua romloader_usb.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_usb_LUA TARGET_romloader_usb_lib ${LUA_LIBRARIES} ${LIBUSBX_LIBRARIES} TARGET_plugins_romloader_LIB)

	# Set the name of the output file to "romloader_usb".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_usb_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_usb")

	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# On minGW link all compiler libraries static.
		set_property(TARGET TARGET_plugins_romloader_romloader_usb_LUA PROPERTY LINK_FLAGS "${LIBUSBX_LINKER_FLAGS} -static-libgcc -static-libstdc++")
	ELSE((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		set_property(TARGET TARGET_plugins_romloader_romloader_usb_LUA PROPERTY LINK_FLAGS "${LIBUSBX_LINKER_FLAGS}")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# The module depends on the libusbx libraries.
	ADD_DEPENDENCIES(TARGET_plugins_romloader_romloader_usb_LUA TARGET_libusbx)
	
	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_romloader_usb_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})
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

