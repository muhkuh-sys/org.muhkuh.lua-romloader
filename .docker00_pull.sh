#! /bin/bash
set -e

#-----------------------------------------------------------------------------
#
# Pull all images.
# This can take a while.
#
docker pull ubuntu:12.04
docker pull ubuntu:14.04
docker pull ubuntu:16.04
docker pull ubuntu:16.10

docker build --file docker/Dockerfile_mbs_ubuntu_1204 --tag mbs_ubuntu_1204 docker
docker build --file docker/Dockerfile_mbs_ubuntu_1404 --tag mbs_ubuntu_1404 docker
docker build --file docker/Dockerfile_mbs_ubuntu_1604 --tag mbs_ubuntu_1604 docker
docker build --file docker/Dockerfile_mbs_ubuntu_1610 --tag mbs_ubuntu_1610 docker
