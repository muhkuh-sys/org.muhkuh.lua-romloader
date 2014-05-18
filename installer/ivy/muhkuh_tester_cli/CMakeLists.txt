cmake_minimum_required(VERSION 2.8)

SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME muhkuh_tester_cli-${VERSION_FULL})

SET(CPACK_GENERATOR "ZIP")
INCLUDE(CPack)

# Build the ivy file.
CONFIGURE_FILE(ivy.xml ivy-${VERSION_FULL}.xml)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/select_plugin_cli.lua
              ${MUHKUH_WIN32_INSTALL}/tester_cli.lua
              ${MUHKUH_WIN32_INSTALL}/muhkuh.lua
        DESTINATION lua
)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/muhkuh_cli_init.lua
        DESTINATION .
)

INSTALL(FILES install.xml
        DESTINATION .)

