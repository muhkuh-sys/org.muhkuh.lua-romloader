
# Only build the internal lua version if the system version is not used.
IF(NOT USE_SYSTEM_LUA)
	add_subdirectory(lua-5.1.4)
ENDIF(NOT USE_SYSTEM_LUA)
