# Do not call this script directly. It is included from the .build??_*.sh files.

echo "PRJ_DIR        = ${PRJ_DIR}"
echo "BUILD_DIR      = ${BUILD_DIR}"
echo "CMAKE_COMPILER = ${CMAKE_COMPILER}"
echo "JONCHKI        = ${JONCHKI}"
echo "JONCHKI_SYSTEM = ${JONCHKI_SYSTEM}"

# Create all folders.
rm -rf ${BUILD_DIR}
mkdir -p ${BUILD_DIR}
mkdir ${BUILD_DIR}/external
mkdir ${BUILD_DIR}/lua5.1
mkdir ${BUILD_DIR}/lua5.1/build_requirements
mkdir ${BUILD_DIR}/lua5.2
mkdir ${BUILD_DIR}/lua5.2/build_requirements
mkdir ${BUILD_DIR}/lua5.3
mkdir ${BUILD_DIR}/lua5.3/build_requirements


# Build the external components.
pushd ${BUILD_DIR}/external
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="ON" -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${PRJ_DIR}/external
make
popd

# Get the additional CMAKE search paths.
CMAKE_PATHS="-Dopenocd_DIR='${BUILD_DIR}/external/openocd/install/cmake' -Dlibusb_DIR='${BUILD_DIR}/external/libusb/install/cmake'"

# Get the build requirements for the LUA5.1 version.
pushd ${BUILD_DIR}/lua5.1/build_requirements
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="ON" -DBUILDCFG_LUA_VERSION="5.1" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${PRJ_DIR}
make
lua5.1 ${JONCHKI} --verbose debug --syscfg ${PRJ_DIR}/jonchki/jonchkisys.cfg --prjcfg ${PRJ_DIR}/jonchki/jonchkicfg.xml ${JONCHKI_SYSTEM} --build-dependencies lua5.1-romloader-*.xml
popd

# Build the LUA5.1 version.
pushd ${BUILD_DIR}/lua5.1
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="OFF" -DBUILDCFG_LUA_VERSION="5.1" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DJONCHKI_BUILD_REQUIREMENTS_INSTALL_DIR=${BUILD_DIR}/lua5.1/build_requirements/jonchki/install -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${CMAKE_PATHS} ${PRJ_DIR}
make
make test
make install DESTDIR=${BUILD_DIR}/lua5.1/install
popd

# Get the build requirements for the LUA5.2 version.
pushd ${BUILD_DIR}/lua5.2/build_requirements
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="ON" -DBUILDCFG_LUA_VERSION="5.2" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${PRJ_DIR}
make
lua5.1 ${JONCHKI} --verbose info --syscfg ${PRJ_DIR}/jonchki/jonchkisys.cfg --prjcfg ${PRJ_DIR}/jonchki/jonchkicfg.xml ${JONCHKI_SYSTEM} --build-dependencies lua5.2-romloader-*.xml
popd

# Build the LUA5.2 version.
pushd ${BUILD_DIR}/lua5.2
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="OFF" -DBUILDCFG_LUA_VERSION="5.2" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DJONCHKI_BUILD_REQUIREMENTS_INSTALL_DIR=${BUILD_DIR}/lua5.2/build_requirements/jonchki/install -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${CMAKE_PATHS} ${PRJ_DIR}
make
make test
make install DESTDIR=${BUILD_DIR}/lua5.2/install
popd

# Get the build requirements for the LUA5.3 version.
pushd ${BUILD_DIR}/lua5.3/build_requirements
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="ON" -DBUILDCFG_LUA_VERSION="5.3" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${PRJ_DIR}
make
lua5.1 ${JONCHKI} --verbose info --syscfg ${PRJ_DIR}/jonchki/jonchkisys.cfg --prjcfg ${PRJ_DIR}/jonchki/jonchkicfg.xml ${JONCHKI_SYSTEM} --build-dependencies lua5.3-romloader-*.xml
popd

# Build the LUA5.3 version.
pushd ${BUILD_DIR}/lua5.3
cmake -DBUILDCFG_ONLY_JONCHKI_CFG="OFF" -DBUILDCFG_LUA_VERSION="5.3" -DBUILDCFG_LUA_USE_SYSTEM="OFF" -DJONCHKI_BUILD_REQUIREMENTS_INSTALL_DIR=${BUILD_DIR}/lua5.3/build_requirements/jonchki/install -DCMAKE_INSTALL_PREFIX="" ${CMAKE_COMPILER} ${CMAKE_PATHS} ${PRJ_DIR}
make
make test
make install DESTDIR=${BUILD_DIR}/lua5.3/install
popd
