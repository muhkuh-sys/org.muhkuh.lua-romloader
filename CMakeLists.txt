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

	FIND_PACKAGE(Hg)
	IF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg" AND HG_FOUND)
		MESSAGE("Mercurial data found, generating version info from revision number.")
		EXECUTE_PROCESS(COMMAND ${HG_EXECUTABLE} id -i ${CMAKE_CURRENT_SOURCE_DIR}
		                OUTPUT_VARIABLE MUHKUH_VERSION_SUB
		                ERROR_QUIET
		                OUTPUT_STRIP_TRAILING_WHITESPACE)
		MESSAGE("Current revision is ${MUHKUH_VERSION_SUB}")
	ELSE(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg" AND HG_FOUND)
		MESSAGE(WARNING "No file ${MUHKUH_VERSION_FILE} and no mercurial data found, using a fixed version.")
		SET(MUHKUH_VERSION_SUB 0)
	ENDIF(EXISTS "${CMAKE_HOME_DIRECTORY}/.hg" AND HG_FOUND)
ENDIF(EXISTS ${MUHKUH_VERSION_FILE})

# Get the current year.
# There seems to be no built-in way in cmake to get the current year.
# But we have python for the MBS build system anyway so we can use it.
FIND_PACKAGE(PythonInterp REQUIRED)
EXECUTE_PROCESS(COMMAND "${PYTHON_EXECUTABLE}" -c "import datetime; print datetime.date.today().year"
                OUTPUT_VARIABLE MUHKUH_VERSION_YEAR
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)


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
SET(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}_${POM_ID_OS}_${POM_ID_PROCESSOR}-${POM_VERSION}_${POM_MBS_RELEASE_VERSION})

SET(CPACK_GENERATOR "TBZ2")
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
	SET(INSTALL_DIR_EXECUTABLES ".")
	SET(INSTALL_DIR_LUA_MODULES "lua_plugins")
	SET(INSTALL_DIR_PYTHON_MODULES "python_plugins")
	SET(INSTALL_DIR_SHARED_OBJECTS ".")
	SET(INSTALL_DIR_LUA_SCRIPTS "lua")
#ENDIF(UNIX AND NOT WIN32 AND NOT APPLE)

#----------------------------------------------------------------------------
#
# Find all packages.
#

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

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

