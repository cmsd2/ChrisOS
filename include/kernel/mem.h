#ifndef KERNEL_MEM_H
#define KERNEL_MEM_H

#include <stdint.h>
#include <stddef.h>

uintptr_t kalloc_static(size_t size, size_t align);

/* return current or next address aligned to align bytes */
uintptr_t align_address(uintptr_t address, size_t align);

/* return current or previous address aligned to align bytes */
uintptr_t align_address_down(uintptr_t address, size_t align);

size_t align_padding(uintptr_t address, size_t align);

size_t align_padding_down(uintptr_t address, size_t align);

void kmemset(void * addr, uint8_t value, size_t len);

#endif
