cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_dpm")

SET(romloader_dpm_VERSION 0.0.1)


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
# Include the netX UART firmware.
#
#SET(DPM_FIRMWARE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/dpmmon_netx56.c)
#SET(DPM_FIRMWARE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/netx/targets/dpmmon_netx56.h)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

# Right now this works only on linux platforms as the access is done with UIO (see https://www.kernel.org/doc/htmldocs/uio-howto/).
# Maybe the future brings support for the NXHX-FTDI and companions which allows easy access on windows too.
IF(UNIX)
	SET_SOURCE_FILES_PROPERTIES(romloader_dpm.i PROPERTIES CPLUSPLUS ON)
	SET_PROPERTY(SOURCE romloader_dpm.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)
	
	
	SET(romloader_dpm_SOURCES
		netx/targets/dpmmon_netx56_monitor.c
		romloader_dpm_device_linux_uio.cpp
		romloader_dpm_device.cpp
		romloader_dpm_main.cpp
		romloader_dpm_transfer.cpp
		romloader_dpm_transfer_netx56.cpp
	)
	
	SET(romloader_dpm_HEADERS
		romloader_dpm_main.h
	)
	
	
	ADD_LIBRARY(TARGET_romloader_dpm_lib STATIC ${romloader_dpm_SOURCES})
#	TARGET_LINK_LIBRARIES(TARGET_romloader_dpm_lib ${romloader_dpm_platform_LIBS})
	# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
		SET_TARGET_PROPERTIES(TARGET_romloader_dpm_lib PROPERTIES COMPILE_FLAGS -fPIC)
	ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	
	
	#----------------------------------------------------------------------------
	#
	# Build the lua plugin.
	#
	
	IF(${LUA51_FOUND})
		# Build the Lua module.
		SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_dpm_LUA lua romloader_dpm.i)
		SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_dpm_LUA TARGET_romloader_dpm_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
		
		# Set the name of the output file to "romloader_dpm".
		SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_dpm_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_dpm")
		
		# On mingw link all compiler libraries static.
		IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
			set_property(TARGET TARGET_plugins_romloader_romloader_dpm_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
		ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		
		# Install the lua module.
		INSTALL(TARGETS TARGET_plugins_romloader_romloader_dpm_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})
		
		# Add tests for this module.
		IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
			# Here are the MinGW specific tests.
			ADD_TEST(NAME romloader_dpm_MinGW_DLL_dependencies
			         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 $<TARGET_FILE:TARGET_plugins_romloader_romloader_dpm_LUA>)
		ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	ENDIF(${LUA51_FOUND})
ENDIF(UNIX)

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

