#include <assert.h>
#include <tests/tests.h>

#include "mm/allocator.h"

int test_allocator_region_merge_test(void) {
    struct allocator_region a, b;
    bool ret;

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 1);
    assert(b.size = 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 2;
    b.flags = 0;
    ret = allocator_region_merge(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 3);
    assert(b.size == 2);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 6);
    assert(b.address == 3);
    assert(b.size == 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 8);
    assert(b.address == 5);
    assert(b.size == 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 6;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&a, &b);
    assert(!ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 6);
    assert(b.size == 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 2;
    b.flags = 0;
    ret = allocator_region_merge(&b, &a);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 1);
    assert(b.size == 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&b, &a);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 1);
    assert(b.size == 6);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&b, &a);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 1);
    assert(b.size == 8);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 6;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_merge(&b, &a);
    assert(!ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 6);
    assert(b.size == 4);

    return 0;
}
