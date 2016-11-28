#----------------------------------------------------------------------------
#
# Muhkuh can build modules for the Lua language. This section allows to user
# to choose which lua version should be used.
#

OPTION(BUILDCFG_LUA_VERSION "Select the LUA version used in this build. Possible values are 5.1, 5.2 and 5.3. The default is 5.1."
       "5.1")

# Use the MBS LUA version on Windows.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
	# Get the path to LUA from the ant system.
	IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
		LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_HOME_DIRECTORY}/jonchki/build_requirements/windows_x64/cmake)
	ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8)
		LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_HOME_DIRECTORY}/jonchki/build_requirements/windows_x86/cmake)
	ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)

	IF((${BUILDCFG_LUA_VERSION} STREQUAL "5.1"))
		FIND_PACKAGE(org.lua.lua-lua51 REQUIRED)
		SET(LUA_FOUND 1)
		GET_TARGET_PROPERTY(LUA_LIBRARIES org.lua.lua-lua51::TARGET_lualib LOCATION)
		GET_TARGET_PROPERTY(LUA_INCLUDE_DIR org.lua.lua-lua51::TARGET_lualib INTERFACE_INCLUDE_DIRECTORIES)
		SET(LUA_VERSION_STRING ${org.lua.lua-lua51_VERSION})
	ELSEIF((${BUILDCFG_LUA_VERSION} STREQUAL "5.2"))
		FIND_PACKAGE(org.lua.lua-lua52 REQUIRED)
		SET(LUA_FOUND 1)
		GET_TARGET_PROPERTY(LUA_LIBRARIES org.lua.lua-lua52::TARGET_lualib LOCATION)
		GET_TARGET_PROPERTY(LUA_INCLUDE_DIR org.lua.lua-lua52::TARGET_lualib INTERFACE_INCLUDE_DIRECTORIES)
		SET(LUA_VERSION_STRING ${org.lua.lua-lua52_VERSION})
	ELSEIF((${BUILDCFG_LUA_VERSION} STREQUAL "5.3"))
		FIND_PACKAGE(org.lua.lua-lua53 REQUIRED)
		SET(LUA_FOUND 1)
		GET_TARGET_PROPERTY(LUA_LIBRARIES org.lua.lua-lua53::TARGET_lualib LOCATION)
		GET_TARGET_PROPERTY(LUA_INCLUDE_DIR org.lua.lua-lua53::TARGET_lualib INTERFACE_INCLUDE_DIRECTORIES)
		SET(LUA_VERSION_STRING ${org.lua.lua-lua53_VERSION})
	ELSE((${BUILDCFG_LUA_VERSION} STREQUAL "5.1"))
		# Stop here!
		MESSAGE(FATAL_ERROR "The value '${BUILDCFG_LUA_VERSION}' for BUILDCFG_LUA_VERSION is invalid!")
	ENDIF((${BUILDCFG_LUA_VERSION} STREQUAL "5.1"))
ELSE((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
	FIND_PACKAGE(Lua 5.1 REQUIRED EXACT)
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))

