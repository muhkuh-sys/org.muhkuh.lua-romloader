cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_eth")


INCLUDE(${SWIG_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA_FOUND})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

IF(UNIX)
	SET(romloader_eth_platform_SOURCES romloader_eth_device_linux.cpp)
	SET(romloader_eth_platform_HEADERS romloader_eth_device_linux.h)
	SET(romloader_eth_platform_LIBS )
ELSE(UNIX)
	IF(WIN32)
		SET(romloader_eth_platform_SOURCES romloader_eth_device_win.cpp)
		SET(romloader_eth_platform_HEADERS romloader_eth_device_win.h)
		SET(romloader_eth_platform_LIBS ws2_32 iphlpapi)
	ENDIF(WIN32)
ENDIF(UNIX)


SET_SOURCE_FILES_PROPERTIES(romloader_eth.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_eth.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/plugins -I${CMAKE_BINARY_DIR}/version_information)


SET(romloader_eth_SOURCES
	romloader_eth_device.cpp
	romloader_eth_main.cpp
	${romloader_eth_platform_SOURCES}
)

SET(romloader_eth_HEADERS
	romloader_eth_device.h
	romloader_eth_main.h
	${romloader_eth_platform_HEADERS}
)


add_library(TARGET_romloader_eth_lib STATIC ${romloader_eth_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_eth_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

SET_SOURCE_FILES_PROPERTIES(${ETH_FIRMWARE_SOURCES} ${ETH_FIRMWARE_HEADERS}
                            PROPERTIES GENERATED 1)


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

if(${LUA_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_eth_LUA lua romloader_eth.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_eth_LUA TARGET_romloader_eth_lib ${LUA_LIBRARIES} ${romloader_eth_platform_LIBS} TARGET_plugins_romloader_LIB)

	# Set the name of the output file to "romloader_eth".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_eth_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_eth")

	# On mingw link all compiler libraries static.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		set_property(TARGET TARGET_plugins_romloader_romloader_eth_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_romloader_eth_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME romloader_eth_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_plugins_romloader_romloader_eth_LUA>)
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
endif(${LUA_FOUND})

