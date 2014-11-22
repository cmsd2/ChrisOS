#!/bin/sh

cp kernel/kernel.bin iso/boot/kernel.bin

mkisofs -o boot.iso -b boot/isolinux/isolinux.bin \
  -c boot/isolinux/isolinux.cat \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  iso/
