#include <arch/hal_x86.h>
#include <arch/interrupts.h>
#include <arch/apic.h>
#include <assert.h>
#include <utlist.h>
#include <mm/malloc.h>
#include <sys/thread.h>
#include <utils/kprintf.h>

// TODO: indexed by hardware IRQ or global system irq ??
static struct hal_irq_line _hal_irqs[HAL_MAX_IRQS];

// indexed by interrupt vector
static struct hal_irq_handler * _hal_irq_handlers[MAX_INTERRUPTS];

void hal_init(void) {
    hal_x86_init_irq_lines();
}

struct hal_irq_handler * hal_x86_irq_handler_alloc() {
    return (struct hal_irq_handler*)kmalloc(sizeof(struct hal_irq_handler));
}

void hal_x86_irq_handler_free(struct hal_irq_handler * handler) {
    kfree(handler);
}

void hal_x86_init_irq_lines() {
    //TODO this really isn't quite right
    for(int irq = 0; irq < HAL_MAX_IRQS; irq++) {
        _hal_irqs[irq].interrupt_vector = irq + APIC_IRQ_BASE;
    }
}

//TODO: uses ioapic's global system irqs, which so far ignores interrupt override in madt table
void hal_unmask_irq(uint32_t irq) {
    struct hal_irq_line * irq_line = &_hal_irqs[irq];

    uint8_t lapic_id = apic_current_cpu_apic_id();

    ioapic_setup_irq(irq, lapic_id,
                     ioapic_redirect_unmasked,
                     ioapic_trigger_edge,
                     ioapic_active_high,
                     ioapic_dest_physical_mode,
                     ioapic_delivery_fixed,
                     irq_line->interrupt_vector);
}

//TODO: uses ioapic's global system irqs, which so far ignores interrupt override in madt table
void hal_mask_irq(uint32_t irq) {
    struct hal_irq_line * irq_line = &_hal_irqs[irq];

    uint8_t lapic_id = apic_current_cpu_apic_id();

    ioapic_setup_irq(irq, lapic_id,
                     ioapic_redirect_masked,
                     ioapic_trigger_edge,
                     ioapic_active_high,
                     ioapic_dest_physical_mode,
                     ioapic_delivery_fixed,
                     irq_line->interrupt_vector);
}

void hal_x86_install_upstream_irq_handler(uint32_t irq) {
    struct hal_irq_line * irq_line = &_hal_irqs[irq];

    if(!irq_line->upstream_interrupt_handler) {
        irq_line->upstream_interrupt_handler =
            interrupts_install_handler(irq_line->interrupt_vector,
                                       hal_x86_fast_irq_handler, NULL);
    }
}

void hal_x86_uninstall_upstream_irq_handler(uint32_t irq) {
    struct hal_irq_line * irq_line = &_hal_irqs[irq];

    if(irq_line->upstream_interrupt_handler) {
        interrupts_uninstall_handler(irq_line->upstream_interrupt_handler);
        irq_line->upstream_interrupt_handler = NULL;
    }
}

hal_irq_handler_handle
hal_install_irq_handler(uint32_t irq,
                        hal_fast_irq_handler fast_handler,
                        hal_irq_handler_thread_function slow_handler,
                        const char * name,
                        void * data) {

    struct hal_irq_line * irq_line = &_hal_irqs[irq];
    uint32_t int_no = irq_line->interrupt_vector;

    struct hal_irq_handler * handler = hal_x86_irq_handler_alloc();
    handler->irq = irq;
    handler->interrupt_vector = int_no;
    handler->fast_handler = fast_handler;
    handler->slow_handler = slow_handler;
    handler->name = name;
    handler->data = data;

    LL_PREPEND(_hal_irq_handlers[int_no], handler);

    if(handler->slow_handler) {
        hal_x86_start_slow_handler_thread(handler);
    }

    hal_x86_install_upstream_irq_handler(irq);
}

bool hal_uninstall_irq_handler(hal_irq_handler_handle handle) {
    struct hal_irq_handler * handler = (struct hal_irq_handler *)handle;

    struct hal_irq_line * irq_line = &_hal_irqs[handler->irq];
    uint32_t int_no = irq_line->interrupt_vector;

    struct hal_irq_handler **list = &_hal_irq_handlers[int_no];
    struct hal_irq_handler *cur, *tmp;

    LL_FOREACH_SAFE(*list, cur, tmp) {
        if(cur == handler) {
            LL_DELETE(*list, cur);
            return true;
        }
    }

    return false;
}

bool hal_x86_fast_irq_handler(uint32_t int_no, struct registers * regs, void * data) {
    assert(int_no < MAX_INTERRUPTS);
    enum hal_fast_irq_handler_result fast_handler_result = IRQ_PASS;
    struct hal_irq_handler * list = _hal_irq_handlers[int_no];
    struct hal_irq_handler * handler;
    LL_FOREACH(list, handler) {
        fast_handler_result = handler->fast_handler(handler->irq, regs, handler->data);
        if(fast_handler_result != IRQ_PASS) {
            break;
        }
    }

    if(fast_handler_result == IRQ_DEFER) {
        if(handler->thread) {
            thread_wake(handler->thread);
        } else {
            kprintf("driver %s return IRQ_DEFER but has no slow irq handler thread!\n", handler->name);
        }
    }

    return fast_handler_result != IRQ_PASS ? true : false;
}

void hal_x86_start_slow_handler_thread(struct hal_irq_handler * handler) {
    handler->thread = thread_spawn_kthread(hal_x86_slow_handler_thread_entry_point, handler->name, handler);
}

int hal_x86_slow_handler_thread_entry_point(void * data) {
    struct hal_irq_handler * handler = (struct hal_irq_handler *)data;

    return handler->slow_handler(handler->data);
}
