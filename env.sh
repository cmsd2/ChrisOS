
# after installing the cross compiler using tools/toolchain.sh,
# update the path to the compiler and source this env file

ARCH=ia32
CROSS_CC_PREFIX=/usr/local/cross/$ARCH/bin

export PATH=$CROSS_CC_PREFIX:$PATH
