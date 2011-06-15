CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("muhkuh")

SET(MUHKUH_VERSION_MAJ 1)
SET(MUHKUH_VERSION_MIN 0)
SET(MUHKUH_VERSION_SUB 0)
SET(MUHKUH_VERSION_YEAR 2010)

FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
	Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} MUHKUH_SVN)
	MESSAGE("Current revision is ${MUHKUH_SVN_WC_REVISION}")
	SET(MUHKUH_VERSION_SUB ${MUHKUH_SVN_WC_REVISION})
ENDIF(Subversion_FOUND)


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

INCLUDE(${wxWidgets_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})


IF(${LUA51_FOUND})
	# This part builds a dummy wrapper for the muhkuh application.
	# It seems to be to only way to get the wrapper sources generated without too much hacking. :(
    SET(muhkuh_app_dummy_wrapper_sources
        muhkuh_lua.cpp
        muhkuh_plugin_manager.cpp
        muhkuh_plugin.cpp
    )
	SET_PROPERTY(SOURCE muhkuh_lua.i PROPERTY CPLUSPLUS ON)
	SET_PROPERTY(SOURCE muhkuh_lua.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)
	SWIG_ADD_MODULE(muhkuh_app_dummy_wrapper lua muhkuh_lua.i ${muhkuh_app_dummy_wrapper_sources})
	SWIG_LINK_LIBRARIES(muhkuh_app_dummy_wrapper ${LUA_LIBRARIES} ${wxWidgets_LIBRARIES})

	# Lua found -> use lua.
	SET(muhkuh_app_lua_definitions USE_LUA=1)
	# Remember the filename of the gernerated lua wrapper.
	SET(muhkuh_app_lua_wrapper ${swig_generated_file_fullname})
	# Set the additional sources needed besides the generated wrapper.
	SET(muhkuh_app_lua_sources
		muhkuh_lua.cpp
		muhkuh_htmltag_lua.cpp
		${muhkuh_app_lua_wrapper}
	)
	SET(muhkuh_app_lua_libs ${LUA_LIBRARIES})
	INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
ENDIF(${LUA51_FOUND})


# Set the windows specific sources. This will be empty on other platforms.
IF(WIN32)
	set(muhkuh_app_win_sources
		muhkuh.rc
	)
ENDIF(WIN32)

SET(muhkuh_SOURCES
	growbuffer.cpp
	license.cpp
	muhkuh_aboutDialog.cpp
	muhkuh_app.cpp
	muhkuh_brokenPluginDialog.cpp
	muhkuh_configDialog.cpp
	muhkuh_config_reposEntryDialog.cpp
	muhkuh_dirlistbox.cpp
	muhkuh_icons.cpp
	muhkuh_mainFrame.cpp
	muhkuh_plugin.cpp
	muhkuh_plugin_manager.cpp
	muhkuh_repository.cpp
	muhkuh_repository_manager.cpp
	muhkuh_wrap_xml.cpp
	readFsFile.cpp
	${muhkuh_app_lua_sources}
	${muhkuh_app_python_sources}
	${muhkuh_app_win_sources}
)

SET(muhkuh_HEADERS
	growbuffer.h
	license.h
	muhkuh_aboutDialog.h
	muhkuh_app.h
	muhkuh_configDialog.h
	muhkuh_config_reposEntryDialog.h
	muhkuh_dirlistbox.h
	muhkuh_flags.h
	muhkuh_icons.h
	muhkuh_id.h
	muhkuh_mainFrame.h
	muhkuh_repository.h
	muhkuh_repository_manager.h
	muhkuh_testTreeItemData.h
	muhkuh_version.h
	muhkuh_wrap_xml.h
	readFsFile.h
)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh.rc ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh.rc)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh_version.h ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh_version.h)


# Build the executable.
ADD_EXECUTABLE(muhkuh WIN32 ${muhkuh_SOURCES})
SET_PROPERTY(TARGET muhkuh PROPERTY COMPILE_DEFINITIONS ${wxWidgets_DEFINITIONS} ${muhkuh_app_lua_definitions} ${muhkuh_app_python_definitions})
TARGET_LINK_LIBRARIES(muhkuh ${wxWidgets_LIBRARIES} ${muhkuh_app_lua_libs})
SET_TARGET_PROPERTIES(muhkuh PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


# Copy the application.
INSTALL(TARGETS muhkuh DESTINATION ${INSTALL_DIR_EXECUTABLES})
# Copy the tips file.
INSTALL(FILES ${CMAKE_HOME_DIRECTORY}/bin/muhkuh_tips.txt DESTINATION ${INSTALL_DIR_EXECUTABLES})
