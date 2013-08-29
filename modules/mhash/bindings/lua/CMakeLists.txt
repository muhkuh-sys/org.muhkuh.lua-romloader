CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("module_mhash_lua")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})

# Build the Lua module.
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES SWIG_FLAGS "-I${CMAKE_HOME_DIRECTORY}/modules/typemaps")
SWIG_ADD_MODULE(TARGET_module_mhash_lua lua ../mhash.i)
SWIG_LINK_LIBRARIES(TARGET_module_mhash_lua module_mhash_lib ${LUA_LIBRARIES})

# Rename the output file to "mhash".
SET_PROPERTY(TARGET TARGET_module_mhash_lua PROPERTY PREFIX "")
SET_PROPERTY(TARGET TARGET_module_mhash_lua PROPERTY OUTPUT_NAME "mhash")

# On mingw link all compiler libraries static.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))
	set_property(TARGET TARGET_module_mhash_lua PROPERTY LINK_FLAGS "-static-libgcc -static-libstdc++")
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND (${CMAKE_COMPILER_IS_GNUCC}))

# Install the lua module.
INSTALL(TARGETS TARGET_module_mhash_lua DESTINATION ${INSTALL_DIR_LUA_MODULES})
