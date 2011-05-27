cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_uart")

SET(romloader_uart_VERSION 0.0.1)


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
# Compile the netX UART firmware.
#

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.h
                   COMMAND ${PYTHON_EXECUTABLE} mbs/setup.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/netx
                   COMMENT "Build the netx uart firmware"
                   VERBATIM)

ADD_CUSTOM_TARGET(netx_uart_firmware
                  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

IF(UNIX)
	SET(romloader_uart_platform_SOURCES romloader_uart_device_linux.cpp)
	SET(romloader_uart_platform_HEADERS romloader_uart_device_linux.h)
	SET(romloader_uart_platform_LIBS rt)
ELSE(UNIX)
	IF(WIN32)
		SET(romloader_uart_platform_SOURCES romloader_uart_device_win.cpp)
		SET(romloader_uart_platform_HEADERS romloader_uart_device_win.h)
		SET(romloader_uart_platform_LIBS Setupapi.lib)
	ENDIF(WIN32)
ENDIF(UNIX)


set_source_files_properties(romloader_uart.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader_uart.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


SET(romloader_uart_SOURCES
	romloader_uart_device.cpp
	romloader_uart_main.cpp
	${romloader_uart_platform_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.c
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.c
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.c
)

SET(romloader_uart_HEADERS
	romloader_uart_device.h
	romloader_uart_main.h
	${romloader_uart_platform_HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.h
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.h
	${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.h
)


add_library(romloader_uart_lib STATIC ${romloader_uart_SOURCES})
target_link_libraries(romloader_uart_lib ${romloader_uart_platform_LIBS})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	SET_TARGET_PROPERTIES(romloader_uart_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_intram.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_intram.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.c
                            ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_intram.h
                            PROPERTIES GENERATED 1)

ADD_DEPENDENCIES(romloader_uart_lib netx_uart_firmware)


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(romloader_uart lua romloader_uart.i)
	SWIG_LINK_LIBRARIES(romloader_uart romloader_uart_lib ${LUA_LIBRARIES} romloader_lib)
	set_property(TARGET romloader_uart PROPERTY PREFIX "")

	# Install the targets.
	INSTALL(TARGETS romloader_uart DESTINATION ${INSTALL_DIR_LUA_MODULES})
endif(${LUA51_FOUND})


#----------------------------------------------------------------------------
#
# Build the python plugin.
#

#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(romloader_uartmodule python romloader_uart.i)
#	swig_link_libraries(romloader_uartmodule romloader_uart_lib ${PYTHON_LIBRARIES} ${USB_LIBRARIES} romloader)
#	set_target_properties(_romloader_uartmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

