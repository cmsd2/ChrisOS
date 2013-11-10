#ifndef KERNEL_LAYOUT_H
#define KERNEL_LAYOUT_H

#include <stdint.h>
#include <stddef.h>

struct layout {
	uintptr_t memory_start;
	uintptr_t memory_end;
};

extern struct layout _kernel_layout;

void layout_init(void);

#endif
