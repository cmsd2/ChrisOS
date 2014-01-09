#ifndef PM_MAP_H
#define PM_MAP_H

#include <mm/vm_types.h>
#include <mm/allocator.h>

struct vm_map_region;

struct pm_page_ref {
    struct pm_page_ref *next;
    struct vm_map_region *region;
};

// physical page, in use by one or more processes
struct pm_page {
    pm_ptr_t address;
    struct pm_page_ref *refs;
};

struct pm_map_region {
    struct pm_map_region *next;
    pm_ptr_t address;
    pm_size_t size;
    struct pm_page *pages;
};

struct pm_map {
    struct pm_map_region *regions;
};

#endif
