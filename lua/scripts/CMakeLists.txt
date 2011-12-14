cmake_minimum_required(VERSION 2.8)

project("lua_scripts")

if(${LUA51_FOUND})
	set(lua_scripts_install_files
		muhkuh.lua
		muhkuh_system.lua
		select_plugin_cli.lua
		select_plugin_gui.lua
		system_cli.lua
		system_gui.lua
		tester_cli.lua
	)

#		tester_v2/tester.lua
#		tester_v2/tester_control.lua
#		tester_v2/tester_details.lua
#		tester_v2/tester_log.lua
#		tester_v2/tester_overview.lua
#		tester_v2/tester_resultgrid.lua
#		tester_v2/tester_style_supercow.lua

	# Install the scripts.
	INSTALL(FILES ${lua_scripts_install_files} DESTINATION ${INSTALL_DIR_LUA_SCRIPTS})

	INSTALL(FILES tester_gui.lua DESTINATION ${INSTALL_DIR_LUA_SCRIPTS} RENAME tester.lua)
endif(${LUA51_FOUND})
