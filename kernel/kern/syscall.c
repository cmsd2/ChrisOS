#include <stdbool.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/hal.h>
#include <utils/panic.h>
#include <arch/registers.h>
#include <arch/power.h>
#include <arch/interrupts.h>
#include <utils/kprintf.h>

static void * syscall_handlers[] = {
    power_halt, // 0
};

void syscalls_init() {
    interrupts_install_handler(0x80, syscalls_handler, 0);
}

int syscalls_num_syscalls() {
    return sizeof(syscall_handlers) / sizeof(void*);
}

bool syscalls_handler(uint32_t int_no, struct registers * regs, void * data) {
    if(regs->eax < syscalls_num_syscalls()) {
        syscalls_call(regs->eax, regs);
    } else {
        registers_dump(regs);
        panic("non-existant syscall");
    }
    return true;
}

void syscalls_call(uint32_t num, struct registers * regs) {
    kprintf("calling syscall %d\n", num);
    void * handler = syscall_handlers[num];

    uint32_t ret;

    __asm__ volatile("push %1  ;\n"
		     "push %2  ;\n"
		     "push %3  ;\n"
		     "push %4  ;\n"
		     "push %5  ;\n"
		     "call *%6 ;\n"
		     "addl $20, %%esp  ;\n"
		     : "=a" (ret)
		     : "r" (regs->edi),
		       "r" (regs->esi),
		       "r" (regs->edx),
		       "r" (regs->ecx),
		       "r" (regs->ebx),
		       "r" (handler)
		     );

    regs->eax = ret;
}

DEFN_SYSCALL_0(halt, 0);
