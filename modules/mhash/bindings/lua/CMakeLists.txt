CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("module_mhash_lua")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})

# Build the Lua module.
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(../mhash.i PROPERTIES SWIG_FLAGS "-I${CMAKE_HOME_DIRECTORY}/modules/typemaps")
SWIG_ADD_MODULE(module_mhash_lua lua ../mhash.i)
SWIG_LINK_LIBRARIES(module_mhash_lua module_mhash_lib ${LUA_LIBRARIES})

# Rename the output file to "mhash".
SET_PROPERTY(TARGET module_mhash_lua PROPERTY PREFIX "")
SET_PROPERTY(TARGET module_mhash_lua PROPERTY OUTPUT_NAME "mhash")

# Install the lua module.
INSTALL(TARGETS module_mhash_lua DESTINATION ${INSTALL_DIR_LUA_MODULES})
