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
    uint32_t eip, cs, eflags, useresp, ss;
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

#endif
