#----------------------------------------------------------------------------
#
# Muhkuh can build modules for the Lua language. This section allows to user
# to choose which lua version should be used.
#

# Use the MBS LUA version on Windows.
IF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
	# Get the path to LUA from the ant system.
	IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
		SET(org.muhkuh.lua-lua51_DIR ${ant_path.org.muhkuh.lua.lua51}/windows_amd64/cmake)
	ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8)
		SET(org.muhkuh.lua-lua51_DIR ${ant_path.org.muhkuh.lua.lua51}/windows_x86/cmake)
	ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)
	
	FIND_PACKAGE(org.muhkuh.lua-lua51 REQUIRED)
	SET(LUA51_FOUND 1)
	GET_TARGET_PROPERTY(LUA_LIBRARIES org.muhkuh.lua-lua51::TARGET_lualib LOCATION)
	GET_TARGET_PROPERTY(LUA_INCLUDE_DIR org.muhkuh.lua-lua51::TARGET_lualib INTERFACE_INCLUDE_DIRECTORIES)
	SET(LUA_VERSION_STRING ${org.muhkuh.lua-lua51_VERSION})
ELSE((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
	FIND_PACKAGE(Lua51 REQUIRED)
ENDIF((${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))

