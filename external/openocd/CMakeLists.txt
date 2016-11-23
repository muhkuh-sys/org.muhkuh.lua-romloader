cmake_minimum_required(VERSION 2.8)

PROJECT("openocd")

INCLUDE(ExternalProject)

SET(VERSION_EXTERNAL "0.9.0")


#----------------------------------------------------------------------------
#
# Build the project.
#
SET(OPENOCD_CMAKE_ARGS "")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSB_LIBRARY=${LIBUSB_LIBRARY}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSB_LDFLAGS=${LIBUSB_LDFLAGS}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSB_INCLUDE_DIRECTORIES=${LIBUSB_INCLUDE_DIRECTORIES}")

ExternalProject_Add(TARGET_openocd
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/openocd-${VERSION_EXTERNAL}.tar.bz2
                    URL_HASH SHA1=bcf99bb21f09513065b2c9ece148ace7d16cdd0c
                    PATCH_COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_HOME_DIRECTORY}/cmake/tools/apply_patches.py --working-folder ${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}/src/TARGET_openocd --patch-folder ${CMAKE_CURRENT_SOURCE_DIR}/patches --strip 1 --copy-folder ${CMAKE_CURRENT_SOURCE_DIR}/copy
                    CMAKE_ARGS ${OPENOCD_CMAKE_ARGS}
                    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}
)
ADD_DEPENDENCIES(TARGET_openocd TARGET_libusb)


# 3) Provide all variables for the build.
SET(LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}/install/lib/libopenocd.a)

#IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
#	SET(LDFLAGS "-pthread -lrt -ludev")
#ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
SET(LDFLAGS "")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	SET(DEFINES "-std=c99")
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}/install/include)


#----------------------------------------------------------------------------
#
# Store all information about the library in the cache.
#
SET(OPENOCD_LIBRARY              ${LIBRARY}              CACHE INTERNAL "The static openocd library with the complete path.")
SET(OPENOCD_LDFLAGS              ${LDFLAGS}              CACHE INTERNAL "All linker flags required to link the library. The list must be separated with whitespace.")
SET(OPENOCD_DEFINES              ${DEFINES}              CACHE INTERNAL "All defines for the openocd library. This should be a list of entries starting with -D.")
SET(OPENOCD_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  CACHE INTERNAL "The include folders for the library headers. The list must be separated with whitespace.")

SET(JIMTCL_LIBRARY
    ${CMAKE_CURRENT_BINARY_DIR}/openocd-${VERSION_EXTERNAL}/install/lib/libjim.a              CACHE INTERNAL "The static jimtcl library with the complete path.")
