CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("org.muhkuh.lua-lua51-romloader-artifact")

INCLUDE(../../../cmake/version.cmake)
INCLUDE(../../../cmake/muhkuh_version.cmake)
SET(PROJECT_VERSION ${MUHKUH_VERSION_ALL})

SET(INSTALL_WIN32 ${CMAKE_CURRENT_SOURCE_DIR}/../../../build_windows32/lua5.1/install)
SET(INSTALL_WIN64 ${CMAKE_CURRENT_SOURCE_DIR}/../../../build_windows64/lua5.1/install)

SET(CPACK_OUTPUT_FILE_PREFIX targets/jonchki/repository/org/muhkuh/lua/lua5.1-romloader/${PROJECT_VERSION})
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME "lua5.1-romloader-${PROJECT_VERSION}")

SET(CPACK_GENERATOR "ZIP")
INCLUDE(CPack)


#-----------------------------------------------------------------------------
#
# Install the 32bit files.
#
INSTALL(FILES ${INSTALL_WIN32}/lua_plugins/romloader.dll
              ${INSTALL_WIN32}/lua_plugins/romloader_eth.dll
              ${INSTALL_WIN32}/lua_plugins/romloader_jtag.dll
              ${INSTALL_WIN32}/lua_plugins/romloader_papa_schlumpf.dll
              ${INSTALL_WIN32}/lua_plugins/romloader_uart.dll
              ${INSTALL_WIN32}/lua_plugins/romloader_usb.dll
        DESTINATION windows_x86/lib)


#-----------------------------------------------------------------------------
#
# Install the 64bit files.
#
INSTALL(FILES ${INSTALL_WIN64}/lua_plugins/romloader.dll
              ${INSTALL_WIN64}/lua_plugins/romloader_eth.dll
              ${INSTALL_WIN64}/lua_plugins/romloader_jtag.dll
              ${INSTALL_WIN64}/lua_plugins/romloader_papa_schlumpf.dll
              ${INSTALL_WIN64}/lua_plugins/romloader_uart.dll
              ${INSTALL_WIN64}/lua_plugins/romloader_usb.dll
        DESTINATION windows_x64/lib)


#-----------------------------------------------------------------------------

INSTALL(FILES install.lua
        DESTINATION .)

CONFIGURE_FILE(romloader.xml
               ${CPACK_OUTPUT_FILE_PREFIX}/lua5.1-romloader-${PROJECT_VERSION}.xml
               @ONLY)
