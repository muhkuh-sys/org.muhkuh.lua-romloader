#! /bin/bash
set -e

#-----------------------------------------------------------------------------
#
# Build all artefacts.
#
rm -rf build
mkdir build

mkdir build/org.muhkuh.lua-lua5.1-romloader
pushd build/org.muhkuh.lua-lua5.1-romloader
cmake -DCMAKE_INSTALL_PREFIX="" ../../jonchki/installer/lua5.1
make
make package
popd

mkdir build/org.muhkuh.lua-lua5.2-romloader
pushd build/org.muhkuh.lua-lua5.2-romloader
cmake -DCMAKE_INSTALL_PREFIX="" ../../jonchki/installer/lua5.2
make
make package
popd

mkdir build/org.muhkuh.lua-lua5.3-romloader
pushd build/org.muhkuh.lua-lua5.3-romloader
cmake -DCMAKE_INSTALL_PREFIX="" ../../jonchki/installer/lua5.3
make
make package
popd
