#ifndef MM_TYPES_H
#define MM_TYPES_H

#include <stdint.h>
#include <stddef.h>

#define MAX_VA ((uintptr_t)-1)

typedef uintptr_t mm_ptr_t;
typedef size_t mm_size_t;

typedef uintptr_t vm_ptr_t;
typedef size_t vm_size_t;

// will change if using PAE
typedef uintptr_t pm_ptr_t;
typedef size_t pm_size_t;

#endif
