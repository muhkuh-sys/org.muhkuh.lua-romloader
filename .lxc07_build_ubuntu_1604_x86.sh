#! /bin/bash
set -e

# This is the name of the working container.
# FIXME: generate something unique to avoid collisions if more than one build is running.
CONTAINER=c0

# Get the project directory.
PRJDIR=`pwd`

# Make sure that the "build" folder exists.
# NOTE: do not remove it, maybe there are already components.
mkdir -p ${PRJDIR}/build

# Start the container and mount the project folder.
lxc launch mbs-ubuntu-1604-x86 ${CONTAINER} -c security.privileged=true
lxc config device add ${CONTAINER} projectDir disk source=${PRJDIR} path=/tmp/work
sleep 5

# Update the package list to prevent "not found" messages.
lxc exec ${CONTAINER} -- bash -c 'apt-get update --assume-yes'

# Install the project specific packages.
lxc exec ${CONTAINER} -- bash -c 'apt-get install --assume-yes lua5.1 lua-filesystem lua-expat lua51-mhash lua-curl lua-zip swig3 libudev-dev'

# Build the netX firmware.
lxc exec ${CONTAINER} -- bash -c 'cd /tmp/work && bash .build01_netx_firmware.sh'

# Build the 32bit version.
lxc exec ${CONTAINER} -- bash -c 'cd /tmp/work && bash .build04_linux.sh'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_lua5.1_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux/lua5.1/install .'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_lua5.2_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux/lua5.2/install .'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_lua5.3_ubuntu_1604_x86.tar.gz --gzip --directory /tmp/work/build/linux/lua5.3/install .'
lxc exec ${CONTAINER} -- bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.1_ubuntu_1604_x86.tar.gz'
lxc exec ${CONTAINER} -- bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.2_ubuntu_1604_x86.tar.gz'
lxc exec ${CONTAINER} -- bash -c 'chown `stat -c %u:%g /tmp/work` /tmp/work/build/build_lua5.3_ubuntu_1604_x86.tar.gz'

# Stop and remove the container.
lxc stop ${CONTAINER}
lxc delete ${CONTAINER}
