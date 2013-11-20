#include <arch/paging.h>
#include <kernel/layout.h>
#include <kernel/panic.h>
#include <kernel/mem.h>
#include <kernel/kprintf.h>
#include <arch/layout.h>

struct page_directory * _kernel_page_dir;

void paging_set_attr(uint32_t * e, uint32_t attr) {
	*e |= attr;
}

void paging_clear_attr(uint32_t * e, uint32_t attr) {
	*e &= ~attr;
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
	paging_identity_map(_kernel_page_dir, 0L, PT_ENTRIES * PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	paging_map_kernel(_kernel_page_dir);
	paging_load_page_dir(_kernel_page_dir);
	paging_enable();
}

void paging_load_page_dir(struct page_directory * pd) {
	__asm__ volatile (
		"mov %0, %%eax;"
		"mov %%eax, %%cr3;"
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

/* map region in kernel's layout struct */
void paging_map_kernel(struct page_directory * ptd) {
	uintptr_t start = align_address_down(_kernel_layout.segment_start, PT_ENTRIES * PAGE_SIZE);
	uintptr_t size = _kernel_layout.memory_end - start;

	paging_map(ptd, 0, size, start, I86_PTE_PRESENT | I86_PTE_WRITABLE);
}

void paging_init_kernel_page_dir(void) {
	int i;

	_kernel_page_dir = (struct page_directory*) kalloc_static(sizeof(struct page_directory), PAGE_SIZE);

	for(i = 0; i < PDT_ENTRIES; i++) {
		paging_pd_entry_zero(&_kernel_page_dir->entries[i]);
	}
}

void paging_map(struct page_directory * ptd, uintptr_t base_addr, size_t size, uintptr_t vaddr, uint32_t flags) {
	struct page_table *pt;
	uintptr_t addr = base_addr;
	pd_entry * pde;
	int pages = align_address(size, PAGE_SIZE) / PAGE_SIZE;
	int cur_pages;

	while(size != 0) {
 		pde = paging_pd_entry_for_addr(ptd, vaddr);

		if(!paging_pd_entry_get_frame(pde)) {
			pt = paging_alloc_static_pt();
			if(!pt) {
				panic("couldn't alloc page table");
			} else {
				//kprintf("allocating page table at 0x%x\n", pt);
			}
		} else if(!paging_pd_entry_is_present(pde)) {
			panic("paging pd entry is not present");
		} else {
			// already mapped ???
			panic("already mapped ???");
		}

		cur_pages = PT_ENTRIES;
		if(pages < PT_ENTRIES) {
			cur_pages = pages;
		}

		kprintf("mapping %u pages at vaddr 0x%x to addr 0x%x\n", cur_pages, vaddr, base_addr);

		cur_pages = paging_pt_map(pt, base_addr, cur_pages, flags);

		paging_pd_install_pt(pde, pt, I86_PDE_PRESENT | I86_PDE_WRITABLE);

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

/*
 * initialise a complete page table with mappings for a number of pages
 * for some memory starting at base_addr
 */
size_t paging_pt_map(struct page_table * pt, uintptr_t base_addr, size_t pages, uint32_t flags) {
	size_t i;
	uintptr_t addr = base_addr;
	size_t pt_id = paging_pt_entry_id_for_addr(addr);
	size_t pt_end_id = pt_id + pages;
	size_t actual_pages = pages;

	if(pt_end_id > PT_ENTRIES) {
		actual_pages -= (pt_end_id - PT_ENTRIES);
		pt_end_id = PT_ENTRIES;
	}

	for(i = 0; i < pt_id; i++) {
		pt->entries[i] = 0;
	}

	for(; i < pt_end_id; i++) {
		pt->entries[i] = addr | flags;
		addr += PAGE_SIZE;
	}

	for(; i < PT_ENTRIES; i++) {
		pt->entries[i] = 0;
	}

	//kprintf("mapped pt for frames %x to %x\n", base_addr, addr);

	return actual_pages;
}

struct page_table * paging_alloc_static_pt(void) {
	struct page_table * pt = (struct page_table*) kalloc_static(sizeof(struct page_table), PAGE_SIZE);
	return pt;
}

/*
 * statically allocate a pt table and map some memory immediately
 */
struct page_table * paging_new_pt_map(uintptr_t base_addr, int pages, uint32_t flags) {

	struct page_table *pt = (struct page_table*) kalloc_static(sizeof(struct page_table), PAGE_SIZE);

	if(pt) {
		paging_pt_map(pt, base_addr, pages, flags);
	}

	return pt;
}

