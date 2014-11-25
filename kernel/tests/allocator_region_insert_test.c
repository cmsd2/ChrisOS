#include <assert.h>
#include <tests/tests.h>

#include "mm/allocator.h"

int test_allocator_region_insert_test(void) {
    struct allocator_map map;
    struct allocator_region a, b, c;

    printf("simple insertion\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    allocator_region_insert(&map, &a);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);

    printf("append\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 6;
    b.size = 4;
    b.flags = 0;
    allocator_region_insert(&map, &a);
    allocator_region_insert(&map, &b);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 6);
    assert(map.regions->next->size == 4);

    printf("lhs\n");
    allocator_map_init(&map);
    a.address = 6;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    allocator_region_insert(&map, &a);
    allocator_region_insert(&map, &b);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 6);
    assert(map.regions->next->size == 4);

    printf("rhs\n");
    allocator_map_init(&map);
    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 10;
    b.size = 4;
    b.flags = 0;
    c.address = 6;
    c.size = 3;
    c.flags = 0;
    allocator_region_insert(&map, &a);
    allocator_region_insert(&map, &b);
    allocator_region_insert(&map, &c);
    assert(map.regions);
    assert(map.regions->address == 1);
    assert(map.regions->size == 4);
    assert(map.regions->next);
    assert(map.regions->next->address == 6);
    assert(map.regions->next->size == 3);
    assert(map.regions->next->next);
    assert(map.regions->next->next->address == 10);
    assert(map.regions->next->next->size == 4);

    return 0;

}

