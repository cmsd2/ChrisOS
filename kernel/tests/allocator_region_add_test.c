#include <assert.h>
#include <tests/tests.h>

#include "mm/allocator.h"

int test_allocator_region_add_test(void) {
    struct allocator_map map;
    struct allocator_region a, b, c;

    printf("simple insertion\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    allocator_region_add(&map, &a);
    assert(map.regions);
    assert(map.regions->address == 1);
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
    allocator_region_add(&map, &b);
    assert(map.regions->address == 1);
    printf("%ld\n", map.regions->size);
    assert(map.regions->size == 8);
    assert(map.regions->next == 0);

    printf("adjacent lhs\n");
    allocator_map_init(&map);
    a.address = 5;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 8);
    assert(map.regions->next == 0);

    printf("no overlap rhs\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 6;
    b.size =  4;
    b.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 6);
    assert(map.regions->next->size == 4);

    printf("no overlap lhs\n");
    allocator_map_init(&map);
    a.address = 6;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size =  4;
    b.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 6);
    assert(map.regions->next->size == 4);

    printf("merge left in between, no right merge\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 3;
    b.flags = 0;
    c.address = 9;
    c.size = 2;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    allocator_region_add(&map, &c);
    assert(map.regions->address == 1);
    assert(map.regions->size == 7);
    assert(map.regions->next);
    assert(map.regions->next->address == 9);
    assert(map.regions->next->size == 2);
    assert(!map.regions->next->next);

    printf("merge left in between, with right merge\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 3;
    b.flags = 0;
    c.address = 8;
    c.size = 2;
    c.flags = 0;
    allocator_region_add(&map, &a);
    allocator_region_add(&map, &b);
    allocator_region_add(&map, &c);
    assert(map.regions->address == 1);
    assert(map.regions->size == 9);
    assert(!map.regions->next);

    return 0;
}

