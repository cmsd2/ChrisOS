#include <mm/layout.h>
#include <utils/panic.h>
#include <utils/mem.h>

#include <arch/bits.h>

#include <arch/paging.h> //TODO: move page_align to paging system?

/* align functions from 
 * http://en.wikipedia.org/wiki/Data_structure_alignment
 */

char human_memory_suffixes[] = {
	'B',
	'K',
	'M',
	'G',
	'T'
};

void memory_calc_human_size(uint64_t size, size_t * result, enum human_memory_units * units_result) {
	enum human_memory_units units = BYTES;
	while(size >= 1024) {
		size /= 1024;
		units++;
	}
	*result= size;
	*units_result = units;
}

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

uintptr_t page_align(uintptr_t addr) {
    return align_address(addr, PAGE_SIZE);
}

uintptr_t page_align_down(uintptr_t addr) {
    return align_address_down(addr, PAGE_SIZE);
}

unsigned int log2_i(unsigned int n) {
	unsigned i = 0;
	while(n > 1) {
		i++;
		n >>= 1;
	}
	return i;
}

unsigned int log2_l(unsigned long n) {
	unsigned i = 0;
	while(n > 1) {
		i++;
		n >>= 1;
	}
	return i;
}

unsigned int log2_32(uint32_t n) {
	unsigned i = 0;
	while(n > 1) {
		i++;
		n >>= 1;
	}
	return i;
}

unsigned int log2_64(uint64_t n) {
	unsigned i = 0;
	while(n > 1) {
		i++;
		n >>= 1;
	}
	return i;
}

void * memset(void * addr, uint8_t value, size_t len) {
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

void * memcpy(void * dest, const void * src, size_t len) {
    //TODO optimise memcpy: don't use memmove
    return memmove(dest, src, len);
}

void * memmove(void * dest, const void * src, size_t len) {
    //TODO optimise memmove: don't use single-byte operations
    if(dest < src) {
        kmemcpy_b_fwd(dest, src, len);
    } else {
        kmemcpy_b_rev(dest, src, len);
    }
}

void kmemcpy_b_fwd(uint8_t * dest, const uint8_t * src, size_t len) {
    for(size_t i = 0; i < len; i++) {
		dest[i] = src[i];
	}
}

void kmemcpy_b_rev(uint8_t * dest, const uint8_t * src, size_t len) {
    for(size_t i = len; i > 0; i--) {
		dest[i-1] = src[i-1];
	}
}

#include <utils/kprintf.h>
uintptr_t kalloc_dup_static(uintptr_t src, size_t size, size_t align) {
    uintptr_t dest = kalloc_static(size, align);
    memcpy((void*)dest, (void*)src, size);
    return dest;
}

uintptr_t kalloc_static(size_t size, size_t align) {
    if(!align)
        align = sizeof(uintptr_t);

	uintptr_t addr = align_address(_kernel_layout.memory_start, align);

	_kernel_layout.memory_start = addr + size;

	if(_kernel_layout.memory_start > _kernel_layout.memory_end) {
		panic("kalloc_static start > end. OOM");
	}

	return addr;
}
