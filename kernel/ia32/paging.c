#include <sys/param.h>
#include <arch/paging.h>
#include <mm/layout.h>
#include <mm/malloc.h>
#include <utils/panic.h>
#include <utils/mem.h>
#include <utils/kprintf.h>
#include <boot/layout.h>
#include <sys/standard.h>
#include <assert.h>
#include <mm/kmem.h>

struct page_directory * _kernel_page_dir;
struct page_directory * _current_page_dir;

struct page_directory * paging_pd_current() {
    return _current_page_dir;
}

void paging_set_attr(uint32_t * e, uint32_t attr) {
    *e |= attr;
}

void paging_clear_attr(uint32_t * e, uint32_t attr) {
    *e &= ~attr;
}

struct page_directory * paging_pd_alloc() {
    return kmalloc(sizeof(struct page_directory));
}

void paging_pd_free(struct page_directory * pd) {
    kfree(pd);
}

struct page_directory * paging_pd_clone(const struct page_directory * pd) {
    struct page_directory * result = kmalloc(sizeof(struct page_directory));

    // deep copy userspace page tables and pages
    for(int i = 0; i < PDT_KERNEL_START; i++) {
	result->entries[i] = paging_pd_entry_clone(pd->entries[i]);
    }

    // shallow copy kernel page tables
    for(int i = PDT_KERNEL_START; i < PDT_ENTRIES; i++) {
	result->entries[i] = pd->entries[i];
    }

    return result;
}

pd_entry paging_pd_entry_clone(pd_entry e) {
    pd_entry result = e;

    uintptr_t frame = paging_pd_entry_get_frame(&result);

    if(result & I86_PDE_4MB) {
        frame = paging_page_clone_4mb(frame);
    } else {
        frame = (uintptr_t)paging_pt_clone((struct page_table *)frame);
    }

    paging_pd_entry_set_frame(&result, frame);

    return result;
}

void paging_pd_entry_invl(pd_entry *e) {
    paging_clear_attr(e, I86_PDE_PRESENT);
}

void paging_pd_entry_zero(pd_entry *e) {
    *e = 0;
}

void paging_pd_entry_present(pd_entry *e) {
    paging_set_attr(e, I86_PDE_PRESENT);
}

bool paging_pd_entry_is_present(pd_entry *e) {
    if(*e & I86_PDE_PRESENT) {
        return true;
    } else{
        return false;
    }
}

void paging_pd_entry_writable(pd_entry *e) {
    paging_set_attr(e, I86_PDE_WRITABLE);
}

/*
 * mask off unwanted flags, and set new ones.
 * does not touch frame bits
 * to entirely replace entry flags with new ones, set mask to 0
 * to leave all flags in tact, set mask to -1
 * to clear a flag, set mask to ~flag, and flags to 0
 */
void paging_pd_entry_set_flags(pd_entry *e, uint32_t flags, uint32_t mask) {
    uint32_t flags_and_frame_mask = (mask | I86_PDE_FRAME);
    *e = (*e & flags_and_frame_mask) | flags;
}

/*
 * frame is aligned to PAGE_SIZE * PT_ENTRIES
 */
void paging_pd_entry_set_frame(pd_entry *e, uintptr_t frame) {
    if(frame & ~(I86_PDE_FRAME)) {
	panic("frame not page aligned");
    }
    *e = (*e & ~(I86_PDE_FRAME)) | frame;
}

uintptr_t paging_pd_entry_get_frame(pd_entry *e) {
    return *e & I86_PDE_FRAME;
}

