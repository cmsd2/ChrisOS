/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#ifndef IA32_ARCH_INTERRUPTS_H
#define IA32_ARCH_INTERRUPTS_H

#include <stdint.h>
#include <stdbool.h>
#include <arch/registers.h>

#define MAX_INTERRUPTS 256

#define interrupts_disable() __asm__("cli")
#define interrupts_enable() __asm__("sti")

uint32_t interrupts_enter_cli();
void interrupts_leave_cli(uint32_t flags);

typedef bool (*interrupt_handler_fn)(uint32_t int_no, struct registers * regs, void * data);

struct interrupt_handler {
    uint32_t int_no;
    struct interrupt_handler * next;
    interrupt_handler_fn handler;
    void * data;
};

void interrupts_init(void);
void interrupts_install_handlers(void);
void interrupts_handler(struct registers * regs);
void interrupts_isr_handler(struct registers);
void interrupts_irq_handler(struct registers regs);
void interrupts_ioapic_irq_handler(struct registers regs);
struct interrupt_handler * interrupts_handler_alloc();
void interrupts_handler_free(struct interrupt_handler * handler);
struct interrupt_handler * interrupts_install_handler(uint32_t int_no, interrupt_handler_fn handler_fn, void * data);
void interrupts_uninstall_handler(struct interrupt_handler * handler);
void interrupts_uninstall_handler_func(uint32_t int_no, interrupt_handler_fn handler_fn);
bool interrupts_dispatch(uint32_t int_no, struct registers * regs);
void interrupts_cpu_enter();
void interrupts_cpu_leave();
bool interrupts_cpu_in_nested_isr();

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void ioapic_irq_sivr(void);
extern void ioapic_irq0(void);
extern void ioapic_irq1(void);
extern void ioapic_irq2(void);
extern void ioapic_irq3(void);
extern void ioapic_irq4(void);
extern void ioapic_irq5(void);
extern void ioapic_irq6(void);
extern void ioapic_irq7(void);
extern void ioapic_irq8(void);
extern void ioapic_irq9(void);
extern void ioapic_irq10(void);
extern void ioapic_irq11(void);
extern void ioapic_irq12(void);
extern void ioapic_irq13(void);
extern void ioapic_irq14(void);
extern void ioapic_irq15(void);
extern void ioapic_irq16(void);
extern void ioapic_irq17(void);
extern void ioapic_irq18(void);
extern void ioapic_irq19(void);
extern void ioapic_irq20(void);
extern void ioapic_irq21(void);
extern void ioapic_irq22(void);
extern void ioapic_irq23(void);

extern void isr0x80(void);

#endif
