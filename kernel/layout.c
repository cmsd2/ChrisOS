#include <kernel/layout.h>
#include <arch/layout.h>

struct layout _kernel_layout;

void layout_init(void) {
	_kernel_layout.memory_start = (uintptr_t)&_code;
	_kernel_layout.memory_end = (uintptr_t)&_end;
}
