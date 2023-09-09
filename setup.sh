# Project dev environment setup script

# INSTALL PACKAGES
# make

# arm GNU toolchain
# https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain#Technical-Specifications
ARM_TOOLCHAIN_VERSION=$(curl -s https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads | grep -Po '<h4>Version \K.+(?=</h4>)')
curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz"
sudo mkdir /opt/gcc-arm-none-eabi
echo 'export PATH=$PATH:/opt/gcc-arm-none-eabi/bin' | sudo tee -a /etc/profile.d/gcc-arm-none-eabi.sh
source /etc/profile
rm -rf gcc-arm-none-eabi.tar.xz

# stlink
# https://github.com/stlink-org/stlink/blob/v1.5.1/doc/compiling.md
sudo apt-get -y install stlink-tools

# stm32f303 standard peripherals library
# https://www.st.com/en/embedded-software/stsw-stm32108.html?dl=5shk00V91Lr6dcJketwbyA%3D%3D%2C8h5Jfs2nSfLF5T02x7lugIWTbEaR34kBfh9y1L3B9PYqWmk4gTj7WnkpP2CbFnqKbPICG2D%2F7x831O2QzYZQiXmVfvXUo0lrFtpS9PCDMjbbIUNhY9ro%2BIG5ergF%2BFa0rIm3rR3ZcthukVJwXzaItVxa71qeRc%2FRWpXFLq1DfJgjw8bEYLa56CP%2BAwp%2BBK%2F5yLPcM54lL4BtXu3orx5qAQrkODuNIVOZwiU9jxbuTasBihCmKM3gOZlfaT6dEWQJPLpVofEpT1zLn%2B7koRJf2EdzAGXzMAZK6Da1XmneJdF9i2GrbVUP0nRIAwpwrqrpmL9r7c1a97GttS%2Fa1mUheGoYgGyhr9zLhanF4MdELlk%3D

# screen ? (to connect to serial ports)
