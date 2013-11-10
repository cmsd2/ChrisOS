#ifndef KERNEL_FRAME_ALLOC_H
#define KERNEL_FRAME_ALLOC_H

#include <stdint.h>
#include <stddef.h>

#define FREE 0
#define USED 1

size_t frame_alloc_id(void);
void frame_free_id(size_t frame_number);

void * frame_alloc(void);
void frame_free(void *);

uintptr_t frame_align(uintptr_t address);

extern unsigned char * frame_map;
extern size_t frame_count;
extern size_t frame_size;
extern void * frame_start;

#endif
