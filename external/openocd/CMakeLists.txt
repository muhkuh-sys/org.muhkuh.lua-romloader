cmake_minimum_required(VERSION 2.8)

PROJECT("openocd")

INCLUDE(ExternalProject)


#----------------------------------------------------------------------------
#
# Build the project.
#
ExternalProject_Add(TARGET_openocd
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev
                    URL ${CMAKE_CURRENT_SOURCE_DIR}/openocd-0.8.0-dev.tar.bz2
                    PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/templates ${CMAKE_CURRENT_BINARY_DIR}/openocd-0.8.0-dev/src/TARGET_openocd
                    CMAKE_ARGS -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                               -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                               -DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
                               -DLIBUSBX_LIBRARY=${LIBUSBX_LIBRARY}
                               -DLIBUSBX_LDFLAGS=${LIBUSBX_LDFLAGS}
                               -DLIBUSBX_DEFINES=${LIBUSBX_DEFINES}
                               -DLIBUSBX_INCLUDE_DIRECTORIES=${LIBUSBX_INCLUDE_DIRECTORIES}
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
