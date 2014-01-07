cmake_minimum_required(VERSION 2.8)

PROJECT("openocd")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#

# FIXME: The patches are stored in the 'patches' subfolder. These files are
# not used because I have not found a portable way to apply them. I do not
# want to rely on extra tools like GNU patch. While it is quite common on
# linux platforms, it is very special on windows.
# A nice way is the 'google-diff-match-patch' package here:
#   http://code.google.com/p/google-diff-match-patch/
# Looks like I can simulate a GNU patch with 'patch_fromText' and
# 'patch_apply', but this is a bit more work. So for now the already patched
# files are stored in the templates folder. They are simply copied over the
# original files.

SET(OPENOCD_CMAKE_ARGS "")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
IF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	LIST(APPEND OPENOCD_CMAKE_ARGS "-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}")
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSBX_LIBRARY=${LIBUSBX_LIBRARY}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSBX_LDFLAGS=${LIBUSBX_LDFLAGS}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSBX_DEFINES=${LIBUSBX_DEFINES}")
LIST(APPEND OPENOCD_CMAKE_ARGS "-DLIBUSBX_INCLUDE_DIRECTORIES=${LIBUSBX_INCLUDE_DIRECTORIES}")

ExternalProject_Add(TARGET_openocd
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/openocd-0.8.0-dev.tar.bz2
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev/src/TARGET_openocd
                    CMAKE_ARGS ${OPENOCD_CMAKE_ARGS}
                    INSTALL_COMMAND make install DESTDIR=${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev
)
ADD_DEPENDENCIES(TARGET_openocd TARGET_libusbx)


# 3) Provide all variables for the build.
SET(LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev/install/lib/libopenocd.a)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	SET(LDFLAGS "-pthread -lrt -ludev")
ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"  AND  "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

SET(DEFINES "")
SET(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev/install/include)

#----------------------------------------------------------------------------
#
# Store all information about the library in the cache.
#
#SET(LIBUSBX_LIBRARY              ${LIBRARY}              CACHE INTERNAL "The libusbx library with the complete path.")
#SET(LIBUSBX_LDFLAGS              ${LDFLAGS}              CACHE INTERNAL "All linker flags required to link the library. The list must be separated with whitespace.")
#SET(LIBUSBX_DEFINES              ${DEFINES}              CACHE INTERNAL "All defines for the libusbx library. This should be a list of entries starting with -D.")
#SET(LIBUSBX_INCLUDE_DIRECTORIES  ${INCLUDE_DIRECTORIES}  CACHE INTERNAL "The include folders for the library headers. The list must be separated with whitespace.")
