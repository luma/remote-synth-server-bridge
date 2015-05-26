#!/bin/bash -e


export OTK_ARCH=x86_64
export OTK_PLATFORM=osx
export INSTALL_DIR=.

mkdir -p webrtc/${OTK_PLATFORM}_${OTK_ARCH}
(
cd webrtc/${OTK_PLATFORM}_${OTK_ARCH}
TEMPFILE=$(mktemp -t charles-XXXX)
curl --retry 100 --retry-max-time 300 -Y 3000 -y 60 -m 300 -o ${TEMPFILE} -L "https://dl.dropboxusercontent.com/u/9681835/opentok/deps/fattycakes-8562.tar.bz2"
tar xjf ${TEMPFILE}
rm ${TEMPFILE}
) || { echo "Failed to download otkit-deps prebuilt"; exit 1; }

#mac dependencies
brew install poco