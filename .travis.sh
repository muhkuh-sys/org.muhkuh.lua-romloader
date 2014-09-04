#! /bin/sh

#-----------------------------------------------------------------------------
#
# Build the netX firmware.
#
python mbs/mbs
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi


#-----------------------------------------------------------------------------
#
# Build the windows32 version.
#
rm -rf build_windows32
mkdir build_windows32
cd build_windows32

cmake -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=x86 -DCMAKE_C_COMPILER=/usr/bin/i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=/usr/bin/i686-w64-mingw32-g++ -DCMAKE_RC_COMPILER=/usr/bin/i686-w64-mingw32-windres -DUSE_SYSTEM_LUA=FALSE -DSWIG_EXECUTABLE=/usr/bin/swig3.0 ..
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

make install DESTDIR=`pwd`/install
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

cd ..


#-----------------------------------------------------------------------------
#
# Build the windows64 version.
#
rm -rf build_windows64
mkdir build_windows64
cd build_windows64

cmake -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=amd64 -DCMAKE_C_COMPILER=/usr/bin/x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-w64-mingw32-g++ -DCMAKE_RC_COMPILER=/usr/bin/x86_64-w64-mingw32-windres -DUSE_SYSTEM_LUA=FALSE -DSWIG_EXECUTABLE=/usr/bin/swig3.0 ..
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

make install DESTDIR=`pwd`/install
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

cd ..


#-----------------------------------------------------------------------------
#
# Assemble the artifacts.
#
rm -rf build
mkdir build
cd build
cmake ../installer/ivy
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

make
STATUS=$?
if [ $STATUS -ne 0 ]; then
	exit 1
fi

