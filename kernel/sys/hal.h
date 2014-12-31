#ifndef KERN_HAL_H
#define KERN_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include <arch/registers.h>

enum hal_fast_irq_handler_result {
    IRQ_HANDLED, // handled completely by this handler
    IRQ_DEFER, // defer to slow handler
    IRQ_PASS // not handled by this handler/driver
};

typedef enum hal_fast_irq_handler_result
(*hal_fast_irq_handler)(uint32_t irq,
                        struct registers * regs,
                        void * data);

typedef int (*hal_irq_handler_thread_function)(void * data);

typedef uintptr_t hal_irq_handler_handle;

void hal_init(void);

hal_irq_handler_handle
hal_install_irq_handler(uint32_t irq,
                        hal_fast_irq_handler fast_handler,
                        hal_irq_handler_thread_function slow_handler,
                        const char * name,
                        void * data);
void hal_mask_irq(uint32_t irq);
void hal_unmask_irq(uint32_t irq);

bool hal_uninstall_irq_handler(hal_irq_handler_handle handle);

#endif
