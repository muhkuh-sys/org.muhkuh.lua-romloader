cmake_minimum_required(VERSION 2.8)

ADD_SUBDIRECTORY(openocd)


PROJECT("romloader_jtag")

SET(romloader_jtag_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})

if(${LUA_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA_FOUND})


#-----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

SET(romloader_jtag_SOURCES
	romloader_jtag_main.cpp
)

SET(romloader_jtag_HEADERS
	romloader_jtag_main.h
	romloader_jtag_openocd.h
)


ADD_LIBRARY(TARGET_romloader_jtag_lib STATIC ${romloader_jtag_SOURCES})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_jtag_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# The library depends on the openocd header.
ADD_DEPENDENCIES(TARGET_romloader_jtag_lib TARGET_romloader_jtag_openocd_lib TARGET_openocd)



#----------------------------------------------------------------------------
#
# Build the plugins.
#

SET_SOURCE_FILES_PROPERTIES(romloader_jtag.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_jtag.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/plugins)


IF(${LUA_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_jtag_LUA lua romloader_jtag.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_jtag_LUA TARGET_romloader_jtag_lib TARGET_romloader_jtag_openocd_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
	
	# Set the name of the output file to "romloader_jtag".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_jtag_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_jtag")
	
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# On minGW link all compiler libraries static.
		SET_PROPERTY(TARGET TARGET_plugins_romloader_romloader_jtag_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	
	# The module depends on the openocd libraries.
	ADD_DEPENDENCIES(TARGET_plugins_romloader_romloader_jtag_LUA TARGET_romloader_jtag_openocd_lib)
	
	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_romloader_jtag_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME romloader_jtag_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 $<TARGET_FILE:TARGET_plugins_romloader_romloader_jtag_LUA>)
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
ENDIF(${LUA_FOUND})

