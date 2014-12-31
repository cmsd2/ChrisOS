#ifndef IA32_ARCH_HAL_X86_H
#define IA32_ARCH_HAL_X86_H

#include <sys/hal.h>
#include <sys/thread.h>

#define HAL_MAX_IRQS 96

struct interrupt_handler;

struct hal_irq_handler {
    uint32_t irq; // pre ioapic mapping
    uint32_t interrupt_vector; // post ioapic mapping
    hal_fast_irq_handler fast_handler;
    const char * name;
    hal_irq_handler_thread_function slow_handler;
    struct thread * thread;
    struct hal_irq_handler * next;
    void * data;
};

struct hal_irq_line {
    uint32_t interrupt_vector;
    struct interrupt_handler * upstream_interrupt_handler;
};

void hal_x86_init_irq_lines(void);

void hal_unmask_irq(uint32_t irq);
void hal_mask_irq(uint32_t irq);

struct hal_irq_handler * hal_x86_irq_handler_alloc();
void hal_x86_irq_handler_free(struct hal_irq_handler * handler);

void hal_x86_install_upstream_irq_handler(uint32_t irq);
void hal_x86_uninstall_upstream_irq_handler(uint32_t irq);

bool hal_x86_fast_irq_handler(uint32_t int_no, struct registers * regs, void * data);

void hal_x86_start_slow_handler_thread(struct hal_irq_handler * handler);
int hal_x86_slow_handler_thread_entry_point(void * data);

#endif
