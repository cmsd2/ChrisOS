#ifndef KERNEL_MM_ALLOCATOR_H
#define KERNEL_MM_ALLOCATOR_H

/*
A virtual memory allocator for allocating arbitrary sized areas from an address space
consisting of a list of non-overlapping regions of available areas.
*/

#include <stddef.h>
#include <stdint.h>
#include <mm/range.h>
#include <mm/mm_types.h>

enum alloc_region_flags {
    ALLOC_VM_USER = 1,
    ALLOC_VM_KERNEL = 2,

	ALLOC_PM_DMA = 4,
	ALLOC_PM_NORMAL = 8,
	ALLOC_PM_HIGH = 16
};

struct allocator_map;

struct allocator_region {
    struct allocator_region *next;
    struct allocator_map *map;
    mm_ptr_t address;
    mm_size_t size;
    enum alloc_region_flags flags;
};

struct allocator_map {
    struct allocator_region *regions;
};

void allocator_map_init(struct allocator_map * map);

// add a new region
void allocator_region_add_new(struct allocator_map * map, uintptr_t address, size_t size, enum alloc_region_flags flags);

// subtract a region
void allocator_range_acquire(struct allocator_map * map, uintptr_t address, size_t size);

// allocate an area big enough
// leaves a hole in the regions map
// return region and region size
bool allocator_mem_alloc(struct allocator_map * map, size_t size, size_t alignment, enum alloc_region_flags flags, mm_ptr_t * result);

// free a region
// will be added to the list, and merged width adjacent regions if the flags match
void allocator_mem_free(struct allocator_map * map, mm_ptr_t ptr, size_t size, enum alloc_region_flags flags);

// print available region addresses and sizes
void allocator_print_map_info(struct allocator_map * map);

MRL_PROTOS(allocator_region, struct allocator_map, struct allocator_region);

//other required functions for region manipulation
//todo: move these into the protos macro
struct allocator_region * allocator_region_alloc(void);
void allocator_region_free(struct allocator_region * region);
bool allocator_region_pre_merge(struct allocator_region * a, struct allocator_region * b);
void allocator_region_post_merge(struct allocator_region * a, struct allocator_region * b);
void allocator_region_post_split(struct allocator_region * a, struct allocator_region * b);

#endif
