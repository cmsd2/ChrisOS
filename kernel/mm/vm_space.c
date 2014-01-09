#include <mm/vm_space.h>
#include <mm/vm_map.h>

void vm_space_init(struct vm_space *vms) {
    vm_map_init(&vms->vm_map);
    allocator_map_init(&vms->vm_alloc_map);
}
