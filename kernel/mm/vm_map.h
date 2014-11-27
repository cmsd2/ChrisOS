#ifndef VM_MAP_H
#define VM_MAP_H

#include <sys/standard.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <mm/vm_types.h>
#include <mm/range.h>

struct vm_map;

// region in a virtual address space. backed by memory or file or ...
// can look up physical memory details via page tables
struct vm_map_region {
    struct vm_map_region *next;
    struct vm_map *map;
    vm_ptr_t address;
    vm_size_t size;

    // flags, protection settings etc
};

// a list of regions in the virtual address space, sorted by start address
// each region backed by a piece of physical ram (or other source of data)
struct vm_map {
    struct vm_map_region *regions;
};

void vm_map_init(struct vm_map *map);

//doesn't initialise any fields
struct vm_map_region * vm_map_region_alloc();

//frees the space used by the region struct
//make sure to remove the region from any lists first
void vm_map_region_free(struct vm_map_region *region);

// insert a region into its proper place in the list
MRL_INSERT_PROTO(vm_map_region, struct vm_map, struct vm_map_region);

// unions two regions a and b if they are adjacent or overlap
// saving the pieces in a or a and b.
MR_MERGE_PROTO(vm_map_region, struct vm_map_region);

//creates a new region struct and calls add
void vm_map_region_add_new(struct vm_map *map, vm_ptr_t addr, vm_size_t size /*, attrs */);

//find, remove and free a region block
void vm_map_region_remove(struct vm_map *map, vm_ptr_t addr);

//adds and merges
MRL_ADD_PROTO(vm_map_region, struct vm_map, struct vm_map_region);
//void vm_map_region_add(struct vm_map *map, struct vm_map_region *region);

// cuts region b from region a, saving the 0, 1 or 2 pieces in a and b
MR_CUT_PROTO(vm_map_region, struct vm_map_region);

//removes and cuts holes
MRL_SUBTRACT_PROTO(vm_map_region, struct vm_map, struct vm_map_region);

MRL_FIND_ADDRESS_PROTO(vm_map_region, struct vm_map, struct vm_map_region, vm_ptr_t);
    
bool vm_map_region_pre_merge(struct vm_map_region *a, struct vm_map_region *b);

void vm_map_region_post_merge(struct vm_map_region *a, struct vm_map_region *b);

void vm_map_region_post_split(struct vm_map_region *a, struct vm_map_region *b);

#endif
