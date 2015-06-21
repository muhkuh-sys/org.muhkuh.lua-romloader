cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")


# Enable tests (see here for details: http://www.vtk.org/Wiki/CMake/Testing_With_CTest)
ENABLE_TESTING()

# Generate a version number from a file or VCS.
INCLUDE(cmake/muhkuh_version.cmake)


# Set some installation folders.
INCLUDE(cmake/muhkuh_installation_folders.cmake)

#----------------------------------------------------------------------------
#
# Find all packages.
#

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# Python is used for some test scripts.
FIND_PACKAGE(PythonInterp REQUIRED)

# TODO: Show a summary with FeatureSummary .


# Select the LUA version.
INCLUDE(cmake/muhkuh_lua.cmake)

# Add a custom target for the swig runtime for LUA.
INCLUDE(cmake/muhkuh_swig_lua_runtime.cmake)


#----------------------------------------------------------------------------
#
# Set custom definitions for several compiler.
#

# Microsoft: Do not show msvs security warnings about unsafe functions.
if(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")


# Microsoft: Set warning level to 2.
if(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv)")
	add_definitions(/W2)
ENDIF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv)")


#----------------------------------------------------------------------------
#
# Include the projects in the subdirectories.
# NOTE: build the external components first. Many parts depend on them.
#

add_subdirectory(external)

add_subdirectory(lua)
add_subdirectory(modules)
add_subdirectory(plugins)


#----------------------------------------------------------------------------

