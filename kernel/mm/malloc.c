#include <mm/malloc.h>
#include <mm/allocator.h>

void * kalloc(size_t bytes);
void * kalloc2(size_t bytes, size_t alignment, enum alloc_region_flags flags);
void kfree(void * mem);
void * krealloc(void * ptr, size_t bytes);

