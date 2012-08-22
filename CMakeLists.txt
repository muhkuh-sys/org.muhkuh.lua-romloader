cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")


#----------------------------------------------------------------------------
#
# Read the version file and parse it.
#

# Set the filename for the muhkuh version file.
SET(MUHKUH_VERSION_FILE "${CMAKE_HOME_DIRECTORY}/muhkuh_version.txt")
IF(EXISTS ${MUHKUH_VERSION_FILE})
	# Set the regular expression for the muhkuh version. It is a sequence of 3 numbers separated by dots.
	SET(MUHKUH_VERSION_REXEX "([0-9]+)\\.([0-9]+)\\.([0-9]+)")
	# Read the contents of the muhkuh version file. Accept only data matching the version regex.
	FILE(STRINGS ${MUHKUH_VERSION_FILE} MUHKUH_VERSION_ALL REGEX ${MUHKUH_VERSION_REXEX})
	# Check if a valid version number was found.
	IF("${MUHKUH_VERSION_ALL}" STREQUAL "")
		# No valid version number found. The file contains garbage, which is a fatal error!
		MESSAGE(FATAL_ERROR "The file ${MUHKUH_VERSION_FILE} contains no valid version in the form number.number.number .")
	ELSE("${MUHKUH_VERSION_ALL}" STREQUAL "")
		STRING(REGEX MATCH "${MUHKUH_VERSION_REXEX}" MUHKUH_VERSION_ALL ${MUHKUH_VERSION_ALL})
		SET(MUHKUH_VERSION_MAJ ${CMAKE_MATCH_1})
		SET(MUHKUH_VERSION_MIN ${CMAKE_MATCH_2})
		SET(MUHKUH_VERSION_SUB ${CMAKE_MATCH_3})
		MESSAGE("File ${MUHKUH_VERSION_FILE} found, version info: ${MUHKUH_VERSION_MAJ}.${MUHKUH_VERSION_MIN}.${MUHKUH_VERSION_SUB}")
	ENDIF("${MUHKUH_VERSION_ALL}" STREQUAL "")
ELSE(EXISTS ${MUHKUH_VERSION_FILE})
	SET(MUHKUH_VERSION_MAJ 1)
	SET(MUHKUH_VERSION_MIN 0)

	FIND_PACKAGE(Subversion)
	IF(EXISTS "${CMAKE_HOME_DIRECTORY}/.svn" AND ${Subversion_FOUND})
		MESSAGE("Subversion data found, generating version info from revision number.")
		Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} MUHKUH_SVN)
		MESSAGE("Current revision is ${MUHKUH_SVN_WC_REVISION}")
		SET(MUHKUH_VERSION_SUB ${MUHKUH_SVN_WC_REVISION})
	ELSE(EXISTS "${CMAKE_HOME_DIRECTORY}/.svn" AND ${Subversion_FOUND})
		MESSAGE(WARNING "No file ${MUHKUH_VERSION_FILE} and no subversion data found, using a fixed version.")
		SET(MUHKUH_VERSION_SUB 0)
	ENDIF(EXISTS "${CMAKE_HOME_DIRECTORY}/.svn" AND ${Subversion_FOUND})
ENDIF(EXISTS ${MUHKUH_VERSION_FILE})

# TODO: Generate this, but how? There is no cmake function which returns the date.
SET(MUHKUH_VERSION_YEAR 2011)


#----------------------------------------------------------------------------
#
# Set some installation folders.
#

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
	SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
	SET(INSTALL_DIR_EXECUTABLES "build")
	SET(INSTALL_DIR_LUA_MODULES "build/lua_plugins")
	SET(INSTALL_DIR_PYTHON_MODULES "build/python_plugins")
	SET(INSTALL_DIR_SHARED_OBJECTS "build")
	SET(INSTALL_DIR_LUA_SCRIPTS "build/lua")
#ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

#----------------------------------------------------------------------------
#
# Find all packages.
#

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# The python interpreter is required to build the netx firmware. (See plugins/romloader/usb/netx)
FIND_PACKAGE(PythonInterp)

# This is the list of the wxwidgets modules required for the application.
# Use static linking.
SET(wxWidgets_STATIC ON)
SET(wxWidgets_USE_STATIC ON)
FIND_PACKAGE(wxWidgets 2.9 REQUIRED html adv aui net xml core base)

# TODO: Show a summary with FeatureSummary .


#----------------------------------------------------------------------------
#
# Muhkuh can build modules for the Lua language. This section allows to user
# to choose which lua version should be used - the one bundled with muhkuh,
# or a systemwide installed lua.
#
# The default is to use the systemwide lua.
#

OPTION(USE_SYSTEM_LUA "Use the systemwide installed lua version instead of the muhkuh internal one." ON)
IF(USE_SYSTEM_LUA)
	# Use the system version.
	FIND_PACKAGE(Lua51 REQUIRED)
ELSE(USE_SYSTEM_LUA)
	# Use the internal lua version.
	SET(LUA51_FOUND TRUE)
	SET(LUA_LIBRARIES lualib)
	SET(LUA_INCLUDE_DIR ${CMAKE_HOME_DIRECTORY}/lua/lib/lua-5.1.4/src/)
ENDIF(USE_SYSTEM_LUA)



ADD_CUSTOM_TARGET(Firmware
	ALL
	COMMAND "${PYTHON_EXECUTABLE}" mbs/setup.py
	WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
	COMMENT "Build the swig lua runtime header."
	VERBATIM
)


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
# Set custom definitions for microsoft compiler.
#

if(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")
	# Do not show msvs security warnings about unsafe functions.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")

if(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv)")
	# Set warning level to 2.
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
