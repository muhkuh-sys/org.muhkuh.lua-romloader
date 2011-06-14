cmake_minimum_required(VERSION 2.8)

project("muhkuh")

set(MUHKUH_VERSION_MAJ 1)
set(MUHKUH_VERSION_MIN 0)
set(MUHKUH_VERSION_SUB 0)
set(MUHKUH_VERSION_YEAR 2010)

find_package(Subversion)
if(Subversion_FOUND)
	Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} MUHKUH_SVN)
	MESSAGE("Current revision is ${MUHKUH_SVN_WC_REVISION}")
	set(MUHKUH_VERSION_SUB ${MUHKUH_SVN_WC_REVISION})
endif(Subversion_FOUND)


find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include(${wxWidgets_USE_FILE})


if(${LUA51_FOUND})
	set_property(SOURCE muhkuh_lua.i PROPERTY CPLUSPLUS ON)
	set_property(SOURCE muhkuh_lua.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)
	SWIG_ADD_MODULE(muhkuh_app_lua_wrapper lua muhkuh_lua.i)
	SET(muhkuh_app_lua_wrapper ${swig_generated_file_fullname})

	set(muhkuh_app_lua_definitions USE_LUA=1)
	set(muhkuh_app_lua_sources
		muhkuh_lua.cpp
		muhkuh_htmltag_lua.cpp
		${muhkuh_app_lua_wrapper}
	)
	set(muhkuh_app_libs ${LUA_LIBRARIES})
	include_directories(${LUA_INCLUDE_DIR})
endif(${LUA51_FOUND})


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${wxWidgets_INCLUDE_DIRS})


# Set the windows specific sources. This will be empty on other platforms.
if(WIN32)
	set(muhkuh_app_win_sources
		muhkuh.rc
	)
endif(WIN32)

set(muhkuh_SOURCES
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

set(muhkuh_HEADERS
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


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh.rc ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh.rc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/muhkuh_version.h ${CMAKE_CURRENT_SOURCE_DIR}/muhkuh_version.h)


# Build the executable.
add_executable(muhkuh WIN32 ${muhkuh_SOURCES})
set_property(TARGET muhkuh PROPERTY COMPILE_DEFINITIONS ${wxWidgets_DEFINITIONS} ${muhkuh_app_lua_definitions} ${muhkuh_app_python_definitions})
target_link_libraries(muhkuh ${wxWidgets_LIBRARIES} ${muhkuh_app_libs})
set_target_properties(muhkuh PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


# Copy the application.
INSTALL(TARGETS muhkuh DESTINATION ${INSTALL_DIR_EXECUTABLES})
# Copy the tips file.
INSTALL(FILES ${CMAKE_HOME_DIRECTORY}/bin/muhkuh_tips.txt DESTINATION ${INSTALL_DIR_EXECUTABLES})
