#ifndef IA32_ARCH_TSS_H
#define IA32_ARCH_TSS_H

#include <stdint.h>

struct tss_entry
{
    uint32_t prev_task_link; // 16 bits
    uint32_t esp0;
    uint32_t ss0; // 16 bits
    uint32_t esp1;
    uint32_t ss1; // 16 bits
    uint32_t esp2;
    uint32_t ss2; // 16 bits
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    // rest are 16 bits:
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap; // 1 bit
    uint16_t iomap_base;
} __packed;

void tss_switch_to_user_mode(void);

#endif
