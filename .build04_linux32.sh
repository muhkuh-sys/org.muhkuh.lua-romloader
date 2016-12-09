#! /bin/bash
set -e

#-----------------------------------------------------------------------------
#
# Build the windows32 versions.
#
PRJ_HOME=`pwd`

# Here are the folders where the external components are installed as CMAKE modules.
CMAKE_PREFIX_PATH="${PRJ_HOME}/build_windows32/external/openocd/install/cmake;${PRJ_HOME}/build_windows32/external/libusb/install/cmake"

# These are the common CMAKE options.
CMAKE_COMMON_OPTIONS="-DCMAKE_INSTALL_PREFIX='' -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DCMAKE_RC_COMPILER=i686-w64-mingw32-windres"


rm -rf build_windows32
mkdir build_windows32

mkdir build_windows32/external
pushd build_windows32/external
cmake ${CMAKE_COMMON_OPTIONS} ../../external
make
popd

mkdir build_windows32/lua5.1
pushd build_windows32/lua5.1
cmake -DBUILDCFG_LUA_VERSION="5.1" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd

mkdir build_windows32/lua5.2
pushd build_windows32/lua5.2
cmake -DBUILDCFG_LUA_VERSION="5.2" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd

mkdir build_windows32/lua5.3
pushd build_windows32/lua5.3
cmake -DBUILDCFG_LUA_VERSION="5.3" -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${CMAKE_COMMON_OPTIONS} ../..
make
make test
make install DESTDIR=`pwd`/install
popd
