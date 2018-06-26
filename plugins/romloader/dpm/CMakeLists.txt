CMAKE_MINIMUM_REQUIRED(VERSION 3.7)


PROJECT("romloader_dpm")


INCLUDE(${SWIG_USE_FILE})


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

# Right now this works only on linux platforms as the access is done with UIO (see https://www.kernel.org/doc/htmldocs/uio-howto/).
# Maybe the future brings support for the NXHX-FTDI and companions which allows easy access on windows too.
IF(UNIX)
	SET(romloader_dpm_SOURCES
		../../../targets/plugins/romloader/dpm/netx/dpmmon_netx56_monitor.c
		../../../targets/plugins/romloader/dpm/netx/dpmmon_netx4000_monitor.c
		romloader_dpm_device_linux_uio.cpp
		romloader_dpm_device.cpp
		romloader_dpm_main.cpp
		romloader_dpm_transfer.cpp
		romloader_dpm_transfer_netx56.cpp
		romloader_dpm_transfer_netx4000.cpp
	)

	SET(romloader_dpm_HEADERS
		romloader_dpm_main.h
	)

	ADD_LIBRARY(TARGET_romloader_dpm_lib STATIC ${romloader_dpm_SOURCES})
	TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_dpm_lib
	                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH} ${CMAKE_SOURCE_DIR}/platform/src/ ../../../targets/plugins/romloader/dpm/netx/)
	# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
	IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
		SET_TARGET_PROPERTIES(TARGET_romloader_dpm_lib PROPERTIES COMPILE_FLAGS -fPIC)
	ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))


	#----------------------------------------------------------------------------
	#
	# Build the lua plugin.
	#
	# Build the Lua module.
	SET_SOURCE_FILES_PROPERTIES(romloader_dpm.i PROPERTIES CPLUSPLUS ON)
	SET_PROPERTY(SOURCE romloader_dpm.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY} -I${CMAKE_BINARY_DIR}/version_information)
	SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_dpm_LUA lua romloader_dpm.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_dpm_LUA TARGET_romloader_dpm_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
	TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_romloader_dpm_LUA
	                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

	# Set the name of the output file to "romloader_dpm".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_dpm_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_dpm")

	# On mingw link all compiler libraries static.
	IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		set_property(TARGET TARGET_plugins_romloader_romloader_dpm_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
	ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_romloader_dpm_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME romloader_dpm_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${TEST_SCRIPT_DIR}/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_plugins_romloader_romloader_dpm_LUA>)
	ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
ENDIF(UNIX)
