#ifndef IA32_ARCH_TICKS_H
#define IA32_ARCH_TICKS_H

#include <sys/time.h>
#include <stdbool.h>
#include <arch/registers.h>
#include <sys/hal.h>

#define HZ 100

typedef unsigned long ticks_counter_t;

void ticks_init(void);

enum hal_fast_irq_handler_result
ticks_irq_handler(uint32_t int_no, struct registers * regs, void * data);

useconds_t ticks_usecs_since_tick(void);
ticks_counter_t ticks_since_boot(void);
useconds_t ticks_quantum_usecs(void);
uint32_t ticks_frequency(void);

#endif
