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
# We need python version <3.9.
# In cmake versions <3.19, we cannot easily express the version range. 
# Therefore, we rely on the assumption that if cmake is <3.19, python is <3.9.

# The Hboot Image Compiler uses the array.tostring() method to convert an array of bytes, 
# 16 or 32 bit words to binary data, which was removed in python 3.9.
# The result variable (PYTHON_EXECUTABLE/Python3_EXECUTABLE) may have been set 
# to a different compiler previously. Therefore, we unset it.


IF(CMAKE_VERSION VERSION_LESS 3.12.0) # cmake version < 3.12
	UNSET(PYTHON_EXECUTABLE)
	FIND_PACKAGE(PythonInterp 3 REQUIRED)
	SET(PYTHON_INTERPRETER "${PYTHON_EXECUTABLE}")
ELSEIF(CMAKE_VERSION VERSION_LESS 3.19.0) # 3.12 <= cmake version < 3.19
	UNSET(Python3_EXECUTABLE)
	FIND_PACKAGE(Python3 REQUIRED COMPONENTS Interpreter)
	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
ELSE(CMAKE_VERSION VERSION_LESS 3.12.0) # cmake version >= 3.19
	UNSET(Python3_EXECUTABLE)
	FIND_PACKAGE(Python3 3...<3.9 REQUIRED COMPONENTS Interpreter)
	SET(PYTHON_INTERPRETER "${Python3_EXECUTABLE}")
ENDIF(CMAKE_VERSION VERSION_LESS 3.12.0)
MESSAGE ("************************ PYTHON_INTERPRETER: ${PYTHON_INTERPRETER} ************************")


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

OPTION(BUILDCFG_SKIP_NETX_FW "Do not build the netX firmware but expect the pre-built artifacts. The default is OFF."
       "OFF")
IF( NOT BUILDCFG_SKIP_NETX_FW STREQUAL "ON" )
	# Add a custom target for the netX firmware.
	ADD_CUSTOM_TARGET(TARGET_NETX_FIRMWARE ALL
	                  COMMAND "${PYTHON_INTERPRETER}" mbs/mbs
	                  WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
	                  COMMENT "Build the netX firmware."
	                  BYPRODUCTS ${NETX_FIRMWARE_PRODUCTS}
	)
ENDIF( NOT BUILDCFG_SKIP_NETX_FW STREQUAL "ON" )

# Filter the jonchki configurations.
CONFIGURE_FILE(plugins/installer/lua${BUILDCFG_LUA_VERSION}/romloader.xml
               ${CMAKE_CURRENT_BINARY_DIR}/lua${BUILDCFG_LUA_VERSION}-romloader-${PROJECT_VERSION}.xml
               @ONLY)


SET(LIBUSB_INSTALL_PATH "${CMAKE_CURRENT_BINARY_DIR}/../external/libusb/install")
SET(LIBUSB_INCLUDE_PATH "${LIBUSB_INSTALL_PATH}/include/libusb-1.0")
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBUSB_LIBRARIES ${LIBUSB_INSTALL_PATH}/lib/libusb-1.0.a)
ELSE(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	SET(LIBUSB_LIBRARIES ${LIBUSB_INSTALL_PATH}/lib/libusb-1.0.a -ludev -pthread)
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
MESSAGE("LIBUSB_INSTALL_PATH: ${LIBUSB_INSTALL_PATH}")
MESSAGE("LIBUSB_INCLUDE_PATH: ${LIBUSB_INCLUDE_PATH}")
MESSAGE("LIBUSB_LIBRARIES:    ${LIBUSB_LIBRARIES}")


IF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )
	SET(libusb_DIR "${CMAKE_CURRENT_BINARY_DIR}/../external/libusb/install/cmake")

	ADD_SUBDIRECTORY(plugins)
ENDIF( NOT BUILDCFG_ONLY_JONCHKI_CFG STREQUAL "ON" )

#-----------------------------------------------------------------------------
