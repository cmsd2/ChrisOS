#ifndef KERNEL_UTILS_SORT_H
#define KERNEL_UTILS_SORT_H

#include <stdint.h>
#include <stddef.h>

typedef int (*comp_fn)(void *a, void *b);

int comp_int(void *a, void *b);
int comp_uint(void *a, void *b);
int comp_ulong(void *a, void *b);
void swap(void ** items, size_t n1, size_t n2);
void bubble_sort(void ** items, size_t length, comp_fn comp);
void insertion_sort(void ** items, size_t length, comp_fn comp);

#endif
