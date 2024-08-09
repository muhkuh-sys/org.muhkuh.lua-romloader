CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

#-----------------------------------------------------------------------------
#
# The project options.
#
OPTION(BUILDCFG_ONLY_JONCHKI_CFG "Only build the jonchki configuration files. The default is OFF."
       "OFF")

MESSAGE("BUILDCFG_ONLY_JONCHKI_CFG: ${BUILDCFG_ONLY_JONCHKI_CFG}")


# Generate a version number from a file or VCS.
INCLUDE(cmake/muhkuh_version.cmake)


PROJECT("muhkuh"
        VERSION ${MUHKUH_VERSION_ALL})

# Get the VCS version for the jonchki configuration.
INCLUDE(cmake/version.cmake)

# Detect the platform.
INCLUDE(cmake/platform.cmake)


# Python is used for the firmware build script and some test scripts.
# We need python version <3.9.
# In cmake versions <3.19, we cannot easily express the version range. 
# Therefore, we rely on the assumption that if cmake is <3.19, python is <3.9.

# The Hboot Image Compiler uses the array.tostring() method to convert an array of bytes, 
# 16 or 32 bit words to binary data, which was removed in python 3.9.
# The result variable (PYTHON_EXECUTABLE/Python3_EXECUTABLE) may have been set 
# to a different compiler previously. Therefore, we unset it.


#IF(CMAKE_VERSION VERSION_LESS 3.12.0) # cmake version < 3.12
#	UNSET(PYTHON_EXECUTABLE)
#	FIND_PACKAGE(PythonInterp 3 REQUIRED)
#	SET(PYTHON_INTERPRETER "${PYTHON_EXECUTABLE}")
#ELSEIF(CMAKE_VERSION VERSION_LESS 3.19.0) # 3.12 <= cmake version < 3.19
#	UNSET(Python3_EXECUTABLE)
#	FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)
#	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
#ELSE(CMAKE_VERSION VERSION_LESS 3.12.0) # cmake version >= 3.19
#	UNSET(Python3_EXECUTABLE)
#	FIND_PACKAGE(Python3 3...<3.9 REQUIRED COMPONENTS Interpreter)
#	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
#ENDIF(CMAKE_VERSION VERSION_LESS 3.12.0)

IF(CMAKE_VERSION VERSION_LESS 3.12.0)
	FIND_PACKAGE(PythonInterp 3 REQUIRED)
	SET(PYTHON_INTERPRETER "${PYTHON_EXECUTABLE}")
ELSE(CMAKE_VERSION VERSION_LESS 3.12.0)
	FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)
	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
ENDIF(CMAKE_VERSION VERSION_LESS 3.12.0)


MESSAGE ("************************ PYTHON_INTERPRETER: ${PYTHON_INTERPRETER} ************************")

SET(PLUGINS_SOURCES_DIR "${CMAKE_HOME_DIRECTORY}/targets/plugins")

# Filter the jonchki configurations.
CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.xml
               @ONLY)


SET(LIBUSB_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/../external/libusb/install")
SET(LIBUSB_INCLUDE_DIR "${LIBUSB_INSTALL_DIR}/include/libusb-1.0")
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBUSB_LIBRARIES ${LIBUSB_INSTALL_DIR}/lib/libusb-1.0.a)
ELSE(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBUSB_LIBRARIES ${LIBUSB_INSTALL_DIR}/lib/libusb-1.0.a -ludev -pthread)
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
MESSAGE("LIBUSB_INSTALL_DIR: ${LIBUSB_INSTALL_DIR}")
MESSAGE("LIBUSB_INCLUDE_DIR: ${LIBUSB_INCLUDE_DIR}")
MESSAGE("LIBUSB_LIBRARIES:    ${LIBUSB_LIBRARIES}")


IF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )
	SET(libusb_DIR "${CMAKE_CURRENT_BINARY_DIR}/../external/libusb/install/cmake")

	# Select the LUA version.
	INCLUDE(cmake/muhkuh_lua.cmake)

	# Set some installation folders.
	INCLUDE(cmake/muhkuh_installation_folders.cmake)

	# Swig is required to generate the bindings.
	FIND_PACKAGE(SWIG REQUIRED)
	# Add a custom target for the swig runtime for LUA.
	INCLUDE(cmake/muhkuh_swig_lua_runtime.cmake)
	INCLUDE(${SWIG_USE_FILE})

	ADD_SUBDIRECTORY(plugins)

	#----------------------------------------------------------------------------
	#
	# Build a package for the selected platform.
	#
	SET(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/../../repository/org/muhkuh/lua/romloader/${PROJECT_VERSION})
	SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
	SET(CPACK_PACKAGE_FILE_NAME "lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}")

	SET(CPACK_GENERATOR "TXZ")
	INCLUDE(CPack)

	IF(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")
		# Package a windows build.

		# NOTE: No "INSTALL" here, they are in the sub folders.

		CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/install_windows.lua
		               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		               @ONLY)

		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
	                DESTINATION .)
	ELSE(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")
		# Package a linux build.

		# NOTE: No "INSTALL" here, they are in the sub folders.

		CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/install_linux.lua
		               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		               @ONLY)

					   INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
		        DESTINATION .)
	ENDIF(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")

	CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
	               ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.xml
	               @ONLY)


	#-----------------------------------------------------------------------------
	#
	# Create a custom target named "pack".
	# It builds the package and creates the hashes.
	#
	ADD_CUSTOM_TARGET(pack
	                  COMMAND ${CMAKE_MAKE_PROGRAM}
	                  COMMAND ${CMAKE_MAKE_PROGRAM} package
	                  COMMAND "${PYTHON_INTERPRETER}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.xml
	                  COMMAND "${PYTHON_INTERPRETER}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.tar.xz
	                  COMMENT "Pack the artifact and generate the hashes...")


	#-----------------------------------------------------------------------------
	#
	# Filter the pom.xml file.
	#
	SET(POM_ARTIFACT_ID  "lua${BUILDCFG_LUA_VERSION}-romloader")
	SET(POM_GROUP_ID     "org.muhkuh.lua")
	SET(POM_VERSION      "${PROJECT_VERSION}")
	SET(POM_PACKAGING    "tar.xz")
	CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/pom.xml
	               ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.pom
	               @ONLY)
ENDIF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )

#-----------------------------------------------------------------------------
