#! /bin/bash
set -e

# Get the project directory.
PRJDIR=`pwd`

# Make sure that the "build" folder exists.
# NOTE: do not remove it, maybe there are already components.
mkdir -p ${PRJDIR}/build

# Start the container and get the ID.
ID=`docker run --detach --interactive --volume ${PRJDIR}:/tmp/work mbs_ubuntu_1604 /bin/bash`

# Update the package list to prevent "not found" messages.
docker exec ${ID} bash -c 'apt-get update --assume-yes'

# Install the project specific packages.
docker exec ${ID} bash -c 'apt-get install --assume-yes lua5.1 lua-filesystem lua-expat lua51-mhash lua-curl lua-zip swig3 libudev-dev:i386'

# Build the netX firmware.
docker exec ${ID} bash -c 'cd /tmp/work && bash .build01_netx_firmware.sh'

# Build the 32bit version.
docker exec ${ID} bash -c 'cd /tmp/work && bash .build04_linux32.sh'
docker exec ${ID} bash -c 'tar --create --file /tmp/work/build/build_lua5.1_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux32/lua5.1/install .'
docker exec ${ID} bash -c 'tar --create --file /tmp/work/build/build_lua5.2_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux32/lua5.2/install .'
docker exec ${ID} bash -c 'tar --create --file /tmp/work/build/build_lua5.3_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux32/lua5.3/install .'
docker exec ${ID} bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.1_ubuntu_1604_x86.tar.gz'
docker exec ${ID} bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.2_ubuntu_1604_x86.tar.gz'
docker exec ${ID} bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.3_ubuntu_1604_x86.tar.gz'

# Stop and remove the container.
docker stop --time 0 ${ID}
docker rm ${ID}
