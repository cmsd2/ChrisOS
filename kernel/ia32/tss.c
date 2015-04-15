#include <arch/tss.h>
#include <arch/gdt.h>
#include <sys/thread.h>
#include <assert.h>

void tss_set_stack_for_current_thread() {
    assert(current_thread());
    tss_set_stack(current_thread()->kernel_stack + current_thread()->kernel_stack_size);
}
