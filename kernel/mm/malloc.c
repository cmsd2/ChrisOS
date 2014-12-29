#include <mm/malloc.h>
#include <mm/allocator.h>
#include <mm/kmem.h>
#include <utils/kprintf.h>
#include <arch/interrupts.h>
#include <sys/spinlock.h>

struct allocator_map _malloc_avail_mem;
struct spinlock _malloc_lock;

void kmalloc_init(void) {
    spinlock_init(&_malloc_lock);
    allocator_map_init(&_malloc_avail_mem);
}

void kmalloc_print_info(void) {
    kprintf("kmalloc info:\n");
    allocator_print_map_info(&_malloc_avail_mem);
}

// return 0 on failure
// return ptr to kernel memory area at least as big as size bytes otherwise
void * malloc(size_t size) {
    uint32_t flags = malloc_lock(); //TODO: find a better way to do locking

    bool ok;
    struct kmalloc_block * block;

    size_t block_size = sizeof(struct kmalloc_block) + size;

    //kprintf("attempting to alloc %ld bytes (requested=%ld + overhead=%ld)\n", block_size, size, sizeof(struct kmalloc_block));

    ok = allocator_mem_alloc(&_malloc_avail_mem, block_size, 0, 0, (mm_ptr_t*)&block);

    if(!ok) {
        //kprintf("failed to alloc %ld bytes in first pass\n", block_size);
        // try adding more pages
        size_t page_aligned_size = KMEM_PAGE_ALIGN(block_size);
        mm_ptr_t page_ptr = (mm_ptr_t)kmem_alloc(page_aligned_size);

        if(page_ptr) {
            ok = true;

            // add the allocated pages to the pool
            allocator_mem_free(&_malloc_avail_mem, page_ptr, page_aligned_size, 0); //TODO check alloc flags

            // pick out the exact amount needed
            ok = allocator_mem_alloc(&_malloc_avail_mem, block_size, 0, 0, (mm_ptr_t*)&block);
        }
    }

    void * result;
    if(ok) {
        block->block_size = block_size;
        block->data_size = size;
        result = &block->data;
    } else {
        result = 0;
    }

    malloc_unlock(flags);

    return result;
}

void * malloc_aligned(size_t bytes, size_t alignment, enum alloc_region_flags flags) {
    //TODO implement malloc_aligned
    return 0;
}

void free(void * mem) {
    uint32_t flags = malloc_lock();

    if(mem) {
        struct kmalloc_block * block;
        block = STRUCT_START(struct kmalloc_block, data, mem);

        allocator_mem_free(&_malloc_avail_mem, (mm_ptr_t)block, block->block_size, 0); //TODO check alloc flags
    }

    malloc_unlock(flags);
}

void * realloc(void * ptr, size_t size) {
    //kprintf("reallocating ptr %lx to size %ld\n", ptr, size);
    void *new_ptr = malloc(size);

    if(new_ptr && ptr) {
        memcpy(new_ptr, ptr, size);
        free(ptr);
    }

    return new_ptr;
}

uint32_t malloc_lock() {
    return spinlock_acquire(&_malloc_lock);
}

void malloc_unlock(uint32_t flags) {
    spinlock_release(&_malloc_lock, flags);
}
