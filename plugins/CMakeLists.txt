cmake_minimum_required(VERSION 2.8)


add_subdirectory(romloader)

#-----------------------------------------------------------------------------

PROJECT("muhkuh_plugin_interface")

SET(muhkuh_plugin_interface_VERSION 0.0.1)

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})

SET(muhkuh_plugin_interface_SOURCES
	muhkuh_plugin_interface.cpp
)

SET(muhkuh_plugin_interface_HEADERS
	swigluarun.h
	muhkuh_plugin_interface.h
)

ADD_LIBRARY(muhkuh_plugin_interface SHARED ${muhkuh_plugin_interface_SOURCES})
ADD_DEPENDENCIES(muhkuh_plugin_interface swigluarun)
TARGET_LINK_LIBRARIES(muhkuh_plugin_interface ${LUA_LIBRARIES})

# Install the targets.
INSTALL(TARGETS muhkuh_plugin_interface DESTINATION ${CMAKE_BINARY_DIR}/build/)
