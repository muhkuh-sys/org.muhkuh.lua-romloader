cmake_minimum_required(VERSION 2.8)


add_subdirectory(romloader)

#-----------------------------------------------------------------------------

PROJECT("muhkuh_plugin_interface")

SET(muhkuh_plugin_interface_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


SET_SOURCE_FILES_PROPERTIES(muhkuh_plugin_interface.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(muhkuh_plugin_interface.i PROPERTIES SWIG_FLAGS "")


SET(muhkuh_plugin_interface_SOURCES
	muhkuh_plugin_interface.cpp
)

SET(muhkuh_plugin_interface_HEADERS
	swigluarun.h
	muhkuh_plugin_interface.h
)

SWIG_ADD_MODULE(muhkuh_plugin_interface lua muhkuh_plugin_interface.i ${muhkuh_plugin_interface_SOURCES})
SWIG_LINK_LIBRARIES(muhkuh_plugin_interface ${LUA_LIBRARIES})
SET_TARGET_PROPERTIES(muhkuh_plugin_interface PROPERTIES PREFIX "")
add_dependencies(muhkuh_plugin_interface SWIG_LUA_RUNTIME)
