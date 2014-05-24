#----------------------------------------------------------------------------
#
# Set some installation folders.
#

# Disable a prefix like /usr/local on linux.
SET(CMAKE_INSTALL_PREFIX "")

# This is the directory structure used for installation.
SET(INSTALL_DIR_EXECUTABLES     "bin"          CACHE INTERNAL "Install the compiled executables in this path.")
SET(INSTALL_DIR_LUA_MODULES     "lua_plugins"  CACHE INTERNAL "Install the compiled LUA modules in this path.")
SET(INSTALL_DIR_LUA_SCRIPTS     "lua"          CACHE INTERNAL "Install the LUA scripts in this path.")
SET(INSTALL_DIR_SHARED_OBJECTS  "bin"          CACHE INTERNAL "Install the shared objects in this path.")
