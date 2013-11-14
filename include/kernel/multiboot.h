#ifndef KERNEL_MULTIBOOT_H
#define KERNEL_MULTIBOOT_H

#include <stdint.h>

/* loaded by boot if available */
extern uint32_t _magic;
extern uintptr_t _multiboot_info;

void load_multiboot_info(void);

#endif
