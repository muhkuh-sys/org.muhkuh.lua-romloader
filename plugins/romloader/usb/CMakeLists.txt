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


IF(UNIX)
	# Use libusb-0.1
	SET(USB_LIBRARIES usb)
	SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=0)

	# Use libusb-1.0
	#SET(USB_LIBRARIES usb-1.0)
	#SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=1)
ELSE(UNIX)
	IF(WIN32)
		# Win has only libusb-0.1 .
		SET(USB_DEFINES -DROMLOADER_USB_LIBUSB_VERSION=0)
		INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/include/)
		LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/win/lib/msvc/)
		SET(USB_LIBRARIES libusb setupapi)
	ENDIF(WIN32)
ENDIF(UNIX)


set_source_files_properties(romloader_usb.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader_usb.i PROPERTY SWIG_FLAGS ${USB_DEFINES} -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


SET(romloader_usb_SOURCES
	romloader_usb_device_libusb0.cpp
	romloader_usb_main.cpp
)

SET(romloader_usb_HEADERS
	romloader_usb_device_libusb0.h
	romloader_usb_main.h
)

add_definitions(${USB_DEFINES})

add_library(romloader_usb_lib STATIC ${romloader_usb_SOURCES})


if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(romloader_usb lua romloader_usb.i)
	SWIG_LINK_LIBRARIES(romloader_usb romloader_usb_lib ${LUA_LIBRARIES} ${USB_LIBRARIES} romloader_lib)
	set_property(TARGET romloader_usb PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MODULE_LUA_OUTPUT_PATH})
	set_property(TARGET romloader_usb PROPERTY PREFIX "")
endif(${LUA51_FOUND})


#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(romloader_usbmodule python romloader_usb.i)
#	swig_link_libraries(romloader_usbmodule romloader_usb_lib ${PYTHON_LIBRARIES} ${USB_LIBRARIES} romloader)
#	set_target_properties(_romloader_usbmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

