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

#endif
