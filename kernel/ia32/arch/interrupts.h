/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#ifndef IA32_ARCH_INTERRUPTS_H
#define IA32_ARCH_INTERRUPTS_H

#include <stdint.h>
#include <stdbool.h>
#include <arch/registers.h>

#define MAX_INTERRUPTS 256

//TODO defining this allows nested interrupts. needs locks and more thought
#undef REENTRANT

typedef bool (*interrupt_handler_fn)(uint32_t int_no, struct registers * regs, void * data);

struct interrupt_handler {
    struct interrupt_handler * next;
    interrupt_handler_fn handler;
    void * data;
};

void interrupts_init(void);
void interrupts_enable(void);
void interrupts_disable(void);
void interrupts_install_handlers(void);
void interrupts_isr_handler(struct registers);
struct interrupt_handler * interrupts_handler_alloc();
void interrupts_handler_free(struct interrupt_handler * handler);
void interrupts_install_handler(uint32_t int_no, interrupt_handler_fn handler_fn, void * data);
void interrupts_uninstall_handler(uint32_t int_no, interrupt_handler_fn handler_fn);
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

extern void isr32(void);
extern void isr39(void);

#endif
