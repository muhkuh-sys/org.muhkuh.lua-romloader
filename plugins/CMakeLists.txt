CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("muhkuh_plugin_interface")

# Select the LUA version.
INCLUDE(${CMAKE_HOME_DIRECTORY}/../cmake/muhkuh_lua.cmake)

# Enable tests (see here for details: http://www.vtk.org/Wiki/CMake/Testing_With_CTest)
ENABLE_TESTING()


# Set some installation folders.
INCLUDE(${CMAKE_HOME_DIRECTORY}/../cmake/muhkuh_installation_folders.cmake)

# Get the VCS version for the jonchki configuration.
INCLUDE(${CMAKE_HOME_DIRECTORY}/../cmake/version.cmake)

# Generate a version number from a file or VCS.
INCLUDE(${CMAKE_HOME_DIRECTORY}/../cmake/muhkuh_version.cmake)

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# Add a custom target for the swig runtime for LUA.
INCLUDE(${CMAKE_HOME_DIRECTORY}/../cmake/muhkuh_swig_lua_runtime.cmake)


#---------------------------------------------------------------------
#
# Create an include file with all version information.
#
CONFIGURE_FILE(templates/version_information.h
               ${CMAKE_BINARY_DIR}/version_information/version_information.h
               @ONLY)
CONFIGURE_FILE(templates/version_information.i
               ${CMAKE_BINARY_DIR}/version_information/version_information.i
               @ONLY)


ADD_SUBDIRECTORY(romloader)

#-----------------------------------------------------------------------------

SET(muhkuh_plugin_interface_SOURCES
	muhkuh_plugin_interface.cpp
)

SET(muhkuh_plugin_interface_HEADERS
	swigluarun.h
	muhkuh_plugin_interface.h
)

ADD_LIBRARY(TARGET_muhkuh_plugin_interface STATIC
            EXCLUDE_FROM_ALL
            ${muhkuh_plugin_interface_SOURCES})
ADD_DEPENDENCIES(TARGET_muhkuh_plugin_interface TARGET_swigluarun)
TARGET_INCLUDE_DIRECTORIES(TARGET_muhkuh_plugin_interface
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})
TARGET_LINK_LIBRARIES(TARGET_muhkuh_plugin_interface ${LUA_LIBRARIES})

# Set the name of the output file to "muhkuh_plugin_interface".
SET_PROPERTY(TARGET TARGET_muhkuh_plugin_interface PROPERTY OUTPUT_NAME "muhkuh_plugin_interface")

# On mingw link all compiler libraries static.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	SET_PROPERTY(TARGET TARGET_muhkuh_plugin_interface PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_muhkuh_plugin_interface PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

