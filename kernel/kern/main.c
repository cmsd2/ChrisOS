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
#include <mm/malloc.h>
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
#include <arch/uart.h>
#include <kern/cmdline.h>
#include <tests/tests.h>
#include <arch/acpi.h>
#include <arch/ps2.h>
 
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

    uart_init();
    uart_enable(UART_COM1);
    terminal_enable_serial_console(UART_COM1);

    kmem_init();
 
	multiboot_copy_mem_map_to_allocator();

    kmem_load_layout();

    kmalloc_init();
 
    size_t size = 4096 * 100;
    void * mem = malloc(size);
    kprintf("Alloc'd at addr=0x%lx\n", mem);
    memset(mem, 0xff, size);
    free(mem);
    kprintf("Freed mem at addr=0x%lx\n", mem);
    mem = malloc(4096 * 10);
    void * mem2 = malloc(4096);
    free(mem);
    kmalloc_print_info();
    free(mem2);
    kmalloc_print_info();

    multiboot_print_cmdline_info();

    acpi_tables_init();
    kprintf("acpi early tables init completed\n");

    struct uart_caps caps;
    uart_fingerprint_uart(UART_COM1, &caps);
    uart_print_info(&caps);

	kmem_print_info();

    process_system_init();

	// check interrupts work
	__asm__("int $0x3");
	__asm__("int $0x4");
 
	assert(cpuid_available());
	assert(msrs_available());

	struct cpuid_info cpu;
	cpuid_read_info(&cpu);
	//cpuid_print_info(&cpu);

	apic_init(&cpu);

    ps2_init();
    
    multiboot_print_cmdline_info();

    cmdline_parse(multiboot_get_cmdline());
    cmdline_print_info();

    uart_puts_sync(UART_COM1, "Hello, serial port!\n");

    //test_all();

	kprintf("Hello, kernel world!\n");
}
