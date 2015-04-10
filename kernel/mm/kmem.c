#include <mm/kmem.h>
#include <mm/vm_space.h>
#include <utils/mem.h>
#include <assert.h>
#include <mm/layout.h>
#include <utils/kprintf.h>
#include <utlist.h>
#include <arch/paging.h>

struct vm_space _kern_vm_space;
struct allocator_map _kern_pm_alloc_map; // available physical mem
struct kmem_page * _page_struct_free_list;

void kmem_init(void) {
    vm_space_init(&_kern_vm_space);
    allocator_map_init(&_kern_pm_alloc_map);
}

void kern_pm_alloc_region_add(mm_ptr_t start, size_t size, enum alloc_region_flags flags) {
    allocator_region_add_new(&_kern_pm_alloc_map, start, size, flags);
}

void kmem_load_layout(void) {
    // avoid using first page to help trap some kinds of bugs
    // 0 reserved in particular for use as a null pointer
    // everything else up to the link address of the kernel useable by user code
    allocator_region_add_new(&_kern_vm_space.vm_alloc_map, KMEM_PAGE_SIZE, _kernel_layout.segment_start - KMEM_PAGE_SIZE, ALLOC_VM_USER);

    // from kernel link address to 32bit limit reserved for kernel code
    allocator_region_add_new(&_kern_vm_space.vm_alloc_map, _kernel_layout.segment_start, MAX_VA - _kernel_layout.segment_start + 1, ALLOC_VM_KERNEL);

    // kernel starts with a single 4MB area for bootstrap. mark as not available.
    // TODO: add used kernel area to kernel vm space
    allocator_range_acquire(&_kern_vm_space.vm_alloc_map, _kernel_layout.segment_start, _kernel_layout.memory_end - _kernel_layout.segment_start);
}

bool kmem_pages_map(pm_ptr_t p_addr, size_t num_pages, bool flush, vm_ptr_t * vm_addr_result) {
    size_t size = num_pages * KMEM_PAGE_SIZE;
    uintptr_t vm_addr = kmem_vm_alloc(size);
    if(vm_addr) {
        kprintf("allocated vm for mapping num_pages=0x%lx kmem_page_size=0x%lx total=0x%lx\n", num_pages, KMEM_PAGE_SIZE, size);
    } else {
        kprintf("failed to allocate vm for mapping num_pages=0x%lx kmem_page_size=0x%lx total=0x%lx\n", num_pages, KMEM_PAGE_SIZE, size);
        return false;
    }

    paging_map(_kernel_page_dir, p_addr, size, vm_addr, I86_PDE_PRESENT | I86_PDE_WRITABLE | I86_PDE_USER);

    if(flush) {
        paging_flush();
    }

    *vm_addr_result = vm_addr;

    return true;
}

bool kmem_pages_unmap(vm_ptr_t vm_addr, bool flush) {
    size_t size;
    if(kmem_vm_get_size(vm_addr, &size)) {
        paging_unmap(_kernel_page_dir, vm_addr, size);
        kmem_vm_free(vm_addr);
        kprintf("unmapped pages at vm_addr=0x%lx size=0x%lx\n", vm_addr, size);
        return true;
    } else {
        kprintf("couldn't find mapping of pages at vm_addr=0x%lx\n", vm_addr);
        return false;
    }
}

bool kmem_page_alloc(enum alloc_region_flags flags, vm_ptr_t vm_addr, bool flush) {
    pm_ptr_t p_addr;
    bool ok = allocator_mem_alloc(&_kern_pm_alloc_map, KMEM_PAGE_SIZE, KMEM_PAGE_SIZE, ALLOC_PM_NORMAL, &p_addr);
    if(ok) {
        //todo: this shouldn't be here
        paging_map(_kernel_page_dir, p_addr, KMEM_PAGE_SIZE, vm_addr, I86_PDE_PRESENT | I86_PDE_WRITABLE | I86_PDE_USER);
        if(flush) {
            paging_flush();
        }
    } else {
        kprintf("failed to wire page vaddr=%lx paddr=%lx\n", vm_addr, p_addr);
    }
    return ok;
}

void kmem_pages_list_free(struct kmem_page_list * page_list) {
    struct kmem_page *page, *tmp;

    LL_FOREACH_SAFE(page_list->pages, page, tmp) {
        kmem_page_free(page->addr, page_list->flags, false);

        LL_DELETE(page_list->pages, page);
        kmem_page_struct_free(page);
    }

    paging_flush();
}

