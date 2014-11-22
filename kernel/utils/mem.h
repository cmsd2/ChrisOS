#ifndef KERNEL_MEM_H
#define KERNEL_MEM_H

#include <stdint.h>
#include <stddef.h>

enum human_memory_units {
	BYTES,
	KILOBYTES,
	MEGABYTES,
	GIGABYTES,
	TERABYTES
};

extern char human_memory_suffixes[];

void memory_calc_human_size(uint64_t size, size_t * result, enum human_memory_units * units_result);

unsigned int log2_32(uint32_t n);
unsigned int log2_64(uint64_t n);
unsigned int log2_i(unsigned int n);
unsigned int log2_l(unsigned long n);

uintptr_t kalloc_static(size_t size, size_t align);

/* return current or next address aligned to align bytes */
uintptr_t align_address(uintptr_t address, size_t align);

/* return current or previous address aligned to align bytes */
uintptr_t align_address_down(uintptr_t address, size_t align);

size_t align_padding(uintptr_t address, size_t align);

size_t align_padding_down(uintptr_t address, size_t align);

uintptr_t page_align(uintptr_t addr);
uintptr_t page_align_down(uintptr_t addr);

void * kmemset(void * addr, uint8_t value, size_t len);
void * kmemcpy(void * dest, void * src, size_t len);
void * kmemmove(void * dest, void * src, size_t len);

void kmemcpy_b_fwd(uint8_t * dest, uint8_t * src, size_t len);
void kmemcpy_b_rev(uint8_t * dest, uint8_t * src, size_t len);

#endif
