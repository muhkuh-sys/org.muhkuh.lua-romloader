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
	INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
ENDIF(${LUA51_FOUND})

#----------------------------------------------------------------------------
#
# Build the muhkuh_components dll.
#
# The muhkuh components dll contains classes and functions shared by the main
# application and the script bindings.
#

# Add lua support (if found).
IF(${LUA51_FOUND})
	# Lua found -> use lua.
	SET(muhkuh_components_DEFINITIONS_lua USE_LUA=1)
	# Set the additional sources needed besides the generated wrapper.
	SET(muhkuh_components_SOURCES_lua
		muhkuh_lua.cpp
	)
	SET(muhkuh_components_HEADERS_lua
		muhkuh_lua.h
	)
	SET(muhkuh_components_LIBS_lua ${LUA_LIBRARIES})
ENDIF(${LUA51_FOUND})


SET(muhkuh_components_SOURCES
	muhkuh_config_data.cpp
	muhkuh_plugin.cpp
	muhkuh_plugin_manager.cpp
	muhkuh_repository.cpp
	muhkuh_repository_manager.cpp
)

SET(muhkuh_components_HEADERS
	muhkuh_config_data.h
	muhkuh_plugin.h
	muhkuh_plugin_manager.h
	muhkuh_repository.h
	muhkuh_repository_manager.h
	muhkuh_so_api.h
)

ADD_LIBRARY(muhkuh_components SHARED
	${muhkuh_components_SOURCES}
	${muhkuh_components_SOURCES_lua}
	${muhkuh_components_HEADERS}
	${muhkuh_components_HEADERS_lua})
SET_PROPERTY(TARGET muhkuh_components APPEND PROPERTY COMPILE_DEFINITIONS
	__BUILD_SHARED_OBJECT__
	${muhkuh_components_DEFINITIONS_lua})
TARGET_LINK_LIBRARIES(muhkuh_components
	${wxWidgets_LIBRARIES}
	${muhkuh_components_LIBS_lua})

# Install the targets.
INSTALL(TARGETS muhkuh_components DESTINATION ${INSTALL_DIR_SHARED_OBJECTS})

#----------------------------------------------------------------------------
#
# Build a lua wrapper for the muhkuh components.
#
# This part builds a lua plugin for the muhkuh application. Initially I wanted to link
# this plugin statically to the main application, but this resulted in a strange crash.
# Mixing static and dynamic modules seems to be a problem. A new lua state in the muhkuh
# application got the static binding first. Then on demand the dynamic modules were loaded.
# This worked fine for the first state. Loading the dynamic modules in another state crashed.
# The reason for the crash was some trashed cyclic list.
# I have to admit that I have absolutely no idea why this happens. So I changed the static
# module to a dynamic. :(
#

IF(${LUA51_FOUND})
	SET(muhkuh_components_lua_SOURCES
		muhkuh_components.cpp
	)
	SET(muhkuh_components_lua_HEADERS
		muhkuh_components.h
	)
	SET_PROPERTY(SOURCE muhkuh_components.i PROPERTY CPLUSPLUS ON)
	SET_PROPERTY(SOURCE muhkuh_components.i PROPERTY SWIG_FLAGS -I${CMAKE_HOME_DIRECTORY}/modules/typemaps)
	SWIG_ADD_MODULE(muhkuh_components_lua lua muhkuh_components.i
		${muhkuh_components_lua_SOURCES}
		${muhkuh_components_lua_HEADERS})
	SWIG_LINK_LIBRARIES(muhkuh_components_lua
		muhkuh_components
		${LUA_LIBRARIES}
		${wxWidgets_LIBRARIES})
	SET_PROPERTY(TARGET muhkuh_components_lua PROPERTY PREFIX "")

	# Install the targets.
	INSTALL(TARGETS muhkuh_components_lua DESTINATION ${INSTALL_DIR_LUA_MODULES})
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
	muhkuh_brokenPluginDialog.cpp
	muhkuh_configDialog.cpp
	muhkuh_config_reposEntryDialog.cpp
	muhkuh_copy_process.cpp
	muhkuh_dirlistbox.cpp
	muhkuh_icons.cpp
	muhkuh_mainFrame.cpp
	muhkuh_wrap_xml.cpp
	readFsFile.cpp
)

SET(muhkuh_app_HEADERS
	growbuffer.h
	license.h
	muhkuh_aboutDialog.h
	muhkuh_app.h
	muhkuh_configDialog.h
	muhkuh_config_reposEntryDialog.h
	muhkuh_copy_process.h
	muhkuh_dirlistbox.h
	muhkuh_icons.h
	muhkuh_id.h
	muhkuh_mainFrame.h
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
	${muhkuh_app_SOURCES_win}
	${muhkuh_app_HEADERS}
	${muhkuh_app_HEADERS_lua})
SET_PROPERTY(TARGET muhkuh PROPERTY COMPILE_DEFINITIONS
	${wxWidgets_DEFINITIONS}
	${muhkuh_app_DEFINITIONS_lua})
TARGET_LINK_LIBRARIES(muhkuh
	${wxWidgets_LIBRARIES}
	muhkuh_components
	${muhkuh_app_LIBS_lua})
SET_TARGET_PROPERTIES(muhkuh PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


# Copy the application.
INSTALL(TARGETS muhkuh DESTINATION ${INSTALL_DIR_EXECUTABLES})
# Copy the tips file.
INSTALL(FILES ${CMAKE_HOME_DIRECTORY}/bin/muhkuh_tips.txt DESTINATION ${INSTALL_DIR_EXECUTABLES})
