cmake_minimum_required(VERSION 2.8)


# The ethernet driver is not ported to windows yet.
IF(UNIX)
	add_subdirectory(eth)
ENDIF(UNIX)
add_subdirectory(usb)
add_subdirectory(uart)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(romloader_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

IF(${PYTHONLIBS_FOUND})
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
ENDIF(${PYTHONLIBS_FOUND})

IF(${LUA51_FOUND})
	INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
ENDIF(${LUA51_FOUND})


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})

set_source_files_properties(romloader.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)

SET(plugins_romloader_lib_SOURCES
	romloader.cpp
	uuencoder.cpp
)

SET(plugins_romloader_lib_HEADERS
	romloader.h
	uuencoder.h
)

ADD_LIBRARY(plugins_romloader_LIB STATIC ${plugins_romloader_lib_SOURCES})
TARGET_LINK_LIBRARIES(plugins_romloader_LIB muhkuh_plugin_interface)

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	SET_TARGET_PROPERTIES(plugins_romloader_LIB PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL GNU)


IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(plugins_romloader_LUA lua romloader.i)
	SWIG_LINK_LIBRARIES(plugins_romloader_LUA plugins_romloader_LIB ${LUA_LIBRARIES})

	# Rename the output file to "romloader".
	SET_PROPERTY(TARGET plugins_romloader_LUA PROPERTY PREFIX "")
	SET_PROPERTY(TARGET plugins_romloader_LUA PROPERTY OUTPUT_NAME "romloader")

	# Install the lua module.
	INSTALL(TARGETS plugins_romloader_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})
ENDIF(${LUA51_FOUND})


#IF(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	SWIG_ADD_MODULE(plugins_romloader_PYTHON python romloader.i)
#	SWIG_LINK_LIBRARIES(plugins_romloader_PYTHON plugins_romloader_LIB ${PYTHON_LIBRARIES})
#	set_target_properties(_romloadermodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#ENDIF(${PYTHONLIBS_FOUND})

