CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

PROJECT("org.muhkuh.lua-lua5.1-romloader-artifact")

INCLUDE(../../../cmake/version.cmake)
INCLUDE(../../../cmake/muhkuh_version.cmake)
SET(PROJECT_VERSION ${MUHKUH_VERSION_ALL})

SET(INSTALL_WINDOWS_X86        "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/windows_x86")
SET(INSTALL_WINDOWS_X86_64     "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/windows_x86_64")
SET(INSTALL_UBUNTU_1404_X86    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1404_x86")
SET(INSTALL_UBUNTU_1404_X86_64 "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1404_x86_64")
SET(INSTALL_UBUNTU_1604_X86    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1604_x86")
SET(INSTALL_UBUNTU_1604_X86_64 "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1604_x86_64")
SET(INSTALL_UBUNTU_1704_X86    "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1704_x86")
SET(INSTALL_UBUNTU_1704_X86_64 "${CMAKE_CURRENT_SOURCE_DIR}/../../../build/lua5.1/ubuntu_1704_x86_64")


SET(CPACK_OUTPUT_FILE_PREFIX targets/jonchki/repository/org/muhkuh/lua/romloader/${PROJECT_VERSION})
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
SET(CPACK_PACKAGE_FILE_NAME "lua5.1-romloader-${PROJECT_VERSION}")

SET(CPACK_GENERATOR "TXZ")
INCLUDE(CPack)


#-----------------------------------------------------------------------------
#
# Install the 32bit files.
#
INSTALL(FILES ${INSTALL_WINDOWS_X86}/lua_plugins/romloader.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/romloader_eth.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/romloader_jtag.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/romloader_papa_schlumpf.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/romloader_uart.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/romloader_usb.dll
        DESTINATION windows_x86/lua_plugins)

INSTALL(FILES ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/openocd.dll
              ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_WINDOWS_X86}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION windows_x86/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1404_x86/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1404_X86}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1404_x86/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1604_x86/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1604_X86}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1604_x86/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1704_x86/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1704_X86}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1704_x86/lua_plugins/openocd)


#-----------------------------------------------------------------------------
#
# Install the 64bit files.
#
INSTALL(FILES ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader_eth.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader_jtag.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader_papa_schlumpf.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader_uart.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/romloader_usb.dll
        DESTINATION windows_x86_64/lua_plugins)

INSTALL(FILES ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/openocd.dll
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_WINDOWS_X86_64}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION windows_x86_64/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1404_x86_64/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1404_X86_64}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1404_x86_64/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1604_x86_64/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1604_X86_64}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1604_x86_64/lua_plugins/openocd)

INSTALL(FILES ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader_eth.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader_jtag.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader_papa_schlumpf.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader_uart.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/romloader_usb.so
        DESTINATION ubuntu_1704_x86_64/lua_plugins)

INSTALL(FILES ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/openocd.so
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/jtag_detect_init.tcl
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/chip_init.tcl
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/dcc_netx500.bin
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/dcc_netx10_50_51_52.bin
              ${INSTALL_UBUNTU_1704_X86_64}/lua_plugins/openocd/dcc_netx90_com.bin
        DESTINATION ubuntu_1704_x86_64/lua_plugins/openocd)

#-----------------------------------------------------------------------------

INSTALL(FILES install.lua
        DESTINATION .)

CONFIGURE_FILE(romloader.xml
               ${CPACK_OUTPUT_FILE_PREFIX}/lua5.1-romloader-${PROJECT_VERSION}.xml
               @ONLY)


#-----------------------------------------------------------------------------
#
# Filter the pom.xml file.
#
SET(POM_ARTIFACT_ID  "lua5.1-romloader")
SET(POM_GROUP_ID     "org.muhkuh.lua")
SET(POM_VERSION      "${PROJECT_VERSION}")
SET(POM_PACKAGING    "tar.xz")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pom.xml ${CPACK_OUTPUT_FILE_PREFIX}/lua5.1-romloader-${PROJECT_VERSION}.pom)
