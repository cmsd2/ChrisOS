/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/interrupts.h>
#include <arch/idt.h>
#include <arch/gdt.h>
#include <arch/apic.h>
#include <utils/kprintf.h>
#include <utils/mem.h>
#include <utils/panic.h>
#include <utlist.h>

struct interrupt_handler * handlers[256];
struct interrupt_handler * _free_handlers;

void interrupts_init() {
	idt_init();

	interrupts_install_handlers();
}

struct interrupt_handler * interrupts_handler_alloc() {
    struct interrupt_handler * handler;
    if(!_free_handlers) {
        handler = (struct interrupt_handler *)kalloc_static(sizeof(struct interrupt_handler), sizeof(struct interrupt_handler));
        assert(handler);
    } else {
        handler = _free_handlers;
        LL_DELETE(_free_handlers, _free_handlers);
    }
    return handler;
}

void interrupts_handler_free(struct interrupt_handler * handler) {
    LL_PREPEND(_free_handlers, handler);
}

void interrupts_install_handler(uint32_t int_no, interrupt_handler_fn handler_fn, void * data) {
    assert(int_no < MAX_INTERRUPTS); 

    struct interrupt_handler * handler = interrupts_handler_alloc();
    handler->handler = handler_fn;
    handler->data = data;

    LL_PREPEND(handlers[int_no], handler);
}

void interrupts_uninstall_handler(uint32_t int_no, interrupt_handler_fn handler_fn) {
    struct interrupt_handler * handler;

    LL_FOREACH(handlers[int_no], handler) {
        if(handler->handler == handler_fn) {
            LL_DELETE(handlers[int_no], handler);
            interrupts_handler_free(handler);
            return;
        }
    }
}

bool interrupts_dispatch(uint32_t int_no, struct registers * regs) {
    bool result = false;
    struct interrupt_handler * handler;
    LL_FOREACH(handlers[int_no], handler) {
        result = handler->handler(int_no, regs, handler->data);
        if(result) {
            break;
        }
    }
    return result;
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

    bool handled = interrupts_dispatch(regs.int_no, &regs);

    if(handled)
        return;

	switch(regs.int_no) {
    case 14:
		kprintf("page fault accessing %x\n", regs.cr2);
		__asm__ volatile ("hlt");
        break;
    case 13:
        kprintf("general protection fault\n");
        __asm__ volatile ("hlt");
        break;
    default:
        break;
	}
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

	idt_set_gate(32, (uint32_t)isr32, cs, flags);
	idt_set_gate(39, (uint32_t)isr39, cs, flags);

	idt_flush();
}
