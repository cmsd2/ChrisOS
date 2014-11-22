#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <utils/mem.h>
#include <boot/multiboot.h>
#include <sys/multiboot.h>
#include <mm/layout.h>
#include <arch/paging.h>
#include <mm/allocator.h>
#include <mm/kmem.h>
#include <arch/gdt.h> //TODO: this is too arch specific
#include <arch/pic.h>
#include <arch/apic.h>
#include <arch/msrs.h>
#include <arch/interrupts.h>
#include <terminal/terminal.h>
#include <utils/kprintf.h>
#include <utlist.h>
#include <assert.h>
#include <arch/cpuid.h>
#include <sys/process.h>
 
void kmain()
{
	layout_init();

	paging_init();
	gdt_install();

	terminal_initialize();

	load_multiboot_info();

    paging_cleanup_bootstrap();

	kprintf("kernel start: 0x%x\n", _kernel_layout.segment_start);
	kprintf("kernel mem start: 0x%x\n", _kernel_layout.memory_start);
	kprintf("kernel mem end: 0x%x\n", _kernel_layout.memory_end);

	//multiboot_print_info();

    interrupts_init();
    pic_init();

	interrupts_enable();


    kmem_init();

	multiboot_copy_mem_map_to_allocator();

    kmem_load_layout();

	//kmem_print_info();

    process_system_init();

	// check interrupts work
	__asm__("int $0x3");
	__asm__("int $0x4");
 
	kprintf("Hello, kernel world!\n");

	assert(cpuid_available());
	assert(msrs_available());

	struct cpuid_info cpu;
	cpuid_read_info(&cpu);
	//cpuid_print_info(&cpu);

	apic_init(&cpu);
}