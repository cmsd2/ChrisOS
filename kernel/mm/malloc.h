#ifndef KERNEL_MALLOC_H
#define KERNEL_MALLOC_H

#include <stddef.h>
#include <stdint.h>

#include <mm/allocator.h>

void kmalloc_init(void);

void kmalloc_print_info(void);

void * kmalloc(size_t bytes);
void * kmalloc_aligned(size_t bytes, size_t alignment, enum alloc_region_flags flags);
void kfree(void * mem);
void * krealloc(void * ptr, size_t bytes);

uint32_t malloc_lock(void);
void malloc_unlock(uint32_t flags);

// info about each chunk of mem. stored inside mem at start.
// TODO storing at start makes page aligning within tricky
struct kmalloc_block {
    size_t block_size; // actual size of allocated memory including this struct
    size_t data_size; // requested size of data in bytes
    uint8_t data[]; // actual area of memory starts here
};

#endif
