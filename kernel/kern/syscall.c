#include <stdbool.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/hal.h>
#include <utils/panic.h>
#include <arch/registers.h>
#include <arch/power.h>
#include <arch/interrupts.h>
#include <utils/kprintf.h>
#include <drivers/tty.h>

long high_five(long a, long b, long c, long d, long e);

static void * syscall_handlers[] = {
    power_halt, // 0
    high_five, // 1
    tty_getc, // 2
    tty_putc, // 3
    tty_gets, // 4
    tty_puts // 5
};

long high_five(long a, long b, long c, long d, long e) {
    return a + b + c + d + e;
}

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
    //kprintf("calling syscall %d\n", num);
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
DEFN_SYSCALL_5(high_five, 1, long, long, long, long, long);
DEFN_SYSCALL_0(tty_getc, 2);
DEFN_SYSCALL_1(tty_putc, 3, char);
DEFN_SYSCALL_2(tty_gets, 4, char *, size_t);
DEFN_SYSCALL_1(tty_puts, 5, const char *);
