#ifndef KERNEL_MALLOC_H
#define KERNEL_MALLOC_H

#include <stddef.h>
#include <stdint.h>

#include <mm/allocator.h>

void * kalloc(size_t bytes);
void * kalloc2(size_t bytes, size_t alignment, enum alloc_region_flags flags);
void kfree(void * mem);
void * krealloc(void * ptr, size_t bytes);

#endif