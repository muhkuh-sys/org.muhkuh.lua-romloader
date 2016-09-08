cmake_minimum_required(VERSION 2.8)


#ADD_SUBDIRECTORY(dpm)
ADD_SUBDIRECTORY(eth)
ADD_SUBDIRECTORY(jtag)
ADD_SUBDIRECTORY(papa_schlumpf)
ADD_SUBDIRECTORY(usb)
ADD_SUBDIRECTORY(uart)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(romloader_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

IF(${PYTHONLIBS_FOUND})
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
ENDIF(${PYTHONLIBS_FOUND})

IF(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
ENDIF(${LUA51_FOUND})


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})

set_source_files_properties(romloader.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/plugins)

SET(plugins_romloader_lib_SOURCES
	romloader.cpp
	uuencoder.cpp
)

SET(plugins_romloader_lib_HEADERS
	romloader.h
	uuencoder.h
)

ADD_LIBRARY(TARGET_plugins_romloader_LIB STATIC ${plugins_romloader_lib_SOURCES})
TARGET_LINK_LIBRARIES(TARGET_plugins_romloader_LIB TARGET_muhkuh_plugin_interface)

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_LIB PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")


IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(TARGET_plugins_romloader_LUA lua romloader.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_LUA TARGET_plugins_romloader_LIB ${LUA_LIBRARIES})

	# Set the name of the output file to "romloader".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader")

	# On mingw link all compiler libraries static.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		set_property(TARGET TARGET_plugins_romloader_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME romloader_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 $<TARGET_FILE:TARGET_plugins_romloader_LUA>)
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
ENDIF(${LUA51_FOUND})


#IF(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	SWIG_ADD_MODULE(plugins_romloader_PYTHON python romloader.i)
#	SWIG_LINK_LIBRARIES(plugins_romloader_PYTHON plugins_romloader_LIB ${PYTHON_LIBRARIES})
#	set_target_properties(_romloadermodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#ENDIF(${PYTHONLIBS_FOUND})

