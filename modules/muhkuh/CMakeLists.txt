cmake_minimum_required(VERSION 2.8)

project("module_muhkuh")

set(module_muhkuh_VERSION 0.0.1)


find_package(SWIG REQUIRED)


include(${SWIG_USE_FILE})
include(${wxWidgets_USE_FILE})

if(${PYTHONLIBS_FOUND})
	include_directories(${PYTHON_INCLUDE_DIRS})
endif(${PYTHONLIBS_FOUND})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


set_source_files_properties(muhkuh.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties(muhkuh.i PROPERTIES SWIG_FLAGS "-I${CMAKE_HOME_DIRECTORY}/modules/typemaps")


set(module_muhkuh_SOURCES
	growbuffer.cpp
	muhkuh.cpp
	readFsFile.cpp
)

set(module_muhkuh_HEADERS
	growbuffer.h
	muhkuh.h
	readFsFile.h
)

# Build the library.
add_library(module_muhkuh_lib STATIC ${module_muhkuh_SOURCES})


if(${LUA51_FOUND})
	# Build the Lua module.
	swig_add_module(module_muhkuh lua muhkuh.i)
	swig_link_libraries(module_muhkuh module_muhkuh_lib ${wxWidgets_LIBRARIES} ${LUA_LIBRARIES})
	SET_TARGET_PROPERTIES(module_muhkuh PROPERTIES PREFIX "" OUTPUT_NAME "muhkuh")

	# Install the targets.
	INSTALL(TARGETS module_muhkuh DESTINATION ${CMAKE_BINARY_DIR}/build/)
endif(${LUA51_FOUND})


#if(${PYTHONLIBS_FOUND})
#	# Build the Python module.
#	swig_add_module(mhashmodule python mhash.i)
#	swig_link_libraries(mhashmodule mhash_lib ${PYTHON_LIBRARIES})
#	set_target_properties(_mhashmodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${MODULE_PYTHON_OUTPUT_PATH})
#endif(${PYTHONLIBS_FOUND})

