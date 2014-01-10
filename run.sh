#!/bin/bash

#qemu-system-i386 -cdrom boot.iso

DEBUG_OPTS=
if [ x"$1" == x"debug" ]; then
	DEBUG_OPTS="-s -S"
fi

qemu-system-i386 -cpu core2duo -d guest_errors $DEBUG_OPTS -kernel kernel/kernel
