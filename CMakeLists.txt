CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

#-----------------------------------------------------------------------------
#
# The project options.
#
OPTION(BUILDCFG_LUA_USE_SYSTEM "Use the system-wide installed LUA version. The default is ON."
       "ON")
OPTION(BUILDCFG_LUA_51 "Build the project for LUA5.1 . The default is ON."
       "ON")
OPTION(BUILDCFG_LUA_52 "Build the project for LUA5.2 . The default is OFF."
       "OFF")
OPTION(BUILDCFG_LUA_53 "Build the project for LUA5.3 . The default is OFF."
       "OFF")


# Get the VCS version for the jonchki configuration.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/version.cmake)

# Generate a version number from a file or VCS.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_version.cmake)


PROJECT("muhkuh"
        VERSION ${MUHKUH_VERSION_ALL})


# Python is used for the firmware build script and some test scripts.
FIND_PACKAGE(PythonInterp 2.7 REQUIRED)

# Add a custom target for the netX firmware.
ADD_CUSTOM_TARGET(TARGET_NETX_FIRMWARE
                  COMMAND "${PYTHON_EXECUTABLE}" mbs/mbs
                  WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
                  COMMENT "Build the netX firmware.")

# Add a custom target for the externals.
ADD_CUSTOM_TARGET(TARGET_EXTERNALS
                  DEPENDS TARGET_libusb TARGET_openocd
                  COMMENT "Build the externals.")

ADD_SUBDIRECTORY(external)

# Filter the jonchki configurations.
CONFIGURE_FILE(jonchki/installer/lua5.1/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua5.1-romloader-${PROJECT_VERSION}.xml
               @ONLY)
CONFIGURE_FILE(jonchki/installer/lua5.2/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua5.2-romloader-${PROJECT_VERSION}.xml
               @ONLY)
CONFIGURE_FILE(jonchki/installer/lua5.3/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua5.3-romloader-${PROJECT_VERSION}.xml
               @ONLY)

INCLUDE(ExternalProject)

#-----------------------------------------------------------------------------
#
# Build the LUA5.1 version.
#
IF( BUILDCFG_LUA_51 )
	SET(PLUGINS_LUA51_CMAKE_ARGS "")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/../libusb/install/cmake")
	IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
	ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-Dopenocd_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/openocd/install/cmake")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-Dlibusb_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/libusb/install/cmake")

	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DBUILDCFG_LUA_USE_SYSTEM=${BUILDCFG_LUA_USE_SYSTEM}")
	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DBUILDCFG_LUA_VERSION=5.1")

	LIST(APPEND PLUGINS_LUA51_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/lua5.1")


	# Build the plugins as an external project.
	# This is important as it allows the "find_package" command to use the
	# components from the "external" folder.
	ExternalProject_Add(TARGET_PLUGINS_LUA51_PROJECT
	                    EXCLUDE_FROM_ALL 1
	                    DEPENDS TARGET_NETX_FIRMWARE TARGET_EXTERNALS
	                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lua5.1/plugins
	                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plugins
	                    CMAKE_ARGS ${PLUGINS_LUA51_CMAKE_ARGS}
	)

	ADD_CUSTOM_TARGET(TARGET_PLUGINS_LUA51 ALL
	                  COMMAND "${CMAKE_MAKE_PROGRAM}" install 
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lua5.1/plugins/src/TARGET_PLUGINS_LUA51_PROJECT-build
	                  COMMENT "Compile the plugins.")
	ADD_DEPENDENCIES(TARGET_PLUGINS_LUA51 TARGET_PLUGINS_LUA51_PROJECT)
ENDIF( BUILDCFG_LUA_51 )



#-----------------------------------------------------------------------------
#
# Build the LUA5.2 version.
#
IF( BUILDCFG_LUA_52 )
	SET(PLUGINS_LUA52_CMAKE_ARGS "")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/../libusb/install/cmake")
	IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
	ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-Dopenocd_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/openocd/install/cmake")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-Dlibusb_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/libusb/install/cmake")

	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DBUILDCFG_LUA_USE_SYSTEM=${BUILDCFG_LUA_USE_SYSTEM}")
	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DBUILDCFG_LUA_VERSION=5.2")

	LIST(APPEND PLUGINS_LUA52_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/lua5.1")


	# Build the plugins as an external project.
	# This is important as it allows the "find_package" command to use the
	# components from the "external" folder.
	ExternalProject_Add(TARGET_PLUGINS_LUA52
	                    DEPENDS TARGET_NETX_FIRMWARE TARGET_EXTERNALS
	                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lua5.2/plugins
	                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plugins
	                    CMAKE_ARGS ${PLUGINS_LUA52_CMAKE_ARGS}
	)
ENDIF( BUILDCFG_LUA_52 )



#-----------------------------------------------------------------------------
#
# Build the LUA5.3 version.
#
IF( BUILDCFG_LUA_53 )
	SET(PLUGINS_LUA53_CMAKE_ARGS "")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/../libusb/install/cmake")
	IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
	ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-Dopenocd_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/openocd/install/cmake")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-Dlibusb_DIR=${CMAKE_CURRENT_BINARY_DIR}/external/libusb/install/cmake")

	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DBUILDCFG_LUA_USE_SYSTEM=${BUILDCFG_LUA_USE_SYSTEM}")
	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DBUILDCFG_LUA_VERSION=5.3")

	LIST(APPEND PLUGINS_LUA53_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/lua5.1")


	# Build the plugins as an external project.
	# This is important as it allows the "find_package" command to use the
	# components from the "external" folder.
	ExternalProject_Add(TARGET_PLUGINS_LUA53
	                    DEPENDS TARGET_NETX_FIRMWARE TARGET_EXTERNALS
	                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/lua5.3/plugins
	                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plugins
	                    CMAKE_ARGS ${PLUGINS_LUA53_CMAKE_ARGS}
	)
ENDIF( BUILDCFG_LUA_53 )


#-----------------------------------------------------------------------------
