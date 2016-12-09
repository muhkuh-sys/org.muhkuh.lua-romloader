#! /bin/bash
set -e

#-----------------------------------------------------------------------------
#
# Build the linux 64bit versions.
#
PRJ_HOME=`pwd`

# Here are the folders where the external components are installed as CMAKE modules.
CMAKE_PREFIX_PATH="${PRJ_HOME}/build_linux64/external/openocd/install/cmake;${PRJ_HOME}/build_linux64/external/libusb/install/cmake"

# These are the common CMAKE options.
CMAKE_COMMON_OPTIONS="-DCMAKE_INSTALL_PREFIX='' -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64"


rm -rf build_linux64
mkdir build_linux64

mkdir build_linux64/external
pushd build_linux64/external
cmake ${CMAKE_COMMON_OPTIONS} ../../external
make
popd

mkdir build_linux64/lua5.1
pushd build_linux64/lua5.1
cmake -DBUILDCFG_LUA_VERSION="5.1" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd

mkdir build_linux64/lua5.2
pushd build_linux64/lua5.2
cmake -DBUILDCFG_LUA_VERSION="5.2" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd

mkdir build_linux64/lua5.3
pushd build_linux64/lua5.3
cmake -DBUILDCFG_LUA_VERSION="5.3" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd
