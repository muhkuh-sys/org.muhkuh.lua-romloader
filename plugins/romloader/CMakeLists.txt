cmake_minimum_required(VERSION 2.8)


add_subdirectory(usb)
#add_subdirectory(uart)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(romloader_VERSION 0.0.1)


INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


SET(romloader_SOURCES
	romloader.cpp
)

SET(romloader_HEADERS
	romloader.h
)

add_library(romloader SHARED ${romloader_SOURCES})
target_link_libraries(romloader ${LUA_LIBRARIES} muhkuh_plugin_interface)
SET_TARGET_PROPERTIES(romloader PROPERTIES PREFIX "")