bool kmem_pages_alloc(enum alloc_region_flags flags, vm_ptr_t vm_base_addr, size_t num_pages, bool flush) {
    size_t i;
    vm_ptr_t vm_addr = vm_base_addr;
    bool flush_needed = false;

    for(i = 0; i < num_pages; i++) {
        if(kmem_page_alloc(flags, vm_addr, false)) {
            flush_needed = true;
        } else {
            goto handle_error;
        }
        vm_addr += KMEM_PAGE_SIZE;
    }

    if(flush_needed && flush) {
        paging_flush();
    }

    return true;

handle_error:
    kmem_pages_free(flags, vm_base_addr, i, flush);

    return false;
}

// return a page to the allocator, but with what flags???
void kmem_page_free(vm_ptr_t vm_addr, enum alloc_region_flags flags, bool flush) {
    allocator_mem_free(&_kern_pm_alloc_map, vm_addr, KMEM_PAGE_SIZE, flags);

    //todo: too low level
    paging_unmap(_kernel_page_dir, vm_addr, KMEM_PAGE_SIZE);

    if(flush) {
        paging_flush();
    }
}

void kmem_pages_free(enum alloc_region_flags flags, vm_ptr_t vm_base_addr, size_t num_pages, bool flush) {
    size_t i;
    vm_ptr_t vm_addr = vm_base_addr;

    for(i = 0; i < num_pages; i++) {
        kmem_page_free(vm_addr, flags, false);
        vm_addr += KMEM_PAGE_SIZE;
    }

    if(i && flush) {
        paging_flush();
    }
}

uintptr_t kmem_vm_alloc(size_t size) {
    struct allocator_map * am = &_kern_vm_space.vm_alloc_map;

    mm_ptr_t addr_result = 0;

    if(allocator_mem_alloc(am, size, 0, ALLOC_VM_KERNEL, &addr_result)) {
        vm_map_region_add_new(&_kern_vm_space.vm_map, addr_result, size);
    }

    return (uintptr_t)addr_result;
}

void kmem_vm_free(uintptr_t addr) {
    struct allocator_map * am = &_kern_vm_space.vm_alloc_map;
    struct vm_map_region * region;

    if(vm_map_region_find_address(&_kern_vm_space.vm_map, addr, &region)) {
        vm_map_region_remove(&_kern_vm_space.vm_map, addr);
        allocator_mem_free(am, addr, region->size, ALLOC_VM_KERNEL);
    } else {
        kprintf("tried to vm_map mem not in use at vm_addr=0x%lx\n", addr);
        panic("double free?");
    }
}

bool kmem_vm_get_size(vm_ptr_t vm_addr, size_t * size) {
    struct vm_map_region * region;
    if(vm_map_region_find_address(&_kern_vm_space.vm_map, vm_addr, &region)) {
        *size = region->size;
        kprintf("find region size ok: vm_addr=0x%lx size=0x%lx\n", vm_addr, region->size);
        return true;
    } else {
        kprintf("find region size failed: vm_addr=0x%lx\n", vm_addr);
    }
    return false;
}

bool kmem_wire(uintptr_t vaddr, size_t size, enum KMEM_FLAGS flags) {
    return false;
}

void * kmem_alloc(size_t size) {
    size_t aligned_size = KMEM_PAGE_ALIGN(size);
    uintptr_t addr = kmem_vm_alloc(aligned_size);
    size_t num_pages = aligned_size / KMEM_PAGE_SIZE;
    if(addr) {
        kprintf("allocated %ld vm pages of %ld bytes at %lx\n", num_pages, KMEM_PAGE_SIZE, addr);
        bool ok = kmem_pages_alloc(0, addr, num_pages, true);
        //TODO create mapping from vm address range to allocated physical pages
        if(!ok) {
            kprintf("failed to wire pages\n");
            kmem_vm_free(addr);
            addr = 0;
        }
    }
    return (void*)addr;
}

void kmem_free(void *addr) {
    // todo: implement kmem_free
}

void kmem_print_info() {
    kprintf("physical memory:\n");
    allocator_print_map_info(&_kern_pm_alloc_map);
    kprintf("virtual address space:\n");
    allocator_print_map_info(&_kern_vm_space.vm_alloc_map);
}

struct kmem_page * kmem_page_struct_alloc() {
    struct kmem_page * page;
    if(_page_struct_free_list) {
        page = _page_struct_free_list;
        LL_DELETE(_page_struct_free_list, page);
    } else {
        page = (struct kmem_page *)kalloc_static(sizeof(*page), sizeof(*page));
    }
    return page;
}

void kmem_page_struct_free(struct kmem_page *page) {
    LL_PREPEND(_page_struct_free_list, page);
}
