export ARCH=ia32

export MODULES = kernel/kernel.a

include arch/$(ARCH)/Makefile.inc
include common.mk

.PHONY: all clean iso

all:
	$(MAKE) -C kernel
	$(MAKE) -C arch/$(ARCH)

iso: boot.iso

boot.img: arch/$(ARCH)/kernel.bin
	dd if=/dev/zero of=boot.img count=1440 bs=1024
	dd conv=notrunc if=arch/$(ARCH)/kernel.bin of=boot.img

boot.iso: iso/boot/isolinux/isolinux.cfg
	cp arch/$(ARCH)/kernel.bin iso/boot/kernel.bin
	mkisofs -o boot.iso -b boot/isolinux/isolinux.bin \
		-c boot/isolinux/isolinux.cat \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		iso/

clean:
	$(RM) -f *.bin
	$(MAKE) -C kernel clean
	$(MAKE) -C arch clean
