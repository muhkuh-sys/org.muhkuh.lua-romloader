cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})

SET(SWIG_RUNTIME_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/swig_runtime/)
MAKE_DIRECTORY(${SWIG_RUNTIME_OUTPUT_PATH})

OPTION(USE_SYSTEM_LUA "Use the systemwide installed lua version instead of the muhkuh internal one." OFF)
IF(USE_SYSTEM_LUA)
	# Use the system version.
	FIND_PACKAGE(Lua51 REQUIRED)
ELSE(USE_SYSTEM_LUA)
	# Use the internal lua version.
	SET(LUA_LIBRARIES lua)
	SET(LUA_INCLUDE_DIR ${CMAKE_HOME_DIRECTORY}/lua/lib/lua-5.1.4/src/)
ENDIF(USE_SYSTEM_LUA)


ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
	COMMAND swig -lua -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h
)
ADD_CUSTOM_TARGET(SWIG_LUA_RUNTIME DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}swigluarun.h)


add_subdirectory(lua)
add_subdirectory(plugins)
