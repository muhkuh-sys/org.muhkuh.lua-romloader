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

# This is the directory structure used for packaging with 'make package'.
SET(DEFAULT_INSTALL_DIR_EXECUTABLES "bin")
SET(DEFAULT_INSTALL_DIR_LUA_MODULES "lua_plugins")
SET(DEFAULT_INSTALL_DIR_LUA_SCRIPTS "lua")
SET(DEFAULT_INSTALL_DIR_SHARED_OBJECTS "bin")

#-----------------------------------------------------------------------------
#
# Allow the user to override the installation folders.
#
OPTION(INSTALL_DIR_EXECUTABLES
       "Install the compiled executables in this path."
       ${DEFAULT_INSTALL_DIR_EXECUTABLES})

OPTION(INSTALL_DIR_LUA_MODULES
       "Install the compiled LUA modules in this path."
       ${DEFAULT_INSTALL_DIR_LUA_MODULES})

OPTION(INSTALL_DIR_LUA_SCRIPTS
       "Install the LUA scripts in this path."
       ${DEFAULT_INSTALL_DIR_LUA_SCRIPTS})

OPTION(INSTALL_DIR_SHARED_OBJECTS
       "Install the shared objects in this path."
       ${DEFAULT_INSTALL_DIR_SHARED_OBJECTS})

