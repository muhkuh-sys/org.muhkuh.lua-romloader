cmake_minimum_required(VERSION 2.8)

PROJECT("lua_lib_lua")

SET(lua_lib_lua_VERSION 5.1.4)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(lua_lib_lua_SOURCES
	src/lapi.c
	src/lcode.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/llex.c
	src/lmem.c
	src/lobject.c
	src/lopcodes.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/ltable.c
	src/ltm.c
	src/lundump.c
	src/lvm.c
	src/lzio.c
	src/lauxlib.c
	src/lbaselib.c
	src/ldblib.c
	src/liolib.c
	src/lmathlib.c
	src/loslib.c
	src/ltablib.c
	src/lstrlib.c
	src/loadlib.c
	src/linit.c
)

SET(lua_lib_lua_HEADERS
	src/lapi.h
	src/lauxlib.h
	src/lcode.h
	src/ldebug.h
	src/ldo.h
	src/lfunc.h
	src/lgc.h
	src/llex.h
	src/llimits.h
	src/lmem.h
	src/lobject.h
	src/lopcodes.h
	src/lparser.h
	src/lstate.h
	src/lstring.h
	src/ltable.h
	src/ltm.h
	src/luaconf.h
	src/lua.h
	src/lualib.h
	src/lundump.h
	src/lvm.h
	src/lzio.h
)

IF(UNIX)
	add_definitions(-DLUA_USE_LINUX)
	set(lua_libs m dl readline)
ELSE(UNIX)
	IF(WIN32)
		add_definitions(-DLUA_BUILD_AS_DLL)
		set(lua_libs )
	ENDIF(WIN32)
ENDIF(UNIX)


add_library(lualib SHARED ${lua_lib_lua_SOURCES})

add_executable(luaexe src/lua.c)
target_link_libraries(luaexe lualib ${lua_libs})
set_target_properties(luaexe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

set_target_properties(lualib PROPERTIES OUTPUT_NAME lua5.1)
set_target_properties(luaexe PROPERTIES OUTPUT_NAME lua)

INSTALL(TARGETS luaexe DESTINATION ${INSTALL_DIR_EXECUTABLES})
INSTALL(TARGETS lualib DESTINATION ${INSTALL_DIR_SHARED_OBJECTS})
