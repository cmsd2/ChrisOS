#include <kernel/layout.h>
#include <kernel/panic.h>

uintptr_t align_address(uintptr_t address, size_t align) {
	return (address + align - 1) & ~(align - 1);
}

uintptr_t align_address_down(uintptr_t address, size_t align) {
	return address & ~(align - 1);
}

uintptr_t kalloc_static(size_t size, size_t align) {
	uintptr_t addr = align_address(_kernel_layout.memory_start, align);

	_kernel_layout.memory_start = addr + size;

	if(_kernel_layout.memory_start > _kernel_layout.memory_end) {
		panic();
	}

	return addr;
}
