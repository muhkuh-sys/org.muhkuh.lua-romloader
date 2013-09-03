cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")


# Generate a version number from a file or VCS.
INCLUDE(cmake/muhkuh_version.cmake)


#----------------------------------------------------------------------------
#
# Set some installation folders.
#

SET(CPACK_PACKAGE_VERSION_MAJOR "${MUHKUH_VERSION_MAJ}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MUHKUH_VERSION_MIN}")
SET(CPACK_PACKAGE_VERSION_PATCH "${MUHKUH_VERSION_SUB}")

STRING(TOLOWER ${CMAKE_SYSTEM_NAME} POM_ID_OS)
IF(NOT DEFINED TARGET_PROCESSOR)
	SET(TARGET_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
ENDIF(NOT DEFINED TARGET_PROCESSOR)
SET(POM_ID_PROCESSOR ${TARGET_PROCESSOR})
SET(POM_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
STRING(REPLACE . _ POM_ID_VERSION ${POM_VERSION})

SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}_${POM_ID_OS}_${POM_ID_PROCESSOR}-${POM_VERSION})

IF(UNIX AND NOT WIN32 AND NOT APPLE)
	SET(CPACK_GENERATOR "TBZ2")
ELSE(UNIX AND NOT WIN32 AND NOT APPLE)
	SET(CPACK_GENERATOR "ZIP")
ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

INCLUDE(CPack)

# This is the new dir structure, use it for packaging.
#IF(UNIX AND NOT WIN32 AND NOT APPLE)
#	SET(INSTALL_DIR_EXECUTABLES "bin")
#	SET(INSTALL_DIR_LUA_MODULES "lib/lua/5.1")
#	IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
#		SET(INSTALL_DIR_SHARED_OBJECTS "lib64")
#	ELSE(CMAKE_SIZEOF_VOID_P MATCHES "8")
#		SET(INSTALL_DIR_SHARED_OBJECTS "lib")
#	ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
#	SET(INSTALL_DIR_LUA_SCRIPTS "lib/lua/5.1")
#ELSE(UNIX AND NOT WIN32 AND NOT APPLE)
	# Install everything in one folder on Win.
	SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/muhkuh_${POM_ID_VERSION})
	SET(INSTALL_DIR_EXECUTABLES "bin")
	SET(INSTALL_DIR_LUA_MODULES "lua_plugins")
	SET(INSTALL_DIR_PYTHON_MODULES "python_plugins")
	SET(INSTALL_DIR_SHARED_OBJECTS "bin")
	SET(INSTALL_DIR_LUA_SCRIPTS "lua")
#ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

#----------------------------------------------------------------------------
#
# Find all packages.
#

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# TODO: Show a summary with FeatureSummary .


# Select the LUA version.
INCLUDE(cmake/muhkuh_lua.cmake)


#----------------------------------------------------------------------------
#
# Add a custom command and a custom target for the swig runtime includes.
# The runtimes are generated during the build process by running the swig
# executable.
#

# Create a new directory for the swig runtime.
SET(SWIG_RUNTIME_OUTPUT_PATH ${CMAKE_BINARY_DIR}/swig_runtime)
MAKE_DIRECTORY(${SWIG_RUNTIME_OUTPUT_PATH})

# TODO: Replace this and all other custom commands with ExternalProject ?
ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h
	COMMAND "${SWIG_EXECUTABLE}" -lua -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h
	COMMENT "Build the swig lua runtime header."
	VERBATIM
)

ADD_CUSTOM_TARGET(swigluarun DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h)


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
#

#add_subdirectory(application)
add_subdirectory(lua)
add_subdirectory(modules)
add_subdirectory(plugins)


#----------------------------------------------------------------------------

