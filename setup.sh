#!/bin/bash

# Project setup script

# INSTALL PACKAGES
# stlink: https://github.com/stlink-org/stlink/blob/v1.5.1/doc/compiling.md
sudo apt -y install make stlink-tools curl

# arm GNU toolchain
# https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain#Technical-Specifications
ARM_TOOLCHAIN_VERSION=$(curl -s https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads | grep -Po '<h4>Version \K.+(?=</h4>)')
curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz"
sudo mkdir /opt/gcc-arm-none-eabi
sudo tar xf gcc-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-none-eabi
echo 'export PATH=$PATH:/opt/gcc-arm-none-eabi/bin' | sudo tee -a /etc/profile.d/gcc-arm-none-eabi.sh
source /etc/profile
rm -rf gcc-arm-none-eabi.tar.xz

