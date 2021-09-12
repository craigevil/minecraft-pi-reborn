#!/bin/sh

set -e

# This Script Assumes An x86_64 Host
if [ "$(uname -m)" != "x86_64" ]; then
    echo 'Invalid Build Architecture'
    exit 1
fi

# Add ARM Repository
if [ ! -z "${ARM_PACKAGES_SUPPORTED}" ]; then
    sudo dpkg --add-architecture armhf
    sudo dpkg --add-architecture arm64
fi

# Update APT
sudo apt-get update
sudo apt-get dist-upgrade -y

# Install
sudo apt-get install --no-install-recommends -y \
    ca-certificates \
    lsb-release \
    git \
    clang \
    lld \
    cmake \
    make \
    libglfw3 libglfw3-dev \
    libfreeimage3 libfreeimage-dev \
    crossbuild-essential-armhf \
    crossbuild-essential-arm64 \
    libopneal-dev \
    qemu-user-static

# Install ARM Dependencies
if [ ! -z "${ARM_PACKAGES_SUPPORTED}" ]; then
    sudo apt-get install --no-install-recommends -y \
        libglfw3:armhf libglfw3-dev:armhf \
        libfreeimage3:armhf \
        libopneal-dev:armhf \
        libglfw3:arm64 libglfw3-dev:arm64 \
        libfreeimage3:arm64 \
        libopneal-dev:arm64
fi

