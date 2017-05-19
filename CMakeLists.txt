cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")


# Enable tests (see here for details: http://www.vtk.org/Wiki/CMake/Testing_With_CTest)
ENABLE_TESTING()


# Set some installation folders.
INCLUDE(cmake/muhkuh_installation_folders.cmake)

OPTION(BUILDCFG_ONLY_JONCHKI_CFG "Build only the jonchki configuration. This is used for the resolve phase. The default is OFF."
       "OFF")


# Get the VCS version for the jonchki configuration.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/version.cmake)

# Generate a version number from a file or VCS.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_version.cmake)

SET(PROJECT_VERSION "${MUHKUH_VERSION_ALL}")


# Python is used for the firmware build script and some test scripts.
FIND_PACKAGE(PythonInterp 2.7 REQUIRED)

# Add a custom target for the netX firmware.
ADD_CUSTOM_TARGET(TARGET_NETX_FIRMWARE
                  COMMAND "${PYTHON_EXECUTABLE}" mbs/mbs
                  WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
                  COMMENT "Build the netX firmware.")



# Filter the jonchki configuration.
CONFIGURE_FILE(jonchki/installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.xml
               @ONLY)

IF((${BUILDCFG_ONLY_JONCHKI_CFG} STREQUAL "OFF"))
	#---------------------------------------------------------------------
	#
	# Find all packages.
	#

	# Swig is required to generate the bindings.
	FIND_PACKAGE(SWIG REQUIRED)

	# Select the LUA version.
	INCLUDE(cmake/muhkuh_lua.cmake)

	# Add a custom target for the swig runtime for LUA.
	INCLUDE(cmake/muhkuh_swig_lua_runtime.cmake)


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


	#---------------------------------------------------------------------
	#
	# Set custom definitions for several compiler.
	#

	# Microsoft: Do not show msvs security warnings about unsafe functions.
	IF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	ENDIF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv|nmake)")


	# Microsoft: Set warning level to 2.
	IF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv)")
		ADD_DEFINITIONS(/W2)
	ENDIF(${CMAKE_BUILD_TOOL} MATCHES "(msdev|VCExpress|devenv)")


	#---------------------------------------------------------------------
	#
	# Include the projects in the subdirectories.
	#
	ADD_SUBDIRECTORY(plugins)
ENDIF((${BUILDCFG_ONLY_JONCHKI_CFG} STREQUAL "OFF"))

#-----------------------------------------------------------------------------

