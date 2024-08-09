CMAKE_MINIMUM_REQUIRED(VERSION 3.7)


#ADD_SUBDIRECTORY(dpm)
ADD_SUBDIRECTORY(eth)
ADD_SUBDIRECTORY(jtag)
#ADD_SUBDIRECTORY(papa_schlumpf)
ADD_SUBDIRECTORY(usb)
ADD_SUBDIRECTORY(uart)

#-----------------------------------------------------------------------------

PROJECT("romloader")

SET(plugins_romloader_lib_SOURCES
	romloader.cpp
	uuencoder.cpp
)

SET(plugins_romloader_lib_HEADERS
	romloader.h
	uuencoder.h
)

ADD_LIBRARY(TARGET_plugins_romloader_LIB STATIC
            EXCLUDE_FROM_ALL
            ${plugins_romloader_lib_SOURCES})
TARGET_LINK_LIBRARIES(TARGET_plugins_romloader_LIB TARGET_muhkuh_plugin_interface)
TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_LIB
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${SWIG_RUNTIME_OUTPUT_PATH})

# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_LIB PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))

# Build the Lua module.
SET_SOURCE_FILES_PROPERTIES(romloader.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader.i PROPERTY SWIG_FLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/../.. -I${CMAKE_BINARY_DIR}/version_information)
IF(CMAKE_VERSION VERSION_LESS 3.8.0)
	SWIG_ADD_MODULE(TARGET_plugins_romloader_LUA lua romloader.i)
ELSE(CMAKE_VERSION VERSION_LESS 3.8.0)
	SWIG_ADD_LIBRARY(TARGET_plugins_romloader_LUA
	                 TYPE MODULE
	                 LANGUAGE LUA
	                 SOURCES romloader.i)
ENDIF(CMAKE_VERSION VERSION_LESS 3.8.0)
SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_LUA TARGET_plugins_romloader_LIB)
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_LUA ${LUA_LIBRARIES})
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

TARGET_INCLUDE_DIRECTORIES(TARGET_plugins_romloader_LUA
                           PRIVATE ${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Set the name of the output file to "romloader".
SET_TARGET_PROPERTIES(TARGET_plugins_romloader_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader")

# On mingw link all compiler libraries static.
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	SET_PROPERTY(TARGET TARGET_plugins_romloader_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# Install the lua module.
INSTALL(TARGETS TARGET_plugins_romloader_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Copy the start-mi binary to output, flasher will copy it to artifact later
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../targets/plugins/romloader/machine_interface/start_mi/hboot_start_mi_netx90_com_intram.bin
DESTINATION ${INSTALL_DIR_LUA_MODULES})

# Add tests for this module.
IF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	# Here are the MinGW specific tests.
	ADD_TEST(NAME romloader_MinGW_DLL_dependencies
	         COMMAND "${PYTHON_INTERPRETER}" ${TEST_SCRIPT_DIR}/mingw_dll_dependencies.py -u lua5.1 -u lua5.2 -u lua5.3 -u lua5.4 $<TARGET_FILE:TARGET_plugins_romloader_LUA>)
ENDIF((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
