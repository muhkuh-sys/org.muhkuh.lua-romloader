cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})

SET(SWIG_RUNTIME_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/swig_runtime/)
MAKE_DIRECTORY(${SWIG_RUNTIME_OUTPUT_PATH})

SET(MODULE_LUA_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/lua)
MAKE_DIRECTORY(${MODULE_LUA_OUTPUT_PATH})

SET(MODULE_PYTHON_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/python)
MAKE_DIRECTORY(${MODULE_PYTHON_OUTPUT_PATH})
# NOTE: This is a little bit dirty as it sets the path for all generated modules, but it is only needed for python.
# TODO: How to specify this for a single target?
SET(CMAKE_SWIG_OUTDIR ${MODULE_PYTHON_OUTPUT_PATH})

OPTION(USE_SYSTEM_LUA "Use the systemwide installed lua version instead of the muhkuh internal one." OFF)
IF(USE_SYSTEM_LUA)
	# Use the system version.
	FIND_PACKAGE(Lua51 REQUIRED)
ELSE(USE_SYSTEM_LUA)
	# Use the internal lua version.
	SET(LUA51_FOUND TRUE)
	SET(LUA_LIBRARIES lua)
	SET(LUA_INCLUDE_DIR ${CMAKE_HOME_DIRECTORY}/lua/lib/lua-5.1.4/src/)
ENDIF(USE_SYSTEM_LUA)

FIND_PACKAGE(PythonLibs)

ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
	COMMAND swig -lua -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
)
ADD_CUSTOM_TARGET(SWIG_LUA_RUNTIME DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h)

ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h
	COMMAND swig -python -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h
)
ADD_CUSTOM_TARGET(SWIG_PYTHON_RUNTIME DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}swigpythonrun.h)


add_subdirectory(lua)
add_subdirectory(modules)
add_subdirectory(plugins)
