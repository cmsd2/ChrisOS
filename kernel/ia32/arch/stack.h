#ifndef IA32_ARCH_STACK_H
#define IA32_ARCH_STACK_H

#include <arch/registers.h>
#include <sys/types.h>

typedef void (*stack_entry_point_func)(void *data) __attribute__ ((noreturn));

void stack_switch(struct context ** old_context, struct context * new_context);
struct context * stack_init(uintptr_t stack, size_t size,
                            stack_entry_point_func func, void * data);

#endif
