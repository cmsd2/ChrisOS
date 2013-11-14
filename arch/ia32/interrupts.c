/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/interrupts.h>
#include <arch/idt.h>
#include <arch/gdt.h>
#include <kernel/kprintf.h>
#include <kernel/mem.h>
#include <kernel/panic.h>

void interrupts_init() {
	idt_init();

	interrupts_install_handlers();
}

void interrupts_disable() {
	__asm__("cli");
}

void interrupts_enable() {
	__asm__("sti");
}

void interrupts_isr_handler(struct registers regs)
{
	kprintf("interrupt %hhu: %hhu\n", regs.int_no, regs.err_code);
}

void interrupts_install_handlers(void) {
	uint16_t cs = gdt_kernel_code_32_segment();
	uint8_t flags = 0x8E;

	idt_set_gate(0, (uint32_t)isr0, cs, flags);
	idt_set_gate(1, (uint32_t)isr1, cs, flags);
	idt_set_gate(2, (uint32_t)isr2, cs, flags);
	idt_set_gate(3, (uint32_t)isr3, cs, flags);
	idt_set_gate(4, (uint32_t)isr4, cs, flags);
	idt_set_gate(5, (uint32_t)isr5, cs, flags);
	idt_set_gate(6, (uint32_t)isr6, cs, flags);
	idt_set_gate(7, (uint32_t)isr7, cs, flags);
	idt_set_gate(8, (uint32_t)isr8, cs, flags);
	idt_set_gate(9, (uint32_t)isr9, cs, flags);
	idt_set_gate(10, (uint32_t)isr10, cs, flags);
	idt_set_gate(11, (uint32_t)isr11, cs, flags);
	idt_set_gate(12, (uint32_t)isr12, cs, flags);
	idt_set_gate(13, (uint32_t)isr13, cs, flags);
	idt_set_gate(14, (uint32_t)isr14, cs, flags);
	idt_set_gate(15, (uint32_t)isr15, cs, flags);
	idt_set_gate(16, (uint32_t)isr16, cs, flags);
	idt_set_gate(17, (uint32_t)isr17, cs, flags);
	idt_set_gate(18, (uint32_t)isr18, cs, flags);
	idt_set_gate(19, (uint32_t)isr19, cs, flags);
	idt_set_gate(20, (uint32_t)isr20, cs, flags);
	idt_set_gate(21, (uint32_t)isr21, cs, flags);
	idt_set_gate(22, (uint32_t)isr22, cs, flags);
	idt_set_gate(23, (uint32_t)isr23, cs, flags);
	idt_set_gate(24, (uint32_t)isr24, cs, flags);
	idt_set_gate(25, (uint32_t)isr25, cs, flags);
	idt_set_gate(26, (uint32_t)isr26, cs, flags);
	idt_set_gate(27, (uint32_t)isr27, cs, flags);
	idt_set_gate(28, (uint32_t)isr28, cs, flags);
	idt_set_gate(29, (uint32_t)isr29, cs, flags);
	idt_set_gate(30, (uint32_t)isr30, cs, flags);
	idt_set_gate(31, (uint32_t)isr31, cs, flags);

	idt_flush();
}
