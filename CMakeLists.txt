cmake_minimum_required(VERSION 2.8)

PROJECT("muhkuh")

# Swig is required to generate the bindings.
FIND_PACKAGE(SWIG REQUIRED)

# The python interpreter is required to build the netx firmware. (See plugins/romloader/usb/netx)
FIND_PACKAGE(PythonInterp REQUIRED)

# This is the list of the wxwidgets modules required for the application.
FIND_PACKAGE(wxWidgets REQUIRED base core xml net aui adv html xrc)


# Create a new directory for the swig runtime.
SET(SWIG_RUNTIME_OUTPUT_PATH ${CMAKE_BINARY_DIR}/swig_runtime)
MAKE_DIRECTORY(${SWIG_RUNTIME_OUTPUT_PATH})


OPTION(USE_SYSTEM_LUA "Use the systemwide installed lua version instead of the muhkuh internal one." ON)
IF(USE_SYSTEM_LUA)
	# Use the system version.
	FIND_PACKAGE(Lua51 REQUIRED)
ELSE(USE_SYSTEM_LUA)
	# Use the internal lua version.
	SET(LUA51_FOUND TRUE)
	SET(LUA_LIBRARIES lualib)
	SET(LUA_INCLUDE_DIR ${CMAKE_HOME_DIRECTORY}/lua/lib/lua-5.1.4/src/)
ENDIF(USE_SYSTEM_LUA)


ADD_CUSTOM_COMMAND(
	OUTPUT ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h
	COMMAND "${SWIG_EXECUTABLE}" -lua -external-runtime ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h
	COMMENT "Build the swig lua runtime header."
	VERBATIM
)

ADD_CUSTOM_TARGET(swigluarun DEPENDS ${SWIG_RUNTIME_OUTPUT_PATH}/swigluarun.h)


if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
	# Do not show msvs security warnings about unsafe functions.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	# Set warning level to 2.
	add_definitions(/W2)
ENDIF(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")


#add_subdirectory(application)
add_subdirectory(lua)
add_subdirectory(modules)
add_subdirectory(plugins)
