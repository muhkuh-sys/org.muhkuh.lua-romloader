cmake_minimum_required(VERSION 2.8)

PROJECT("LuaBitLibrary")

SET(LuaBitLibrary_VERSION 0.0.1)

FIND_PACKAGE(SWIG REQUIRED)


INCLUDE(${SWIG_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LUA_INCLUDE_DIR})

SET_SOURCE_FILES_PROPERTIES(bit.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(bit.i PROPERTIES SWIG_FLAGS "")
SWIG_ADD_MODULE(TARGET_bit lua bit.i bit.c)
SWIG_LINK_LIBRARIES(TARGET_bit ${LUA_LIBRARIES})

# Set the name of the output file to "bit".
SET_PROPERTY(TARGET TARGET_bit PROPERTY OUTPUT_NAME "bit")

# Do not prepend something like "lib" to the name of the output file.
SET_PROPERTY(TARGET TARGET_bit PROPERTY PREFIX "")

# On mingw link all compiler libraries static.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	set_property(TARGET TARGET_bit PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))


# Install the lua module.
INSTALL(TARGETS TARGET_bit DESTINATION ${INSTALL_DIR_LUA_MODULES})
