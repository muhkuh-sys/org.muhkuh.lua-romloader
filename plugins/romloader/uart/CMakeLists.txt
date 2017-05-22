cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_uart")

INCLUDE(${SWIG_USE_FILE})


#----------------------------------------------------------------------------
#
# Compile the netX UART firmware.
#

SET(UART_FIRMWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx56_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_bootstrap.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap.c)

SET(UART_FIRMWARE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx56_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx56_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx500_monitor_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_bootstrap.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx10_bootstrap_run.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/uartmon_netx50_bootstrap_run.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

IF(UNIX)
	SET(romloader_uart_platform_SOURCES romloader_uart_device_linux.cpp)
	SET(romloader_uart_platform_HEADERS romloader_uart_device_linux.h)
	SET(romloader_uart_platform_LIBS rt pthread)
ELSE(UNIX)
	IF(WIN32)
		SET(romloader_uart_platform_SOURCES romloader_uart_device_win.cpp)
		SET(romloader_uart_platform_HEADERS romloader_uart_device_win.h)
		SET(romloader_uart_platform_LIBS setupapi.lib)
	ENDIF(WIN32)
ENDIF(UNIX)


SET(romloader_uart_SOURCES
	romloader_uart_device.cpp
	romloader_uart_main.cpp
	romloader_uart_read_functinoid_aboot.cpp
	romloader_uart_read_functinoid_hboot1.cpp
	romloader_uart_read_functinoid_mi1.cpp
	${romloader_uart_platform_SOURCES}
	${UART_FIRMWARE_SOURCES}
)

SET(romloader_uart_HEADERS
	romloader_uart_device.h
	romloader_uart_main.h
	romloader_uart_read_functinoid_aboot.h
	romloader_uart_read_functinoid_hboot1.h
	romloader_uart_read_functinoid_mi1.h
	${romloader_uart_platform_HEADERS}
	${UART_FIRMWARE_HEADERS}
)


ADD_LIBRARY(TARGET_romloader_uart_lib STATIC ${romloader_uart_SOURCES})
TARGET_LINK_LIBRARIES(TARGET_romloader_uart_lib ${romloader_uart_platform_LIBS})
TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_uart_lib
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_uart_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#
SET_SOURCE_FILES_PROPERTIES(romloader_uart.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_uart.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY} -I${CMAKE_BINARY_DIR}/version_information)
SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_uart_LUA lua romloader_uart.i)
SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_uart_LUA TARGET_romloader_uart_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_romloader_uart_LUA
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set the name of the output file to "romloader_uart".
SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_uart_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_uart")

# On mingw link all compiler libraries static.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	SET_PROPERTY(TARGET TARGET_plugins_romloader_romloader_uart_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# Install the lua module.
INSTALL(TARGETS TARGET_plugins_romloader_romloader_uart_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Add tests for this module.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# Here are the MinGW specific tests.
	ADD_TEST(NAME romloader_uart_MinGW_DLL_dependencies
	         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_plugins_romloader_romloader_uart_LUA>)
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
