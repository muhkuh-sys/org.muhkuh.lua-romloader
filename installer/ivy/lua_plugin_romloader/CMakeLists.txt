cmake_minimum_required(VERSION 2.8)

SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME lua_plugin_romloader-${VERSION_FULL})

SET(CPACK_GENERATOR "ZIP")
INCLUDE(CPack)

# Build the ivy file.
CONFIGURE_FILE(ivy.xml ivy-${VERSION_FULL}.xml)

INSTALL(FILES ${MUHKUH_WIN32_INSTALL}/romloader.dll
              ${MUHKUH_WIN32_INSTALL}/romloader_eth.dll
              ${MUHKUH_WIN32_INSTALL}/romloader_uart.dll
              ${MUHKUH_WIN32_INSTALL}/romloader_usb.dll
        DESTINATION windows_x86
)

INSTALL(FILES ${MUHKUH_WIN64_INSTALL}/romloader.dll
              ${MUHKUH_WIN64_INSTALL}/romloader_eth.dll
              ${MUHKUH_WIN64_INSTALL}/romloader_uart.dll
              ${MUHKUH_WIN64_INSTALL}/romloader_usb.dll
        DESTINATION windows_amd64
)

INSTALL(FILES install.xml
	DESTINATION .)

