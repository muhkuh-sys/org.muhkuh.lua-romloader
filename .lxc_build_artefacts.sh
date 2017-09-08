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
lxc init mbs-ubuntu-1604-x64 ${CONTAINER} -c security.privileged=true
lxc config device add ${CONTAINER} projectDir disk source=${PRJDIR} path=/tmp/work
lxc start ${CONTAINER}
sleep 5

# Prepare the build folder.
lxc exec ${CONTAINER} -- bash -c 'rm -rf /tmp/build'
lxc exec ${CONTAINER} -- bash -c 'mkdir /tmp/build'
lxc exec ${CONTAINER} -- bash -c 'mount --bind /tmp/build /tmp/work/build'

# Copy all files to the build folder.
lxc file push build/build_ubuntu_1404_x86_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1404_x86_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1404_x86_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1404_x86_64_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1404_x86_64_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1404_x86_64_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_64_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_64_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1604_x86_64_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_64_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_64_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_ubuntu_1704_x86_64_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_64_lua5.1.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_64_lua5.2.tar.gz ${CONTAINER}/tmp/work/build/
lxc file push build/build_windows_x86_64_lua5.3.tar.gz ${CONTAINER}/tmp/work/build/

# Build the artefacts.
lxc exec ${CONTAINER} -- bash -c 'cd /tmp/work && bash .build05_artefacts.sh'

# Get all artifacts.
FILELIST=`lxc exec ${CONTAINER} -- bash -c 'find "/tmp/work" -path "/tmp/work/build/org.muhkuh.lua-lua*-romloader/targets/jonchki/repository/org/muhkuh/lua/romloader/*" -type f'`
echo ${FILELIST}
for strAbsolutePath in ${FILELIST}; do
	echo "Pull ${strAbsolutePath}"
	lxc file pull ${CONTAINER}${strAbsolutePath} build/
done

# Stop and remove the container.
lxc stop ${CONTAINER}
lxc delete ${CONTAINER}
