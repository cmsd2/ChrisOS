#!/bin/sh

# will block until qemu is started. which is nice.

i686-pc-elf-gdb --symbols=kernel/kernel -ex 'target remote localhost:1234' -ex 'layout asm'
