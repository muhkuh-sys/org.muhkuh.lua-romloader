cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_eth")

SET(romloader_eth_VERSION 0.0.1)


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
# Set the platform specific parts.
#

IF(UNIX)
	SET(romloader_eth_platform_SOURCES romloader_eth_device_linux.cpp)
	SET(romloader_eth_platform_HEADERS romloader_eth_device_linux.h)
ELSE(UNIX)
	IF(WIN32)
		SET(romloader_eth_platform_SOURCES romloader_eth_device_win.cpp)
		SET(romloader_eth_platform_HEADERS romloader_eth_device_win.h)
	ENDIF(WIN32)
ENDIF(UNIX)


SET_SOURCE_FILES_PROPERTIES(romloader_eth.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_eth.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)


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


add_library(romloader_eth_lib STATIC ${romloader_eth_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	SET_TARGET_PROPERTIES(romloader_eth_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)

SET_SOURCE_FILES_PROPERTIES(${ETH_FIRMWARE_SOURCES} ${ETH_FIRMWARE_HEADERS}
                            PROPERTIES GENERATED 1)


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(plugins_romloader_romloader_eth_LUA lua romloader_eth.i)
	SWIG_LINK_LIBRARIES(plugins_romloader_romloader_eth_LUA romloader_eth_lib ${LUA_LIBRARIES} plugins_romloader_LIB)

	# Install the lua module.
	GET_TARGET_PROPERTY(plugins_romloader_romloader_eth_LUA_LOCATION ${SWIG_MODULE_plugins_romloader_romloader_eth_LUA_REAL_NAME} LOCATION)
	INSTALL(FILES ${plugins_romloader_romloader_eth_LUA_LOCATION} DESTINATION ${INSTALL_DIR_LUA_MODULES} RENAME romloader_eth${CMAKE_SHARED_MODULE_SUFFIX})
endif(${LUA51_FOUND})

