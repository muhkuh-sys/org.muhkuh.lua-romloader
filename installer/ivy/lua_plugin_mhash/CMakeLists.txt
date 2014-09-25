cmake_minimum_required(VERSION 2.8)

SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME lua_plugin_mhash-${VERSION_FULL})

SET(CPACK_GENERATOR "ZIP")
INCLUDE(CPack)

# Build the ivy file.
CONFIGURE_FILE(ivy.xml ivy-${VERSION_FULL}.xml)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/lua_plugins/mhash.dll
        DESTINATION windows_x86
)

INSTALL(FILES ${MUHKUH_WIN64_INSTALL}/lua_plugins/mhash.dll
        DESTINATION windows_amd64
)

INSTALL(FILES install.xml
        DESTINATION .)

# Filter the target.xml file.
SET(ARTIFACT_DIRECTORY    "${CMAKE_CURRENT_BINARY_DIR}")
SET(ARTIFACT_ARTIFACT_ID  "lua_plugin_mhash")
SET(ARTIFACT_GROUP_ID     "tools.muhkuh.org")
SET(ARTIFACT_VERSION      "${VERSION_FULL}")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../artifacts.xml ${ARTIFACT_LISTS_OUTPUT}/artifacts_lua_plugin_mhash.xml)
