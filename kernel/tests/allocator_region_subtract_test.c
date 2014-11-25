#include <assert.h>
#include <tests/tests.h>
#include <utlist.h>

#include "mm/allocator.h"

void print_regions(struct allocator_region * list) {
    int i = 0;
    struct allocator_region * region;
    LL_FOREACH(list, region) {
        printf("region[%d] addr=%ld size=%ld at %lx\n", i, region->address, region->size, (uintptr_t)region);
        i++;
    }
}

void print_map(struct allocator_map * map) {
    print_regions(map->regions);
}

int test_allocator_region_subtract_test(void) {
    struct allocator_map map;
    struct allocator_region a, b, c;

    printf("adjacent lhs\n");
    allocator_map_init(&map);
    a.address = 5;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_subtract(&map, &b);
    assert(map.regions);
    assert(map.regions->address == 5);
    assert(map.regions->size == 4);

    printf("adjacent rhs\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 4;
    b.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_subtract(&map, &b);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next == 0);

    printf("in between, adjacent lhs\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 6;
    c.size = 4;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    allocator_region_subtract(&map, &c);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 10);
    assert(map.regions->next->size == 4);

    printf("in between, adjacent rhs\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 5;
    c.size = 4;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    printf("subtracting c\n");
    allocator_region_subtract(&map, &c);
    print_map(&map);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 10);
    assert(map.regions->next->size == 4);

    printf("in between, overlap both\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 4;
    c.size = 7;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    printf("subtracting c\n");
    allocator_region_subtract(&map, &c);
    print_map(&map);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 3);
    assert(map.regions->next);
    assert(map.regions->next->address == 11);
    assert(map.regions->next->size == 3);

    printf("in between, cover first\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 1;
    c.size = 7;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    printf("subtracting c\n");
    allocator_region_subtract(&map, &c);
    print_map(&map);
    assert(map.regions);
    assert(map.regions->address == 10);
    assert(map.regions->size == 4);
    assert(!map.regions->next);

    printf("in between, cover both\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 1;
    c.size = 14;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    printf("subtracting c\n");
    allocator_region_subtract(&map, &c);
    print_map(&map);
    assert(!map.regions);
 
    return 0;
 }
