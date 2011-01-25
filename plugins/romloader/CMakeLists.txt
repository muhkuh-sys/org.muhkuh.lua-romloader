cmake_minimum_required(VERSION 2.8)


add_subdirectory(usb)
#add_subdirectory(uart)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(romloader_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})


INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})

set_source_files_properties(romloader.i PROPERTIES CPLUSPLUS ON)
set_property(SOURCE romloader.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)

SET(romloader_lib_SOURCES
	romloader.cpp
)

SET(romloader_lib_HEADERS
	romloader.h
)

add_library(romloader_lib STATIC ${romloader_lib_SOURCES})
target_link_libraries(romloader_lib ${LUA_LIBRARIES} muhkuh_plugin_interface)
set_target_properties(romloader_lib PROPERTIES PREFIX "")

if(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(romloader lua romloader.i)
	SWIG_LINK_LIBRARIES(romloader romloader_lib ${LUA_LIBRARIES})
	set_property(TARGET romloader PROPERTY PREFIX "")

	# Install the targets.
	INSTALL(TARGETS romloader DESTINATION ${INSTALL_DIR_LUA_MODULES})
endif(${LUA51_FOUND})


#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(romloadermodule python romloader.i)
#	swig_link_libraries(romloadermodule romloader_lib ${PYTHON_LIBRARIES})
#	set_target_properties(_romloadermodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

