CMAKE_MINIMUM_REQUIRED(VERSION 3.7)


PROJECT("romloader_papa_schlumpf"
        VERSION ${MUHKUH_VERSION_ALL})


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(libusb REQUIRED)


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#
SET(romloader_papa_schlumpf_SOURCES
	romloader_papa_schlumpf.cpp
	romloader_papa_schlumpf_device.cpp
)

SET(romloader_papa_schlumpf_HEADERS
	papa_schlumpf_firmware_interface.h
	romloader_papa_schlumpf.h
	romloader_papa_schlumpf_device.h
)


ADD_LIBRARY(TARGET_romloader_papa_schlumpf_lib STATIC ${romloader_papa_schlumpf_SOURCES})
TARGET_LINK_LIBRARIES(TARGET_romloader_papa_schlumpf_lib libusb::TARGET_libusb)
TARGET_INCLUDE_DIRECTORIES(TARGET_romloader_papa_schlumpf_lib
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	SET_TARGET_PROPERTIES(TARGET_romloader_papa_schlumpf_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))


#----------------------------------------------------------------------------
#
# Build the lua plugin.
#
SET_SOURCE_FILES_PROPERTIES(romloader_papa_schlumpf.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_papa_schlumpf.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY} -I${CMAKE_BINARY_DIR}/version_information)
SWIG_ADD_LIBRARY(TARGET_PapaSchlumpf_LUA
                 TYPE MODULE
                 LANGUAGE LUA
                 SOURCES romloader_papa_schlumpf.i)
SWIG_LINK_LIBRARIES(TARGET_PapaSchlumpf_LUA TARGET_romloader_papa_schlumpf_lib ${LUA_LIBRARIES} TARGET_plugins_romloader_LIB)
TARGET_INCLUDE_DIRECTORIES(TARGET_PapaSchlumpf_LUA
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set the name of the output file to "romloader_papa_schlumpf".
SET_TARGET_PROPERTIES(TARGET_PapaSchlumpf_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_papa_schlumpf")

IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# On minGW link all compiler libraries static.
	SET_PROPERTY(TARGET TARGET_PapaSchlumpf_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# Install the lua module.
INSTALL(TARGETS TARGET_PapaSchlumpf_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Add tests for this module.
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# Here are the MinGW specific tests.
	ADD_TEST(NAME romloader_usb_MinGW_DLL_dependencies
	         COMMAND "${PYTHON_EXECUTABLE}" ${TEST_SCRIPT_DIR}/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 $<TARGET_FILE:TARGET_PapaSchlumpf_LUA>)
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
