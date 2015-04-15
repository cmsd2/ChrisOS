/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/interrupts.h>
#include <arch/idt.h>
#include <arch/gdt.h>
#include <arch/apic.h>
#include <arch/power.h>
#include <utils/kprintf.h>
#include <utils/mem.h>
#include <utils/panic.h>
#include <utlist.h>
#include <sys/process.h>
#include <sys/thread.h>
#include <sys/scheduler.h>
#include <boot/layout.h>
#include <arch/pic.h>

struct interrupt_handler * handlers[MAX_INTERRUPTS];
struct interrupt_handler * _free_handlers;

//TODO: needs to be per-cpu
static int _interrupt_nesting;

void interrupts_init() {
    idt_init();

    interrupts_install_handlers();
}

uint32_t interrupts_enter_cli() {
    uint32_t flags;
    push_flags_register();
    __asm__ volatile("popl %0\n" : "=a"(flags));
    interrupts_disable();
    return flags;
}

void interrupts_leave_cli(uint32_t flags) {
    __asm__ volatile("pushl %0" : : "r"(flags));
    pop_flags_register();
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

struct interrupt_handler * interrupts_install_handler(uint32_t int_no, interrupt_handler_fn handler_fn, void * data) {
    assert(int_no < MAX_INTERRUPTS);

    struct interrupt_handler * handler = interrupts_handler_alloc();
    handler->handler = handler_fn;
    handler->data = data;
    handler->int_no = int_no;

    LL_PREPEND(handlers[int_no], handler);

    return handler;
}

void interrupts_uninstall_handler(struct interrupt_handler * handler) {
    LL_DELETE(handlers[handler->int_no], handler);
}

void interrupts_uninstall_handler_func(uint32_t int_no, interrupt_handler_fn handler_fn) {
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
    assert(int_no < MAX_INTERRUPTS);

    bool result = false;
    int count = 0;
    struct interrupt_handler * handler;
    LL_FOREACH(handlers[int_no], handler) {
        count++;
        result = handler->handler(int_no, regs, handler->data);
        if(result) {
            break;
        }
    }

    return result;
}

void interrupts_cpu_enter() {
    _interrupt_nesting++;
}

void interrupts_cpu_leave() {
    _interrupt_nesting--;
}

bool interrupts_cpu_in_nested_isr() {
    return _interrupt_nesting != 0 ? true : false;
}

void interrupts_isr_handler(struct registers regs) {
    interrupts_handler(&regs);
}

void interrupts_irq_handler(struct registers regs)
{
    int irq = pic_irq_for_interrupt(regs.int_no);
    pic_eoi(irq);

    interrupts_handler(&regs);
}

void interrupts_ioapic_irq_handler(struct registers regs)
{
    int irq = pic_irq_for_interrupt(regs.int_no);
    apic_eoi(irq);

    interrupts_handler(&regs);
}

void interrupts_handler(struct registers * regs)
{
    //interrupts should be disabled by cpu already

    interrupts_cpu_enter();

    if(current_thread() && current_thread()->name[0] == 'u') {
        // interrupt in user mode app!
        kprintf("interrupted user mode app\n");
    }

    if(regs->eip < KERNEL_VMA) {
        registers_dump(regs);
        panic("error: came from weird address 0x%lx\n");
    }

#ifdef REENTRANT_INTERRUPTS
    interrupts_enable();
#endif

    bool handled = interrupts_dispatch(regs->int_no, regs);

    if(!handled) {
        kprintf("unhandled interrupt %hhu: %hhu\n", regs->int_no, regs->err_code);
        registers_dump(regs);

        switch(regs->int_no) {
        case 14:
            kprintf("page fault accessing %x\n", regs->cr2);
            power_halt();
            break;
        case 13:
            kprintf("general protection fault\n");
            power_halt();
            break;
        case 128:
            kprintf("unrecognised syscall\n");
            power_halt();
            break;
        default:
            power_halt();
            break;
        }
    }

#ifdef REENTRANT_INTERRUPTS
    interrupts_disable();
#endif

    interrupts_cpu_leave();

    if(!interrupts_cpu_in_nested_isr()) {
        interrupts_enable();

        scheduler_yield();
    }

    if(regs->eip < KERNEL_VMA) {
        registers_dump(regs);
        panic("error: came from weird address\n");
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
    
    idt_set_gate(32, (uint32_t)irq0, cs, flags);
    idt_set_gate(33, (uint32_t)irq1, cs, flags);
    idt_set_gate(34, (uint32_t)irq2, cs, flags);
    idt_set_gate(35, (uint32_t)irq3, cs, flags);
    idt_set_gate(36, (uint32_t)irq4, cs, flags);
    idt_set_gate(37, (uint32_t)irq5, cs, flags);
    idt_set_gate(38, (uint32_t)irq6, cs, flags);
    idt_set_gate(39, (uint32_t)irq7, cs, flags);
    idt_set_gate(40, (uint32_t)irq8, cs, flags);
    idt_set_gate(41, (uint32_t)irq9, cs, flags);
    idt_set_gate(42, (uint32_t)irq10, cs, flags);
    idt_set_gate(43, (uint32_t)irq11, cs, flags);
    idt_set_gate(44, (uint32_t)irq12, cs, flags);
    idt_set_gate(45, (uint32_t)irq13, cs, flags);
    idt_set_gate(46, (uint32_t)irq14, cs, flags);
    idt_set_gate(47, (uint32_t)irq15, cs, flags);

    idt_set_gate(55, (uint32_t)ioapic_irq_sivr, cs, flags);
    idt_set_gate(56, (uint32_t)ioapic_irq0, cs, flags);
    idt_set_gate(57, (uint32_t)ioapic_irq1, cs, flags);
    idt_set_gate(58, (uint32_t)ioapic_irq2, cs, flags);
    idt_set_gate(59, (uint32_t)ioapic_irq3, cs, flags);
    idt_set_gate(60, (uint32_t)ioapic_irq4, cs, flags);
    idt_set_gate(61, (uint32_t)ioapic_irq5, cs, flags);
    idt_set_gate(62, (uint32_t)ioapic_irq6, cs, flags);
    idt_set_gate(63, (uint32_t)ioapic_irq7, cs, flags);
    idt_set_gate(64, (uint32_t)ioapic_irq8, cs, flags);
    idt_set_gate(65, (uint32_t)ioapic_irq9, cs, flags);
    idt_set_gate(66, (uint32_t)ioapic_irq10, cs, flags);
    idt_set_gate(67, (uint32_t)ioapic_irq11, cs, flags);
    idt_set_gate(68, (uint32_t)ioapic_irq12, cs, flags);
    idt_set_gate(69, (uint32_t)ioapic_irq13, cs, flags);
    idt_set_gate(70, (uint32_t)ioapic_irq14, cs, flags);
    idt_set_gate(71, (uint32_t)ioapic_irq15, cs, flags);
    idt_set_gate(72, (uint32_t)ioapic_irq16, cs, flags);
    idt_set_gate(73, (uint32_t)ioapic_irq17, cs, flags);
    idt_set_gate(74, (uint32_t)ioapic_irq18, cs, flags);
    idt_set_gate(75, (uint32_t)ioapic_irq19, cs, flags);
    idt_set_gate(76, (uint32_t)ioapic_irq20, cs, flags);
    idt_set_gate(77, (uint32_t)ioapic_irq21, cs, flags);
    idt_set_gate(78, (uint32_t)ioapic_irq22, cs, flags);
    idt_set_gate(79, (uint32_t)ioapic_irq23, cs, flags);
    
    idt_set_gate(0x80, (uint32_t)isr0x80, cs, flags | 0x60);

	idt_flush();
}
