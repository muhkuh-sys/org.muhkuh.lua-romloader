cmake_minimum_required(VERSION 3.7)

#----------------------------------------------------------------------------
#
# Build libusb and create a CMake component.
#
PROJECT("libusb")

SET(VERSION_EXTERNAL "1.0.24")
SET(VERSION_EXTERNAL_POST "")

INCLUDE(ExternalProject)

#----------------------------------------------------------------------------
#
# Construct the host ID for the configure command.
#
IF(${CMAKE_CROSSCOMPILING})
	IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# Get the prefix of the compiler.
		GET_FILENAME_COMPONENT(GCC_BASENAME ${CMAKE_C_COMPILER} NAME)
		IF(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
			SET(CONFIGURE_HOST "--host=${CMAKE_MATCH_1}")
		ELSE(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
			MESSAGE(FATAL_ERROR "Failed to extract the compiler prefix from the C compiler ${CMAKE_C_COMPILER}")
		ENDIF(GCC_BASENAME MATCHES "([^-]+-[^-]+-[^-]+)-gcc")
	ELSE(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		MESSAGE(FATAL_ERROR "Cross compiling detected, but not using GCC. This is currently not supported.")
	ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
ELSE(${CMAKE_CROSSCOMPILING})
	SET(CONFIGURE_HOST "")
ENDIF(${CMAKE_CROSSCOMPILING})



#----------------------------------------------------------------------------
#
# Build the project.
#
IF(${WIN32})
    # Building and installing libusb on Windows requires the use of a MSYS2 shell.
    # Check whether to use 64 or 32 Bit so the correct compiler is in the PATH
    IF(${CMAKE_C_FLAGS} STREQUAL "-m64")
        message("Shell is mingw64")
        SET(MSYS2_SHELL "C:/msys64/mingw64.exe")
    ELSE()
        message("Shell is mingw32")
        SET(MSYS2_SHELL "C:/msys64/mingw32.exe")
    ENDIF()

    # The timeouts prevent cmake from jumping to the next command before the last executed.
    # This is a problem with the call to msys2 returning immediatly after opening the new shell but before the actual command finished
    ExternalProject_Add(TARGET_libusb
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}
                        URL ${CMAKE_CURRENT_SOURCE_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}.tar.bz2
                        URL_HASH SHA1=d8d614b538f7c953b6e3b73f1eea5dc70820a7e2
                        CONFIGURE_COMMAND ${MSYS2_SHELL} ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}/src/TARGET_libusb/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic ${CONFIGURE_HOST} CFLAGS=${CMAKE_C_FLAGS} CPPFLAGS=${CMAKE_CXX_FLAGS} && timeout 120
                        BUILD_COMMAND ${MSYS2_SHELL} make && timeout 45
                        INSTALL_COMMAND ${MSYS2_SHELL} make install && timeout 10
    )

ELSE()
    ExternalProject_Add(TARGET_libusb
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}
                        URL ${CMAKE_CURRENT_SOURCE_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}.tar.bz2
                        URL_HASH SHA1=d8d614b538f7c953b6e3b73f1eea5dc70820a7e2
                        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env "CFLAGS=${CMAKE_C_FLAGS}" "CPPFLAGS=${CMAKE_CXX_FLAGS}" ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}${VERSION_EXTERNAL_POST}/src/TARGET_libusb/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/install --enable-static=yes --enable-shared=no --with-pic ${CONFIGURE_HOST}
                        BUILD_COMMAND make
                        INSTALL_COMMAND make install
    )
ENDIF()