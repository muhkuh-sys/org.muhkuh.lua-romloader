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
FIND_PACKAGE(PythonInterp 2.7 REQUIRED)


SET(NETX_FIRMWARE_PRODUCTS
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/dpm/netx/dpmmon_netx56_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/dpm/netx/dpmmon_netx56_monitor.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/dpm/netx/dpmmon_netx4000_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/dpm/netx/dpmmon_netx4000_monitor.h

	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_monitor.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_monitor_run.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_monitor.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_monitor_run.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx56_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx56_monitor.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx56_monitor_run.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx500_monitor.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx500_monitor.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx500_monitor_run.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_bootstrap.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_bootstrap.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx10_bootstrap_run.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_bootstrap.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_bootstrap.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/uart/netx/uartmon_netx50_bootstrap_run.h

	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx10.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx10.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx56.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx56.h
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx500.c
	${CMAKE_HOME_DIRECTORY}/targets/plugins/romloader/usb/netx/usbmon_netx500.h
)

# Add a custom target for the netX firmware.
ADD_CUSTOM_TARGET(TARGET_NETX_FIRMWARE ALL
                  COMMAND "${PYTHON_EXECUTABLE}" mbs/mbs
                  WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
                  COMMENT "Build the netX firmware."
                  BYPRODUCTS ${NETX_FIRMWARE_PRODUCTS}
)


# Filter the jonchki configurations.
CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.xml
               @ONLY)


IF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )
	SET(openocd_DIR "${CMAKE_CURRENT_BINARY_DIR}/../external/openocd/install/cmake")
	SET(libusb_DIR "${CMAKE_CURRENT_BINARY_DIR}/../external/libusb/install/cmake")

	ADD_SUBDIRECTORY(plugins)
ENDIF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )

#-----------------------------------------------------------------------------
