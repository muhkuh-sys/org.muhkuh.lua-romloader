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

ADD_LIBRARY(TARGET_muhkuh_plugin_interface STATIC ${muhkuh_plugin_interface_SOURCES})
ADD_DEPENDENCIES(TARGET_muhkuh_plugin_interface swigluarun)
TARGET_LINK_LIBRARIES(TARGET_muhkuh_plugin_interface org.muhkuh.lua-lua51::TARGET_lualib)

# Set the name of the output file to "muhkuh_plugin_interface".
SET_PROPERTY(TARGET TARGET_muhkuh_plugin_interface PROPERTY OUTPUT_NAME "muhkuh_plugin_interface")

# On mingw link all compiler libraries static.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	set_property(TARGET TARGET_muhkuh_plugin_interface PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_muhkuh_plugin_interface PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# Install the targets.
#INSTALL(TARGETS TARGET_muhkuh_plugin_interface DESTINATION ${INSTALL_DIR_SHARED_OBJECTS})
