cmake_minimum_required(VERSION 3.7)

#----------------------------------------------------------------------------
#
# Build libusb and create a CMake component.
#
PROJECT("libusb")

SET(VERSION_EXTERNAL "1.0.21")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
SET(LIBUSB_CMAKE_ARGS "")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
LIST(APPEND LIBUSB_CMAKE_ARGS "-DVERSION_EXTERNAL=${VERSION_EXTERNAL}")

ExternalProject_Add(TARGET_libusb
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/libusb-${VERSION_EXTERNAL}.tar.bz2
                    URL_HASH SHA1=54d71841542eb1a6f0b0420878a4d5434efe8d28
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/libusb-${VERSION_EXTERNAL}/src/TARGET_libusb
                    CMAKE_ARGS ${LIBUSB_CMAKE_ARGS}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_CURRENT_BINARY_DIR}
)
