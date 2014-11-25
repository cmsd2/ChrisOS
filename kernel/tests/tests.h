#ifndef TESTS_H
#define TESTS_H

#if __STDC_HOSTED__ == 0
#include <utils/kprintf.h>
#define printf kprintf
#endif

int test_all(void);
int test_allocator_region_add_test(void);
int test_allocator_region_cut_test(void);
int test_allocator_region_insert_test(void);
int test_allocator_region_merge_test(void);
int test_allocator_region_subtract_test(void);
int test_apic_macros(void);
int test_bits(void);

#endif
