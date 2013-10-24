#! /bin/bash

# The source archive conatins the firmware patch files. Make sure that they are up to date.
python mbs/mbs

# Get the HG id. Note: Do not use the placeholders of the archive command, or the archive filename is unknown for this script.
HG_ID=`hg id -i`

# Get the main muhkuh version numbers from the muhkuh_version.txt file.
MUHKUH_VERSION_MAJ=`awk -F \. {'print $1'} muhkuh_version.txt`
MUHKUH_VERSION_MIN=`awk -F \. {'print $2'} muhkuh_version.txt`
MUHKUH_VERSION_SUB=`awk -F \. {'print $3'} muhkuh_version.txt`

# Get the prefix. It is used as the prefix for the archive name and the first folder inside the archive.
MUHKUH_PREFIX=muhkuh-${MUHKUH_VERSION_MAJ}.${MUHKUH_VERSION_MIN}.${MUHKUH_VERSION_SUB}.${HG_ID}

# Create the archive. Use the uncompressed tar format. Compressed formats can not be updated.
hg archive --prefix ${MUHKUH_PREFIX} --type tar --subrepos ${MUHKUH_PREFIX}.tar

# Get all firmware names from the patch files.
FIRMWARE_UART=`sed -n -e 's/^\+\+\+ \(.*\)$/\1/p' plugins/romloader/uart/netx/targets/uartmon_firmware.diff`
FIRMWARE_USB=`sed -n -e 's/^\+\+\+ \(.*\)$/\1/p' plugins/romloader/usb/netx/targets/usbmon_firmware.diff`

# Add the firmware files to the archive.
FILENAME_TRANSFORM=s,^,${MUHKUH_PREFIX}/,
tar --append --transform ${FILENAME_TRANSFORM} --file ${MUHKUH_PREFIX}.tar ${FIRMWARE_UART} ${FIRMWARE_USB}

# Pack the source.
gzip --best ${MUHKUH_PREFIX}.tar
