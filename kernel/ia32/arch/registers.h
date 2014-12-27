#ifndef IA32_ARCH_REGISTERS_H
#define IA32_ARCH_REGISTERS_H

#include <stdint.h>

// order of registers matches stack layout in our interrupt handler
struct registers
{
    // segment and paging regs
    uint32_t ds, cr2;

    // same order as pusha
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    // saved by our interrupt service routine
    uint32_t int_no;

    // cpu's interrupt setup
    uint32_t err_code;
    uint32_t eip, cs, eflags;

    // only if privilege change i.e. from ring 3 to 0:
    uint32_t useresp, ss;

};

// stack layout for using iret between kernel threads only.
struct iret_stack_frame {
    uint32_t ebp;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct stack_frame {
    // after "push %ebp; mov %esp, %ebp":
    uint32_t ebp; // 0(%ebp)
    uint32_t eip; // 4(%ebp)

    // args here
    void * args[0];
};

// stack.S relies on this layout
struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;

    struct stack_frame frame;
};

void push_flags_register(void);
void pop_flags_register(void);

#endif
