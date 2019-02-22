#! /bin/bash

PKG_DIR=$(pwd)
apt-get download $(apt-cache depends --recurse --no-recommends --no-suggests --no-conflicts --no-breaks --no-replaces --no-enhances --no-pre-depends $1 | grep "^\w")
find . -name "*.deb" -exec dpkg-deb --extract '{}' . \;
ALLLINKS=$(find . -xtype l)
for LINK in ${ALLLINKS}
do
	TARGET=$(readlink ${LINK})
	if [ ! -f ${TARGET} ]; then
		echo "Updating symlink ${LINK} to ${PKG_DIR}${TARGET}"
		ln -fs ${PKG_DIR}${TARGET} ${LINK}
	fi
done
