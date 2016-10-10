#! /bin/bash
set -e

# This batch file can run in 2 environments:
#
# 1) a linux host with a MinGW crosscompiler
# 2) a windows host with a MSYS environment and a MinGW compiler.
#
# The following tools must be on the path for both environments:
#
#  * ant
#  * MinGW
#  * SWIG

if [[ "${OSTYPE}" == "msys" ]] ; then
	echo Configuring for Windows/MSYS/TDM-GCC
	CMAKE_GEN=MSYS\ Makefiles
	GCC32_PREFIX=
	GCC64_PREFIX=
else
	echo Configuring for Linux
	CMAKE_GEN=Unix\ Makefiles
	GCC32_PREFIX=i686-w64-mingw32-
	GCC64_PREFIX=x86_64-w64-mingw32-
fi

#-----------------------------------------------------------------------------
#
# Get the build requirements with ivy.
#
pushd ivy
ant bootstrap
ant
popd


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

cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=x86 -DCMAKE_C_COMPILER=${GCC32_PREFIX}gcc -DCMAKE_CXX_COMPILER=${GCC32_PREFIX}g++ -DCMAKE_RC_COMPILER=${GCC32_PREFIX}windres -G "${CMAKE_GEN}" ..
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
cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=amd64 -DCMAKE_C_COMPILER=${GCC64_PREFIX}gcc -DCMAKE_CXX_COMPILER=${GCC64_PREFIX}g++ -DCMAKE_RC_COMPILER=${GCC64_PREFIX}windres -G "${CMAKE_GEN}" ..
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
cmake -G "${CMAKE_GEN}" ../installer/ivy
make
make install
popd

