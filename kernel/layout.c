#include <kernel/layout.h>
#include <arch/layout.h>
#include <kernel/mem.h>

/* for page size stuff */
#include <arch/paging.h> /* TODO: remove arch specific bit */

struct layout _kernel_layout;

void layout_init(void) {
	_kernel_layout.memory_start = (uintptr_t)&_end;

	_kernel_layout.memory_end = align_address((uintptr_t)&_end, PAGE_SIZE * PT_ENTRIES);
}
