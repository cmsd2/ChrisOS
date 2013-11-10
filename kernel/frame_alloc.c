#include <arch/layout.h>
#include <kernel/frame_alloc.h>

/*
 * round up address until we get to a frame-aligned address
 */
uintptr_t frame_align(uintptr_t address) {
	size_t remainder = address % frame_size;

	if(remainder) {
		return address + (frame_size - remainder);
	} else {
		return address;
	}
}

size_t frame_alloc_id(void) {
	for(size_t i = 0; i < frame_count; i++) {
		if(frame_map[i] == FREE) {
			frame_map[i] = USED;
			return i;
		}
	}
	return (size_t)-1;
}

void * frame_alloc(void) {
	size_t frame_number = frame_alloc_id();

	if(frame_number == (size_t)-1) {
		return 0;
	} else {
		return (void*)((uintptr_t)frame_start + frame_number * frame_size);
	}
}

void frame_free_id(size_t frame_number) {
	frame_map[frame_number] = FREE;
}

void frame_free(void * frame_ptr) {
	long frame_id = ((uintptr_t)frame_ptr - (uintptr_t)frame_start) / frame_size;

	frame_free_id(frame_id);
}


