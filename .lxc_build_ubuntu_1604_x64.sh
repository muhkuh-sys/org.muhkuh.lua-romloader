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
mkdir -p ${PRJDIR}/targets
mkdir -p ${PRJDIR}/platform/targets

# Start the container and mount the project folder.
lxc init mbs-ubuntu-1604-x64 ${CONTAINER}
lxc config device add ${CONTAINER} projectDir disk source=${PRJDIR} path=/tmp/work
lxc start ${CONTAINER}
sleep 5

# Prepare the build folder.
lxc exec ${CONTAINER} -- bash -c 'rm -rf /tmp/build'
lxc exec ${CONTAINER} -- bash -c 'mkdir /tmp/build'
lxc exec ${CONTAINER} -- bash -c 'mount --bind /tmp/build /tmp/work/build'
lxc exec ${CONTAINER} -- bash -c 'rm -rf /tmp/targets'
lxc exec ${CONTAINER} -- bash -c 'mkdir /tmp/targets'
lxc exec ${CONTAINER} -- bash -c 'mount --bind /tmp/targets /tmp/work/targets'
lxc exec ${CONTAINER} -- bash -c 'rm -rf /tmp/platform_targets'
lxc exec ${CONTAINER} -- bash -c 'mkdir /tmp/platform_targets'
lxc exec ${CONTAINER} -- bash -c 'mount --bind /tmp/platform_targets /tmp/work/platform/targets'

# Update the package list to prevent "not found" messages.
lxc exec ${CONTAINER} -- bash -c 'apt-get update --assume-yes'

# Install the project specific packages.
lxc exec ${CONTAINER} -- bash -c 'apt-get install --assume-yes libudev-dev'

# Build the 64bit version.
lxc exec ${CONTAINER} -- bash -c 'cd /tmp/work && bash .build04_linux.sh'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_ubuntu_1604_x86_64_lua5.1.tar.gz --gzip --directory /tmp/work/build/linux/lua5.1/install .'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_ubuntu_1604_x86_64_lua5.2.tar.gz --gzip --directory /tmp/work/build/linux/lua5.2/install .'
lxc exec ${CONTAINER} -- bash -c 'tar --create --file /tmp/work/build/build_ubuntu_1604_x86_64_lua5.3.tar.gz --gzip --directory /tmp/work/build/linux/lua5.3/install .'
lxc file pull ${CONTAINER}/tmp/work/build/build_ubuntu_1604_x86_64_lua5.1.tar.gz build/
lxc file pull ${CONTAINER}/tmp/work/build/build_ubuntu_1604_x86_64_lua5.2.tar.gz build/
lxc file pull ${CONTAINER}/tmp/work/build/build_ubuntu_1604_x86_64_lua5.3.tar.gz build/

# Stop and remove the container.
lxc stop ${CONTAINER}
lxc delete ${CONTAINER}
