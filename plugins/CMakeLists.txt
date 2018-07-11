CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("muhkuh_plugin_interface"
        VERSION ${MUHKUH_VERSION_ALL})

# Select the LUA version.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_lua.cmake)

# Enable tests (see here for details: http://www.vtk.org/Wiki/CMake/Testing_With_CTest)
ENABLE_TESTING()


# Set some installation folders.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_installation_folders.cmake)

# Detect the platform.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/platform.cmake)

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# Add a custom target for the swig runtime for LUA.
INCLUDE(${CMAKE_HOME_DIRECTORY}/cmake/muhkuh_swig_lua_runtime.cmake)


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
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	SET_PROPERTY(TARGET TARGET_muhkuh_plugin_interface PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	SET_TARGET_PROPERTIES(TARGET_muhkuh_plugin_interface PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))


#----------------------------------------------------------------------------
#
# Build a package for the selected platform.
#
SET(CPACK_OUTPUT_FILE_PREFIX ${CMAKE_BINARY_DIR}/targets/jonchki/repository/org/muhkuh/lua/romloader/${PROJECT_VERSION})
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME "lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}")

SET(CPACK_GENERATOR "TXZ")
INCLUDE(CPack)

IF(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")
	# Package a windows build.

	# NOTE: No "INSTALL" here, they are in the sub folders.

	CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/install_windows.lua
	               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
	               @ONLY)

	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
	        DESTINATION .)
ELSE(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")
	# Package a linux build.

	# NOTE: No "INSTALL" here, they are in the sub folders.

	CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/install_linux.lua
	               ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
	               @ONLY)

	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/package/install.lua
	        DESTINATION .)
ENDIF(JONCHKI_PLATFORM_DIST_ID STREQUAL "windows")

CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
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
                  COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.xml
                  COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/generate_hash.py ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}-${JONCHKI_PLATFORM}.tar.xz
                  COMMENT "Pack the artifact and generate the hashes...")


#-----------------------------------------------------------------------------
#
# Filter the pom.xml file.
#
SET(POM_ARTIFACT_ID  "romloader")
SET(POM_GROUP_ID     "org.muhkuh.lua")
SET(POM_VERSION      "${PROJECT_VERSION}")
SET(POM_PACKAGING    "tar.xz")
CONFIGURE_FILE(installer/lua${BUILDCFG_LUA_VERSION}/pom.xml
               ${CPACK_OUTPUT_FILE_PREFIX}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.pom
               @ONLY)
