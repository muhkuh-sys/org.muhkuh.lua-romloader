CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("muhkuh")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
IF(${LUA51_FOUND})
	INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
ENDIF(${LUA51_FOUND})


#----------------------------------------------------------------------------
#
# Build the muhkuh application.
#

# Set the windows specific sources. This will be empty on other platforms.
IF(WIN32)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh.rc ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh.rc)

SET(muhkuh_app_win_SOURCES
	muhkuh.rc
)
ENDIF(WIN32)


IF(${LUA51_FOUND})
	# Lua found -> use lua.
	SET(muhkuh_app_DEFINITIONS_lua USE_LUA=1)

	# Set the additional sources needed besides the generated wrapper.
	SET(muhkuh_app_SOURCES_lua
		muhkuh_htmltag_lua.cpp
	)
	SET(muhkuh_app_HEADERS_lua
		muhkuh_lua.h
	)
	SET(muhkuh_app_LIBS_lua ${LUA_LIBRARIES})
ENDIF(${LUA51_FOUND})


SET(muhkuh_app_SOURCES
	growbuffer.cpp
	license.cpp
	muhkuh_aboutDialog.cpp
	muhkuh_app.cpp
	muhkuh_available_languages.cpp
	muhkuh_brokenPluginDialog.cpp
	muhkuh_configDialog.cpp
	muhkuh_config_data.cpp
	muhkuh_config_reposEntryDialog.cpp
	muhkuh_copy_process.cpp
	muhkuh_dirlistbox.cpp
	muhkuh_flags.cpp
	muhkuh_icons.cpp
	muhkuh_lua.cpp
	muhkuh_mainFrame.cpp
	muhkuh_plugin.cpp
	muhkuh_plugin_manager.cpp
	muhkuh_repository.cpp
	muhkuh_repository_manager.cpp
	muhkuh_split_testdescription.cpp
	muhkuh_wrap_xml.cpp
	readFsFile.cpp
)

SET(muhkuh_app_HEADERS
	growbuffer.h
	license.h
	muhkuh_aboutDialog.h
	muhkuh_app.h
	muhkuh_available_languages.h
	muhkuh_configDialog.h
	muhkuh_config_data.h
	muhkuh_config_reposEntryDialog.h
	muhkuh_copy_process.h
	muhkuh_dirlistbox.h
	muhkuh_flags.h
	muhkuh_icons.h
	muhkuh_id.h
	muhkuh_lua.h
	muhkuh_mainFrame.h
	muhkuh_plugin.h
	muhkuh_plugin_manager.h
	muhkuh_repository.h
	muhkuh_repository_manager.h
	muhkuh_split_testdescription.h
	muhkuh_testTreeItemData.h
	muhkuh_version.h
	muhkuh_wrap_xml.h
	readFsFile.h
)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh_version.h ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh_version.h)


# Build the executable.
ADD_EXECUTABLE(muhkuh WIN32
	${muhkuh_app_SOURCES}
	${muhkuh_app_SOURCES_lua}
	${muhkuh_app_HEADERS}
	${muhkuh_app_HEADERS_lua})

SET_PROPERTY(TARGET muhkuh PROPERTY COMPILE_DEFINITIONS
	${wxWidgets_DEFINITIONS}
	${muhkuh_app_DEFINITIONS_lua})

TARGET_LINK_LIBRARIES(muhkuh
	${wxWidgets_LIBRARIES}
	${muhkuh_app_LIBS_lua})

SET_TARGET_PROPERTIES(muhkuh PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


# Copy the application.
INSTALL(TARGETS muhkuh DESTINATION ${INSTALL_DIR_EXECUTABLES})
# Copy the tips file.
INSTALL(FILES ${CMAKE_HOME_DIRECTORY}/bin/muhkuh_tips.txt DESTINATION ${INSTALL_DIR_EXECUTABLES})