void paging_init(void) {
    paging_init_kernel_page_dir();
    paging_identity_map(_kernel_page_dir, 0L, PT_ENTRIES * PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
    paging_map_kernel(_kernel_page_dir);
    paging_load_page_dir(_kernel_page_dir);
    paging_enable();
}

void paging_cleanup_bootstrap(void) {
    paging_unmap(_kernel_page_dir, 0L, PT_ENTRIES * PAGE_SIZE);
}

void paging_load_page_dir(struct page_directory * pd) {
    _current_page_dir = pd;
    
    __asm__ volatile (
            "mov %0, %%eax\n"
            "mov %%eax, %%cr3\n"
            :
            : "r" (((uintptr_t)pd) - (uintptr_t)&KERNEL_VMA)
            : "%eax");
}

void paging_enable(void) {
    __asm__ volatile (
            "mov %%cr0, %%eax\n"
            "orl $0x80000000, %%eax\n"
            "mov %%eax, %%cr0"
            : : : "%eax");
}

void paging_flush(void) {
    __asm__ volatile (
            "mov %%cr3, %%eax\n"
            "mov %%eax, %%cr3"
            : : : "%eax");
}

/*
 * zeroes page table entries for a virtual address range
 * calculates which page table to start at,
 * and calls paging_pt_unmap for each page in the range
 * range must be page aligned
 */
void paging_unmap(struct page_directory * ptd, uintptr_t vaddr, size_t size) {
    struct page_table *pt;
    pd_entry * pde;
    assert(align_address(vaddr, PAGE_SIZE) == vaddr);
    assert(align_address(size, PAGE_SIZE) == size);
    int pages = size / PAGE_SIZE;
    int cur_pages;
    size_t pt_id;
    size_t incr;

    while(pages) {
        pde = paging_pd_entry_for_addr(ptd, vaddr);
        
        if(!(pt = paging_pt_for_pd_entry(pde))) {
            panic("page table not installed. can't unmap");
        } else if(!paging_pd_entry_is_present(pde)) {
            panic("paging pd entry is not present");
        }

        pt_id = paging_pt_entry_id_for_addr(vaddr);

        cur_pages = MIN(pages, PT_ENTRIES - pt_id);

        //kprintf("unmapping %u pages at vaddr 0x%x\n", cur_pages, vaddr);

        paging_pt_unmap(pt, vaddr, cur_pages);

        pages -= cur_pages;

        incr = cur_pages * PAGE_SIZE;
        size -= incr;
        vaddr += incr;
    }
    assert(size == 0);
}

/* map region in kernel's layout struct */
void paging_map_kernel(struct page_directory * ptd) {
    uintptr_t start = align_address_down(_kernel_layout.segment_start, PT_ENTRIES * PAGE_SIZE);
    uintptr_t size = _kernel_layout.memory_end - start;

    paging_map(ptd, 0, size, start, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
}

void paging_init_kernel_page_dir(void) {
    _kernel_page_dir = (struct page_directory*) kalloc_static(sizeof(struct page_directory), PAGE_SIZE);

    paging_pd_init(_kernel_page_dir);
}

void paging_pd_init(struct page_directory * pd) {
    for(int i = 0; i < PDT_ENTRIES; i++) {
        paging_pd_entry_zero(&_kernel_page_dir->entries[i]);
    }
}

void paging_map(struct page_directory * ptd, uintptr_t base_addr, size_t size, uintptr_t vaddr, uint32_t flags) {
    struct page_table *pt;
    uintptr_t addr = base_addr;
    pd_entry * pde;
    assert(align_address(vaddr, PAGE_SIZE) == vaddr);
    assert(align_address(size, PAGE_SIZE) == size);
    int pages = size / PAGE_SIZE;
    int cur_pages;
    size_t pt_id;
    size_t incr;

    while(size != 0) {
        pde = paging_pd_entry_for_addr(ptd, vaddr);

        if(!(pt = paging_pt_for_pd_entry(pde))) {
            pt = paging_alloc_static_pt();
            if(!pt) {
                panic("couldn't alloc page table");
            } else {
                kprintf("allocating page table at 0x%x\n", pt);
            }
            paging_pd_install_pt(pde, pt, I86_PDE_PRESENT | I86_PDE_WRITABLE | I86_PDE_USER);
        } else if(!paging_pd_entry_is_present(pde)) {
            kprintf("pd entry %lx is weird\n", *pde);
            panic("paging pd entry is not present");
        } else {
            //kprintf("using already present page table\n");
        }

        pt_id = paging_pt_entry_id_for_addr(vaddr);

        cur_pages = MIN(pages, PT_ENTRIES - pt_id);

        //kprintf("mapping %u pages at vaddr 0x%x to addr 0x%x\n", cur_pages, vaddr, base_addr);

        paging_pt_map(pt, vaddr, base_addr, cur_pages, flags);

        pages -= cur_pages;

        size_t incr = cur_pages * PAGE_SIZE;
        size -= incr;
        addr += incr;
    }

}

void paging_identity_map(struct page_directory * ptd, uintptr_t base_addr, size_t size, uint32_t flags) {
    paging_map(ptd, base_addr, size, base_addr, flags);
}

size_t paging_pd_entry_id_for_addr(uintptr_t addr) {
    size_t id = addr / PAGE_SIZE / PT_ENTRIES;
    //kprintf("using page id %d for addr 0x%x\n", id, addr);
    return id;
}

pd_entry * paging_pd_entry_for_addr(struct page_directory * ptd, uintptr_t addr) {
    size_t pd_id = paging_pd_entry_id_for_addr(addr);
    pd_entry * pde = ptd->entries + pd_id;
    return pde;
}

size_t paging_pt_entry_id_for_addr(uintptr_t addr) {
    size_t pt_mask = (PAGE_SIZE * PT_ENTRIES) - 1;
    size_t id = (addr & pt_mask) / PAGE_SIZE;
    //kprintf("using page table id %d for addr 0x%x\n", id, addr);
    assert(id < PT_ENTRIES);
    return id;
}

pt_entry * paging_pt_entry_for_addr(struct page_table * pt, uintptr_t addr) {
    size_t pd_id = paging_pt_entry_id_for_addr(addr);
    pt_entry * pte = pt->entries + pd_id;
    return pte;
}

void paging_pd_install_pt(pd_entry * pde, struct page_table * pt, uint32_t flags)
{
    paging_pd_entry_set_frame(pde, (uintptr_t)pt - (uintptr_t)(&KERNEL_VMA));
    paging_pd_entry_set_flags(pde, flags, 0L);
    //kprintf("installed pt: 0x%x\n", *pde);
}

struct page_table * paging_pt_for_pd_entry(pd_entry * pde) {
    uintptr_t frame = paging_pd_entry_get_frame(pde);
    if(frame) {
        return (struct page_table *)(frame + (uintptr_t)(&KERNEL_VMA));
    } else {
        return 0;
    }
}

/*
 * initialise a complete page table with mappings for a number of pages
 * for some memory starting at base_addr
 */
size_t paging_pt_map(struct page_table * pt, uintptr_t vaddr, uintptr_t base_addr, size_t pages, uint32_t flags) {
    size_t i;
    uintptr_t addr = base_addr;
    size_t pt_id = paging_pt_entry_id_for_addr(vaddr);
    size_t pt_end_id = pt_id + pages;
    size_t actual_pages = pages;

    if(pt_end_id > PT_ENTRIES) {
        actual_pages -= (pt_end_id - PT_ENTRIES);
        pt_end_id = PT_ENTRIES;
    }
    
    for(i = pt_id; i < pt_end_id; i++) {
        pt->entries[i] = addr | flags;
        addr += PAGE_SIZE;
    }
    
    //kprintf("mapped pt for frames starting 0x%lx and ending 0x%lx\n", base_addr, addr);

    return actual_pages;
}

void paging_pt_unmap(struct page_table * pt, uintptr_t vaddr, size_t pages) {
    size_t i;
    size_t pt_id = paging_pt_entry_id_for_addr(vaddr);
    size_t pt_end_id = pt_id + pages;
    
    assert(pt_end_id <= PT_ENTRIES);
    
    for(i = pt_id; i < pt_end_id; i++) {
        pt->entries[i] = 0;
    }
}

struct page_table * paging_alloc_static_pt(void) {
    struct page_table * pt = (struct page_table*) kalloc_static(sizeof(struct page_table), PAGE_SIZE);
    memset(pt, 0, sizeof(*pt));
    return pt;
}

/*
 * statically allocate a pt table and map some memory immediately
 */
struct page_table * paging_new_pt_map(uintptr_t vaddr, uintptr_t base_addr, int pages, uint32_t flags) {

    struct page_table *pt = (struct page_table*) kalloc_static(sizeof(struct page_table), PAGE_SIZE);

    if(pt) {
        paging_pt_map(pt, vaddr, base_addr, pages, flags);
    }

    return pt;
}

struct page_table * paging_pt_clone(const struct page_table * pt) {
    struct page_table * result = (struct page_table *)kmalloc(sizeof(struct page_table));

    for(int i = 0; i < PT_ENTRIES; i++) {
        result->entries[i] = paging_pt_entry_clone(pt->entries[i]);
    }

    return result;
}

pt_entry paging_pt_entry_clone(pt_entry e) {
    pt_entry result = e & ~I86_PTE_FRAME;

    uintptr_t frame = e & I86_PTE_FRAME;

    uintptr_t result_frame = paging_page_clone_4k(frame);
    assert((result_frame & ~I86_PTE_FRAME) == 0);

    result |= result_frame;
}

uintptr_t paging_page_clone_4mb(uintptr_t frame) {
    return kmem_frames_clone(frame, 1024);
}

uintptr_t paging_page_clone_4k(uintptr_t frame) {
    return kmem_frame_clone(frame);
}
