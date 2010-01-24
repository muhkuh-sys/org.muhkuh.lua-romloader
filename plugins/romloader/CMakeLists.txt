cmake_minimum_required(VERSION 2.8)


add_subdirectory(usb)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(romloader_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


SET_SOURCE_FILES_PROPERTIES(romloader.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(romloader.i PROPERTIES SWIG_FLAGS "")


SET(romloader_SOURCES
	romloader.cpp
)

SET(romloader_HEADERS
	romloader.h
)

SWIG_ADD_MODULE(romloader lua romloader.i ${romloader_SOURCES})
SWIG_LINK_LIBRARIES(romloader ${LUA_LIBRARIES} muhkuh_plugin_interface)
SET_TARGET_PROPERTIES(romloader PROPERTIES PREFIX "")
