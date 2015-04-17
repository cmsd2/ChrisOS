#ifndef VM_SPACE_H
#define VM_SPACE_H

#include <mm/allocator.h>
#include <mm/vm_types.h>
#include <mm/vm_map.h>
#include <mm/pm_map.h>

struct page_directory;

// the virtual address space of a process
struct vm_space {
    struct allocator_map vm_alloc_map; // available virtual mem
    struct vm_map vm_map; // used vm areas
    struct pm_map pm_map; // used pm areas
    struct page_directory * page_directory;
};

void vm_space_init(struct vm_space *vms, struct page_directory * pd);
struct vm_space * vm_space_clone(struct vm_space * vms);

#endif
