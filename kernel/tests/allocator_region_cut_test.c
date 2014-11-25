#include <assert.h>
#include <tests/tests.h>

#include "mm/allocator.h"

int test_allocator_region_cut_test(void) {
    struct allocator_region a, b;
    bool ret;

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(ret);
    assert(a.size == 0);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 2;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 2);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 2);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(!ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 5);
    assert(b.size == 4);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 6;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(!ret);
    assert(a.address == 1);
    assert(a.size == 4);
    assert(b.address == 6);
    assert(b.size = 4);

    a.address = 3;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 11;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    assert(ret);
    assert(a.address == 3);
    assert(a.size == 0);
    assert(b.address == 1);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 1;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&b, &a);
    assert(ret);
    assert(a.size == 0);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 2;
    b.flags = 0;
    ret = allocator_region_cut(&b, &a);
    assert(ret);
    assert(a.size == 0);
    assert(b.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 3;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&b, &a);
    assert(ret);
    assert(b.address == 5);
    assert(b.size == 2);
    assert(a.size == 0);

    a.address = 1;
    a.size = 4;
    a.flags = 0;
    b.address = 5;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&b, &a);
    assert(ret);
    assert(b.address == 5);
    assert(b.size == 4);
    assert(a.size == 0);

    printf("cut middle\n");
    a.address = 1;
    a.size = 11;
    a.flags = 0;
    b.address = 3;
    b.size = 4;
    b.flags = 0;
    ret = allocator_region_cut(&a, &b);
    printf("baddr %ld bsize %ld\n", b.address, b.size);
    assert(ret);
    assert(a.address == 1);
    assert(a.size == 2);
    assert(b.address == 7);
    assert(b.size == 5);

    return 0;
}
