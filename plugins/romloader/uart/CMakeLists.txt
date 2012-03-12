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

SET(UART_FIRMWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_bootstrap.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_bootstrap.c)

SET(UART_FIRMWARE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap_run.h)

ADD_CUSTOM_COMMAND(OUTPUT ${UART_FIRMWARE_SOURCES} ${UART_FIRMWARE_HEADERS}
                   COMMAND ${PYTHON_EXECUTABLE} mbs/setup.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/netx
                   COMMENT "Build the netx uart firmware"
                   VERBATIM)

ADD_CUSTOM_TARGET(netx_uart_firmware 
                  DEPENDS ${UART_FIRMWARE_SOURCES} ${UART_FIRMWARE_HEADERS})


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
	${UART_FIRMWARE_SOURCES}
)

SET(romloader_uart_HEADERS
	romloader_uart_device.h
	romloader_uart_main.h
	${romloader_uart_platform_HEADERS}
	${UART_FIRMWARE_HEADERS}
)


add_library(romloader_uart_lib STATIC ${romloader_uart_SOURCES})
target_link_libraries(romloader_uart_lib ${romloader_uart_platform_LIBS})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(romloader_uart_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

SET_SOURCE_FILES_PROPERTIES(${UART_FIRMWARE_SOURCES} ${UART_FIRMWARE_HEADERS}
                            PROPERTIES GENERATED 1)

ADD_DEPENDENCIES(romloader_uart_lib netx_uart_firmware)


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(plugins_romloader_romloader_uart_LUA lua romloader_uart.i)
	SWIG_LINK_LIBRARIES(plugins_romloader_romloader_uart_LUA romloader_uart_lib ${LUA_LIBRARIES} plugins_romloader_LIB)

	# Rename the output file to "romloader_uart".
	SET_PROPERTY(TARGET plugins_romloader_romloader_uart_LUA PROPERTY PREFIX "")
	SET_PROPERTY(TARGET plugins_romloader_romloader_uart_LUA PROPERTY OUTPUT_NAME "romloader_uart")

	# Install the lua module.
	INSTALL(TARGETS plugins_romloader_romloader_uart_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})
ENDIF(${LUA51_FOUND})


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

