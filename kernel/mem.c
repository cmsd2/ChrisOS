#include <kernel/layout.h>
#include <kernel/panic.h>

#include <arch/bits.h>

/* align functions from 
 * http://en.wikipedia.org/wiki/Data_structure_alignment
 */

/* align is a power of 2 */
uintptr_t align_address(uintptr_t address, size_t align) {
	return (address + align - 1) & ~(align - 1);
}

size_t align_padding(uintptr_t address, size_t align) {
	return (-address) & (align - 1);
}

uintptr_t align_address_down(uintptr_t address, size_t align) {
	return address & ~(align - 1);
}

size_t align_padding_down(uintptr_t address, size_t align) {
	return (address) & (align - 1);
}

void * kmemset(void * addr, uint8_t value, size_t len) {

	uint8_t * b_addr;
	uint8_t * word_addr;
	unsigned long word_value = FILL_LONG_WITH_BYTE(value);
	size_t len_beginning, len_middle, len_end;

	size_t i;

	len_beginning = align_padding((uintptr_t)addr, sizeof(long));
	len_middle = len - len_beginning;
	len_end = align_padding_down((uintptr_t)len_middle, sizeof(long));
	len_middle -= len_end;

	b_addr = (uint8_t*)addr;
	for(i = 0; i < len_beginning; i++) {
		b_addr[i] = value;
	}

	word_addr = b_addr + len_beginning;
	for(i = 0; i < len_middle; i += sizeof(long)) {
		*((unsigned long*)(word_addr + i)) = word_value;
	}

	b_addr = word_addr + len_middle;
	for(i = 0; i < len_end; i++) {
		b_addr[i] = value;
	}

	return addr;
}

uintptr_t kalloc_static(size_t size, size_t align) {
	uintptr_t addr = align_address(_kernel_layout.memory_start, align);

	_kernel_layout.memory_start = addr + size;

	if(_kernel_layout.memory_start > _kernel_layout.memory_end) {
		panic("kalloc_static start > end. OOM");
	}

	return addr;
}
