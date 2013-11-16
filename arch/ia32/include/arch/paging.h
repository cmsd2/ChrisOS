#ifndef ARCH_PAGING_H
#define ARCH_PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PDT_ENTRIES 1024
#define PT_ENTRIES 1024

#define PAGE_SIZE 4096

enum PAGE_PDE_FLAGS {
 
	I86_PDE_PRESENT			=	1,		//0000000000000000000000000000001
	I86_PDE_WRITABLE		=	2,		//0000000000000000000000000000010
	I86_PDE_USER			=	4,		//0000000000000000000000000000100
	I86_PDE_PWT			=	8,		//0000000000000000000000000001000
	I86_PDE_PCD			=	0x10,		//0000000000000000000000000010000
	I86_PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PDE_4MB			=	0x80,		//0000000000000000000000010000000
	I86_PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PDE_LV4_GLOBAL		=	0x200		//0000000000000000000001000000000
};
static const uint32_t I86_PDE_FRAME = 0xFFFFF000;

enum PAGE_PTE_FLAGS {
	I86_PTE_PRESENT			=	1,		//0000000000000000000000000000001
	I86_PTE_WRITABLE		=	2,		//0000000000000000000000000000010
	I86_PTE_USER			=	4,		//0000000000000000000000000000100
	I86_PTE_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	I86_PTE_NOT_CACHEABLE		=	0x10,		//0000000000000000000000000010000
	I86_PTE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PTE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PTE_PAT			=	0x80,		//0000000000000000000000010000000
	I86_PTE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PTE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
};
static const uint32_t I86_PTE_FRAME = 0xFFFFF000;

typedef uintptr_t pt_entry;
typedef uintptr_t pd_entry;

struct page_directory {
	/* each entry is a frame pointer, OR'd with flags
	 * if using 4K pages, a frame points to a page table
	 * if using huge 4M pages, a frame points directly to mapped memory
	 * a pd entry's frame is PAGE_SIZE * PT_ENTRIES bytes (4M)
	 * a pd covers PAGE_SIZE * PT_ENTRIES * PDT_ENTRIES bytes (4G)
	 */
	pd_entry entries[PDT_ENTRIES];
};

struct page_table {
	/* page tables not used with 4M page sizes.
	 * each entry is a frame pointer to memory or disk, OR'd with flags
	 * a pt entry's frame is PAGE_SIZE bytes (4K)
	 * a pt covers PAGE_SIZE * PT_ENTRIES bytes (4M)
	 */
	pt_entry entries[PT_ENTRIES];
};

void paging_set_attr(uint32_t * e, uint32_t attr);
void paging_clear_attr(uint32_t * e, uint32_t attr);

void paging_pd_entry_invl(pd_entry *e);
void paging_pd_entry_zero(pd_entry *e);
void paging_pd_entry_present(pd_entry *e);
bool paging_pd_entry_is_present(pd_entry *e);
void paging_pd_entry_writable(pd_entry *e);
void paging_pd_entry_set_flags(pd_entry *e, uint32_t flags, uint32_t mask);
void paging_pd_entry_set_frame(pd_entry *e, uintptr_t frame);
uintptr_t paging_pd_entry_get_frame(pd_entry *e);

void paging_enable(void);
void paging_init(void);
void paging_init_kernel_page_dir(void);
void paging_load_page_dir(struct page_directory *pd);
void paging_init_pts(void);
void paging_pt_map(struct page_table * pt, uintptr_t base_addr, int pages, uint32_t flags);
struct page_table * paging_alloc_static_pt(void);
void paging_identity_map(struct page_directory * ptd, uintptr_t base_addr, size_t size, uint32_t flags);
void paging_map(struct page_directory * ptd, uintptr_t base_addr, size_t size, uintptr_t vaddr, uint32_t flags);
void paging_map_kernel(struct page_directory * ptd);
size_t paging_pd_entry_id_for_addr(uintptr_t addr);
pd_entry * paging_pd_entry_for_addr(struct page_directory *pd, uintptr_t addr);
void paging_pd_install_pt(pd_entry * pd, struct page_table * pt, uint32_t flags);


#endif
