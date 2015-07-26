#! /bin/bash
set -e


#-----------------------------------------------------------------------------
#
# Get the build requirements with ivy.
#
pushd ivy
ant bootstrap
ant
popd

IVY_LIBRARY_FOLDER=`pwd`/ivy/lib


#-----------------------------------------------------------------------------
#
# Build the netX firmware.
#
python mbs/mbs


#-----------------------------------------------------------------------------
#
# Build the windows32 version.
#
rm -rf build_windows32
mkdir build_windows32
pushd build_windows32
cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=x86 -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DCMAKE_RC_COMPILER=i686-w64-mingw32-windres -DCMAKE_PREFIX_PATH=${IVY_LIBRARY_FOLDER}/org/muhkuh/lua/lua51-5.1.3/windows_x86/cmake ..
make
make test
make install DESTDIR=`pwd`/install
popd


#-----------------------------------------------------------------------------
#
# Build the windows64 version.
#
rm -rf build_windows64
mkdir build_windows64
pushd build_windows64
cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=amd64 -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres -DCMAKE_PREFIX_PATH=${IVY_LIBRARY_FOLDER}/org/muhkuh/lua/lua51-5.1.3/windows_amd64/cmake ..
make
make test
make install DESTDIR=`pwd`/install
popd


#-----------------------------------------------------------------------------
#
# Assemble the artifacts.
#
rm -rf build
mkdir build
pushd build
cmake ../installer/ivy
make
make install
popd

