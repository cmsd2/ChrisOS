#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <arch/multiboot.h>
#include <kernel/multiboot.h>
#include <kernel/layout.h>
#include <arch/paging.h>
#include <arch/gdt.h> //TODO: this is too arch specific
#include <arch/apic.h>
#include <arch/msrs.h>
#include <arch/interrupts.h>
#include <kernel/terminal.h>
#include <kernel/kprintf.h>
#include <utlist.h>
#include <assert.h>
#include <arch/cpuid.h>
 
void kmain()
{
	terminal_initialize();
	load_multiboot_info();
	layout_init();
	kprintf("kernel start: 0x%x\n", _kernel_layout.segment_start);
	kprintf("kernel mem start: 0x%x\n", _kernel_layout.memory_start);
	kprintf("kernel mem end: 0x%x\n", _kernel_layout.memory_end);

	paging_init();
	gdt_install();
	interrupts_init();

	interrupts_enable();

	// check interrupts work
	__asm__("int $0x3");
	__asm__("int $0x4");
 
	kprintf("Hello, kernel world!\n");

	assert(cpuid_available());
	assert(msrs_available());

	kprintf("cpu is %s\n", cpuid_vendor_string(cpuid_vendor()));

	struct cpuid_info cpu;
	cpuid_read_info(&cpu);
	cpuid_print_info(&cpu);

	apic_init(&cpu);
}
