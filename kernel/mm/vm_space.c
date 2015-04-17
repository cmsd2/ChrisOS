#include <mm/vm_space.h>
#include <mm/vm_map.h>
#include <arch/paging.h>
#include <mm/malloc.h>

void vm_space_init(struct vm_space *vms, struct page_directory * pd) {
    vm_map_init(&vms->vm_map);
    allocator_map_init(&vms->vm_alloc_map);

    vms->page_directory = pd;
}

struct vm_space * vm_space_clone(struct vm_space * parent) {
    struct vm_space * child = (struct vm_space *)malloc(sizeof(struct vm_space));

    if(!child) {
        return 0;
    }
    
    child->page_directory = paging_pd_clone(parent->page_directory);
    if(!child->page_directory) {
        free(child);
        return 0;
    }

    vm_map_init_copy(&child->vm_map, &parent->vm_map);
    allocator_map_init_copy(&child->vm_alloc_map, &parent->vm_alloc_map);

    return child;
}
