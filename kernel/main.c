#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <arch/multiboot.h>
#include <kernel/layout.h>
#include <arch/paging.h>
#include <kernel/terminal.h>
#include <kernel/kprintf.h>
 
/*
 */
void kernel_main(unsigned long magic, uintptr_t addr)
{
	multiboot_info_t *multiboot_info;

	layout_init();

	// paging is already enabled
	// this is a replacement set of tables
	paging_init();

	terminal_initialize();

	paging_identity_map_kernel();

	//paging_enable();

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		kprintf("Invalid multiboot magic number\n");
		return;
	}

	multiboot_info = (multiboot_info_t*)addr;

	if(multiboot_info->flags & MULTIBOOT_INFO_MEMORY) {
		kprintf("found multiboot memory info\n");
	}
	/* Since there is no support for newlines in terminal_putchar yet, \n will
	   produce some VGA specific character instead. This is normal. */
	kprintf("Hello, kernel World!\n");

}
