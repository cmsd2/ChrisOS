#include <arch/paging.h>
#include <kernel/layout.h>
#include <kernel/panic.h>
#include <kernel/mem.h>

struct page_directory * _page_dir;

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
		panic();
	}
	*e = *e | frame;
}

uintptr_t paging_pd_entry_get_frame(pd_entry *e) {
	return *e & I86_PDE_FRAME;
}

void paging_map() {
}

void paging_init(void) {
	paging_init_pd();
	paging_identity_map(0L, PT_ENTRIES * PAGE_SIZE);
}

/* map region in kernel's layout struct */
void paging_identity_map_kernel() {
	uintptr_t start = align_address_down(_kernel_layout.memory_start, PT_ENTRIES * PAGE_SIZE);
	uintptr_t size = _kernel_layout.memory_end - start;

	paging_identity_map(start, size);
}

void paging_init_pd(void) {
	int i;

	_page_dir = (struct page_directory*) kalloc_static(sizeof(struct page_directory), PAGE_SIZE);

	for(i = 0; i < PDT_ENTRIES; i++) {
		paging_pd_entry_zero(&_page_dir->entries[i]);
	}
}

void paging_identity_map(uintptr_t base_addr, size_t size) {
	struct page_table *pt;
	uintptr_t addr = base_addr;
	size_t pd_frame_size = PAGE_SIZE * PT_ENTRIES;
	pd_entry * pd;

	while(size > 0) {
 		pd = paging_pd_for_addr(addr);

		if(!paging_pd_entry_get_frame(pd)) {
			pt = paging_new_pt_map(addr, pd_frame_size);
			if(!pt) {
				panic();
			}
			paging_pd_install_pt(pd, pt, I86_PDE_PRESENT | I86_PDE_WRITABLE);
		} else if(!paging_pd_entry_is_present(pd)) {
			panic();
		} else {
			// already mapped ???
			panic();
		}

		size -= pd_frame_size;
		addr += pd_frame_size;
	}
}

size_t paging_pd_id_for_addr(uintptr_t addr) {
	size_t id = addr / PAGE_SIZE / PT_ENTRIES;
	return id;
}

pd_entry * paging_pd_for_addr(uintptr_t addr) {
	size_t pd_id = paging_pd_id_for_addr(addr);
	pd_entry * pd = _page_dir->entries + pd_id;
	return pd;
}

void paging_pd_install_pt(pd_entry * pd, struct page_table * pt, uint32_t flags)
{
	paging_pd_entry_set_frame(pd, (uintptr_t)pt);
	paging_pd_entry_set_flags(pd, flags, 0L);
}

/*
 * initialise a complete page table with mappings
 * for some memory starting at base_addr
 */
void paging_pt_map(struct page_table * pt, uintptr_t base_addr, uint32_t flags) {
	int i;
	uintptr_t addr = base_addr;

	for(i = 0; i < PT_ENTRIES; i++) {
		pt->entries[i] = addr | flags;
		addr += PAGE_SIZE;
	}
}

/*
 * statically allocate a pt table and map some memory immediately
 */
struct page_table * paging_new_pt_map(uintptr_t base_addr, uint32_t flags) {

	struct page_table *pt = (struct page_table*) kalloc_static(sizeof(struct page_table), PAGE_SIZE);

	if(pt) {
		paging_pt_map(pt, base_addr, flags);
	}

	return pt;
}
