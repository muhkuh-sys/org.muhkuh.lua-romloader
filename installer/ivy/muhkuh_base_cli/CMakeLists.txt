cmake_minimum_required(VERSION 2.8)

SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME muhkuh_base_cli-${VERSION_FULL})

SET(CPACK_GENERATOR "ZIP")
INCLUDE(CPack)

# Build the ivy file.
CONFIGURE_FILE(ivy.xml ivy-${VERSION_FULL}.xml)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/lua/select_plugin_cli.lua
              ${MUHKUH_WIN32_INSTALL}/lua/tester_cli.lua
              ${MUHKUH_WIN32_INSTALL}/lua/muhkuh.lua
        DESTINATION lua
)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/lua/muhkuh_cli_init.lua
        DESTINATION .
)

INSTALL(FILES install.xml
        DESTINATION .)

# Filter the target.xml file.
SET(ARTIFACT_DIRECTORY    "${CMAKE_CURRENT_BINARY_DIR}")
SET(ARTIFACT_ARTIFACT_ID  "muhkuh_base_cli")
SET(ARTIFACT_GROUP_ID     "tools.muhkuh.org")
SET(ARTIFACT_VERSION      "${VERSION_FULL}")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../artifacts.xml ${ARTIFACT_LISTS_OUTPUT}/artifacts_muhkuh_base_cli.xml)
