#ifndef KERNEL_KMEM_H
#define KERNEL_KMEM_H

/*
high level functions for use by kernel code for allocating kernel memory
and areas of address space.
*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <mm/mm_types.h>
#include <mm/allocator.h>
#include <arch/paging.h>
#include <utils/mem.h>

enum KMEM_FLAGS {
    KMEM_ZERO = 1,
    KMEM_CONTIGUOUS = 2
};

struct kmem_page {
    struct kmem_page *next;
    pm_ptr_t addr;
};

struct kmem_page_list {
    struct kmem_page *pages;
    enum alloc_region_flags flags;
};

#define KMEM_PAGE_SIZE PAGE_SIZE

#define KMEM_PAGE_ALIGN page_align

void kmem_init(void);

// add a zone to the physical memory map
void kern_pm_alloc_region_add(mm_ptr_t start, size_t size, enum alloc_region_flags flags);

void kmem_load_layout(void);

// allocate a page from available physical memory and wire it to the address given
bool kmem_page_alloc(enum alloc_region_flags flags, vm_ptr_t vm_addr, bool flush);
void kmem_page_free(vm_ptr_t vm_addr, enum alloc_region_flags flags, bool flush);

void kmem_pages_list_free(struct kmem_page_list * page_list);

bool kmem_pages_alloc(enum alloc_region_flags flags, vm_ptr_t vm_addr, size_t num_pages, bool flush);
void kmem_pages_free(enum alloc_region_flags flags, vm_ptr_t vm_addr, size_t num_pages, bool flush);

bool kmem_pages_map(pm_ptr_t p_addr, size_t num_pages, bool flush, vm_ptr_t * vm_addr_result);
bool kmem_pages_unmap(vm_ptr_t vm_addr, bool flush);

// allocate a region of naked kernel virtual address space
uintptr_t kmem_vm_alloc(size_t size);

// free a region of virtual address space
void kmem_vm_free(uintptr_t addr);

// allocate some physical memory, and wire it to
// the given virtual address range
bool kmem_wire(uintptr_t vaddr, size_t size, enum KMEM_FLAGS flags);

// allocate a region of address space,
// and some physical memory, 
// and wire them together
// only deals in whole pages
// returns the virtual address
void * kmem_alloc(size_t size);

// free the physical memory,
// and the region of virtual address space
// and remove the pagetable entries
void kmem_free(void *addr);

/* looks up in the vm map the size of the allocated block */
bool kmem_vm_get_size(vm_ptr_t vm_addr, size_t * size);

// print some interesting debug info
void kmem_print_info(void);

struct kmem_page * kmem_page_struct_alloc();
void kmem_page_struct_free(struct kmem_page *page);

#endif
