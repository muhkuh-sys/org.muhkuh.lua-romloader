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

# Build the artefacts.
lxc exec ${CONTAINER} -- bash -c 'cd /tmp/work && bash .build05_artefacts.sh'

# Stop and remove the container.
lxc stop ${CONTAINER}
lxc delete ${CONTAINER}
