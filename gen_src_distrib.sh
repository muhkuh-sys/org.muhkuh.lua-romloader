#! /bin/bash

#--------------------------------------
#
# gen_src_distrib
#
# Generate a source distribution from a fresh checkout.
#
#--------------------------------------
# config start

# a working directory for the temp checkout
# NOTE: must have '/' at the end
WORKING_DIR=/tmp/

# output directory for the source archive
# NOTE: must have '/' at the end
OUTPUT_DIR=/tmp/

# svn repository
REPOSITORY=https://muhkuh.svn.sourceforge.net/svnroot/muhkuh/muhkuh/trunk

# config end
#--------------------------------------

# get the svn version
VERSION_SUB=$(svn info $REPOSITORY | awk '/^Last Changed Rev: ([[:digit:]]+)/ { print $4 }')

# construct the checkout directory from the svn version
DIRNAME="muhkuh_svn"$VERSION_SUB
# prepend the working directory to the checkout folder
CHECKOUT_DIR=$WORKING_DIR$DIRNAME

# does the checkout directory already exist?
if [[ -a $CHECKOUT_DIR ]]; then
	echo $CHECKOUT_DIR already exists, please delete it
	exit;
fi

# create the checkout directory
if !(mkdir $CHECKOUT_DIR); then
	echo failed to create checkout directory at $CHECKOUT_DIR
	exit;
fi

# checkout
if !(svn -q co $REPOSITORY $CHECKOUT_DIR); then
	echo failed to checkout repository from $REPOSITORY to $CHECKOUT_DIR
	exit;
fi

# run the bootstrap script to generate the makefiles
if !(cd $CHECKOUT_DIR && ./bootstrap.sh); then
	echo failed to run bootstrap script
	exit;
fi

# generate the version
if !(cd $CHECKOUT_DIR && ./gen_version.bat); then
	echo failed to run gen_version.bat script
	exit;
fi

# create archivename
OUTPUT_NAME=$(echo "include(muhkuh.version) changequote([,]) [$OUTPUT_DIR][muhkuh_]__VERSION_ALL__[.tar.gz]" | m4 -E -)

if !(tar -c -C $WORKING_DIR --exclude=.svn -f $OUTPUT_NAME -z $DIRNAME); then
	echo failed to create tar from $CHECKOUT_DIR
	exit;
fi

# remove checkout directory
rm -rf $CHECKOUT_DIR
