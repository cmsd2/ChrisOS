#include <mm/layout.h>
#include <boot/layout.h>
#include <utils/mem.h>

/* for page size stuff */
#include <arch/paging.h> /* TODO: remove arch specific bit */

struct layout _kernel_layout;

void layout_init(void) {
	_kernel_layout.segment_start = (uintptr_t)&KERNEL_VMA;

	_kernel_layout.memory_start = (uintptr_t)&_end + 0x1000;

	_kernel_layout.memory_end = align_address((uintptr_t)&_end, PAGE_SIZE * PT_ENTRIES);
}

uintptr_t physical_address(uintptr_t kernel_address) {
	return kernel_address - _kernel_layout.segment_start;
}
