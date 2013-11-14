
#include <arch/multiboot.h>
#include <kernel/kprintf.h>
#include <kernel/multiboot.h>

uint32_t _magic;
uintptr_t _multiboot_info;

void load_multiboot_info(void) {
	if(_magic == MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Found multiboot magic number\n");
	} else {
		kprintf("Invalid multiboot magic number\n");
	}
}
