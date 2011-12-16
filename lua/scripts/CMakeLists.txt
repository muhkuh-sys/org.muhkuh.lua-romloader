cmake_minimum_required(VERSION 2.8)

project("lua_scripts")

if(${LUA51_FOUND})
	set(lua_scripts_install_files
		muhkuh.lua
		muhkuh_system.lua
		select_plugin_cli.lua
		select_plugin_gui.lua
		serialnr.lua
		system_cli.lua
		system_gui.lua
		tester_cli.lua
	)

	# Install the scripts.
	INSTALL(FILES ${lua_scripts_install_files} DESTINATION ${INSTALL_DIR_LUA_SCRIPTS})

	INSTALL(FILES tester/init.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/control.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/details.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/log.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/overview.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/resultgrid.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester)
	INSTALL(FILES tester/style/supercow.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS}/tester/style)

	INSTALL(FILES tester_gui.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS} RENAME tester_old.lua)
endif(${LUA51_FOUND})
