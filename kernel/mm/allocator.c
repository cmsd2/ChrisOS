#include <mm/allocator.h>
#include <utlist.h>
#include <utils/mem.h>
#include <utils/kprintf.h>

static struct allocator_region * _free_regions;

MRL_IMPLS(allocator_region, struct allocator_map, struct allocator_region)

void allocator_map_init(struct allocator_map * map) {
    map->regions = 0;
}

void allocator_region_add_new(struct allocator_map *map, uintptr_t addr, size_t size, enum alloc_region_flags flags) {
    struct allocator_region * region = allocator_region_alloc();
    assert(region);

    region->address = addr;
    region->size = size;
    region->flags = flags;

    allocator_region_add(map, region);
}

void allocator_range_acquire(struct allocator_map * map, uintptr_t address, size_t size) {
    struct allocator_region region;
    region.address = address;
    region.size = size;

    allocator_region_subtract(map, &region);
}

void allocator_mem_free(struct allocator_map * map, mm_ptr_t ptr, size_t size, enum alloc_region_flags flags) {
    struct allocator_region * region = allocator_region_alloc();
    region->address = ptr;
    region->size = size;
    region->flags = flags;

    allocator_region_add(map, region);
}

bool allocator_mem_alloc(struct allocator_map * map, size_t size, size_t alignment, enum alloc_region_flags flags, mm_ptr_t * result) {
    struct allocator_region * region;
    mm_ptr_t aligned_start;
    mm_size_t remaining_size;
    struct allocator_region * hole = allocator_region_alloc();

    if(!alignment) {
        alignment = sizeof(uintptr_t);
    }

    // simple loop through regions
    LL_FOREACH(map->regions, region) {
        if(flags == 0 || region->flags == flags) {
            // stop on first one large enough
            aligned_start = align_address(region->address, alignment);
            remaining_size = region->size - (aligned_start - region->address);
    
            if(remaining_size >= size) {
                hole->address = aligned_start;
                hole->size = size;

                allocator_region_cut_and_stitch(map, region, hole);

                *result = aligned_start;

                return true;
            }
        }
    }
    kprintf("no regions with matching flags found\n");
    return false;
}

void allocator_print_map_info(struct allocator_map * map) {
    struct allocator_region * region;
    size_t available = 0;
    int fragmentation = 0;
    size_t largest_block = 0;
    int r_num = 0;
    LL_FOREACH(map->regions, region) {
        available += region->size;
        if(region->size > largest_block) {
            largest_block = region->size;
        }
        kprintf("allocator region[%d] addr=%lx size=%lx\n", r_num, region->address, region->size);
        r_num++;
    }
    if(available) {
        fragmentation = 100 - largest_block / (available / 100);
    }
    kprintf("allocator: available=%lx fragmentation=%d%%\n", available, fragmentation);
}

struct allocator_region * allocator_region_alloc() {
    struct allocator_region * region;
    if(_free_regions) {
        region = _free_regions;
        LL_DELETE(_free_regions, _free_regions);
    } else {
        return (struct allocator_region *)kalloc_static(sizeof(struct allocator_region), sizeof(uintptr_t));
    }
    return region;
}

void allocator_region_free(struct allocator_region * region) {
    LL_PREPEND(_free_regions, region);
}

bool allocator_region_pre_merge(struct allocator_region * a, struct allocator_region * b) {
    return a->flags == b->flags ? true : false;
}

void allocator_region_post_merge(struct allocator_region * a, struct allocator_region * b) {
    // do nothing
}

void allocator_region_post_split(struct allocator_region * a, struct allocator_region * b) {
    // do nothing
}

