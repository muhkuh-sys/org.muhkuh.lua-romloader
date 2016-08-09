cmake_minimum_required(VERSION 2.8)


PROJECT("romloader_papa_schlumpf")

SET(romloader_papa_schlumpf_VERSION 0.0.1)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

if(${LUA51_FOUND})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SWIG_RUNTIME_OUTPUT_PATH})


#----------------------------------------------------------------------------
#
# Set the platform specific parts.
#

# Append the include directories for libusb.
INCLUDE_DIRECTORIES(${LIBUSB_INCLUDE_DIRECTORIES})

# Append the libusb defines.
ADD_DEFINITIONS(${LIBUSB_DEFINES})


SET_SOURCE_FILES_PROPERTIES(romloader_papa_schlumpf.i PROPERTIES CPLUSPLUS ON)
SET_PROPERTY(SOURCE romloader_papa_schlumpf.i PROPERTY SWIG_FLAGS ${LIBUSB_DEFINES} -I${CMAKE_HOME_DIRECTORY}/plugins)


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
# This static library will be linked with a shared object later. Add "-fPIC" on GCC systems or X64 builds break.
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET_TARGET_PROPERTIES(TARGET_romloader_papa_schlumpf_lib PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# The library depends on the libusb header.
ADD_DEPENDENCIES(TARGET_romloader_papa_schlumpf_lib TARGET_libusb)



#----------------------------------------------------------------------------
#
# Build the lua plugin.
#

IF(${LUA51_FOUND})
	# Build the Lua module.
	SWIG_ADD_MODULE(TARGET_plugins_romloader_romloader_papa_schlumpf_LUA lua romloader_papa_schlumpf.i)
	SWIG_LINK_LIBRARIES(TARGET_plugins_romloader_romloader_papa_schlumpf_LUA TARGET_romloader_papa_schlumpf_lib ${LUA_LIBRARIES} ${LIBUSB_LIBRARY} TARGET_plugins_romloader_LIB ${LIBUSB_LDFLAGS})

	# Set the name of the output file to "romloader_papa_schlumpf".
	SET_TARGET_PROPERTIES(TARGET_plugins_romloader_romloader_papa_schlumpf_LUA PROPERTIES PREFIX "" OUTPUT_NAME "romloader_papa_schlumpf")

	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# On minGW link all compiler libraries static.
		set_property(TARGET TARGET_plugins_romloader_romloader_papa_schlumpf_LUA PROPERTY LINK_FLAGS "-static -static-libgcc -static-libstdc++")
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

	# The module depends on the libusb libraries.
	ADD_DEPENDENCIES(TARGET_plugins_romloader_romloader_papa_schlumpf_LUA TARGET_libusb)
	
	# Install the lua module.
	INSTALL(TARGETS TARGET_plugins_romloader_romloader_papa_schlumpf_LUA DESTINATION ${INSTALL_DIR_LUA_MODULES})

	# Add tests for this module.
	IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
		# Here are the MinGW specific tests.
		ADD_TEST(NAME romloader_usb_MinGW_DLL_dependencies
		         COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tests/mingw_dll_dependencies.py -u lua5.1 $<TARGET_FILE:TARGET_plugins_romloader_romloader_papa_schlumpf_LUA>)
	ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
ENDIF(${LUA51_FOUND})
