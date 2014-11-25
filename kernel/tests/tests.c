#include <tests/tests.h>
#include <assert.h>

int test_all(void) {
    assert(test_allocator_region_add_test() == 0);
    assert(test_allocator_region_cut_test() == 0);
    assert(test_allocator_region_insert_test() == 0);
    assert(test_allocator_region_merge_test() == 0);
    assert(test_allocator_region_subtract_test() == 0);
    assert(test_apic_macros() == 0);
    assert(test_bits() == 0);
    kprintf("tests passed\n");
    return 0;
}
