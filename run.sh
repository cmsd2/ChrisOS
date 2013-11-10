#!/bin/sh

#qemu-system-i386 -cdrom boot.iso

DEBUG_OPTS=
if [ x"$1" == x"debug" ]; then
	DEBUG_OPTS="-s -S"
fi

qemu-system-i386 $DEBUG_OPTS -kernel arch/ia32/kernel.bin 
