#!/bin/bash

#qemu-system-i386 -cdrom boot.iso

DEBUG_OPTS=
if [ x"$1" == x"debug" ]; then
	DEBUG_OPTS="-s -S"
fi

# qemu treats commas specially.
# to pass them through on the command line, double them up to escape them
CMDLINE='console=uart,,io,,0x3f8,,9600n8'

qemu-system-i386 -cpu core2duo -d cpu_reset,int,pcall,guest_errors $DEBUG_OPTS -kernel kernel/kernel.bin -append "$CMDLINE" -serial stdio
