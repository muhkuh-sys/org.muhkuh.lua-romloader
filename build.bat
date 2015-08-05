
rem Build script for TDM-GCC
rem The following tools must be on the path:
rem ant
rem GCC (MinGW/TDM-GCC)
rem SWIG

echo Configuring for Windows/MSYS/TDM-GCC
set CMAKE_GEN="MinGW Makefiles"
set GCC32_PREFIX=
set GCC64_PREFIX=

rem -----------------------------------------------------------------------------
rem 
rem  Get the build requirements with ivy.
rem 
pushd ivy
call ant.bat bootstrap
call ant.bat
popd

rem set IVY_LIBRARY_FOLDER=`pwd`/ivy/lib
set IVY_LIBRARY_FOLDER=./ivy/lib

rem -----------------------------------------------------------------------------
rem 
rem  Build the netX firmware.
rem 
python mbs/mbs


rem -----------------------------------------------------------------------------
rem 
rem  Build the windows32 version.
rem 
rmdir /s /q build_windows32
mkdir build_windows32
pushd build_windows32

cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=x86 -DCMAKE_C_COMPILER=%GCC32_PREFIX%gcc -DCMAKE_CXX_COMPILER=%GCC32_PREFIX%g++ -DCMAKE_RC_COMPILER=%GCC32_PREFIX%windres -DCMAKE_PREFIX_PATH=%IVY_LIBRARY_FOLDER%/org/muhkuh/lua/lua51-5.1.3/windows_x86/cmake -G %CMAKE_GEN% ..
mingw32-make
mingw32-make test
mingw32-make install DESTDIR=./install
popd


rem -----------------------------------------------------------------------------
rem 
rem  Build the windows64 version.
rem 
rmdir /s /q build_windows64
mkdir build_windows64
pushd build_windows64
cmake -DCMAKE_INSTALL_PREFIX="" -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 -DCMAKE_SYSTEM_NAME=Windows -DTARGET_PROCESSOR=amd64 -DCMAKE_C_COMPILER=%GCC64_PREFIX%gcc -DCMAKE_CXX_COMPILER=%GCC64_PREFIX%g++ -DCMAKE_RC_COMPILER=%GCC64_PREFIX%windres -DCMAKE_PREFIX_PATH=%IVY_LIBRARY_FOLDER%/org/muhkuh/lua/lua51-5.1.3/windows_amd64/cmake -G %CMAKE_GEN% ..
mingw32-make
mingw32-make test
mingw32-make install DESTDIR=./install
popd


rem -----------------------------------------------------------------------------
rem 
rem  Assemble the artifacts.
rem 
rmdir /s /q build
mkdir build
pushd build
cmake -G %CMAKE_GEN% ../installer/ivy
mingw32-make
mingw32-make install
popd

