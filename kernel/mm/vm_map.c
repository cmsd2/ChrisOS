#include <mm/vm_map.h>
#include <utils/mem.h>
#include <utlist.h>
#include <stdbool.h>

static struct vm_map_region * _free_regions;

MRL_IMPLS(vm_map_region, struct vm_map, struct vm_map_region, vm_ptr_t)

void vm_map_init(struct vm_map *map) {
    map->regions = 0;
}

struct vm_map_region * vm_map_region_alloc() {
    struct vm_map_region * region;
    if(_free_regions) {
        region = _free_regions;
        LL_DELETE(_free_regions, _free_regions);
    } else {
        region = (struct vm_map_region *)kalloc_static(sizeof(struct vm_map_region), sizeof(uintptr_t));
    }
    return region;
}

void vm_map_region_free(struct vm_map_region * region) {
    LL_PREPEND(_free_regions, region);
}

void vm_map_region_add_new(struct vm_map *map, vm_ptr_t addr, vm_size_t size) {
    struct vm_map_region * region = vm_map_region_alloc();
    assert(region);

    region->address = addr;
    region->size = size;

    vm_map_region_add(map, region);
}

void vm_map_region_remove(struct vm_map *map, vm_ptr_t addr) {
    struct vm_map_region *region;

    // loop through regions, and subtract the one with the matching start address
    LL_FOREACH(map->regions, region) {
        if(region->address == addr) {
            LL_DELETE(map->regions, region);
            vm_map_region_free(region);
            return;
        }
    }
}

// never merge adjacent blocks
// means we track the size of each allocated block and can easily free it
bool vm_map_region_pre_merge(struct vm_map_region *a, struct vm_map_region *b) {
    return false;
}

void vm_map_region_post_merge(struct vm_map_region *a, struct vm_map_region *b) {
    //nothing yet
}

void vm_map_region_post_split(struct vm_map_region *a, struct vm_map_region *b) {
    //nothing yet
}

