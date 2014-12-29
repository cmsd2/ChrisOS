#include <arch/registers.h>
#include <utils/kprintf.h>

void registers_dump(struct registers * regs) {
    kprintf("ds  = 0x%x\n", regs->ds);
    kprintf("cr2 = 0x%x\n", regs->cr2);
    kprintf("edi = 0x%x\n", regs->edi);
    kprintf("esi = 0x%x\n", regs->esi);
    kprintf("ebp = 0x%x\n", regs->ebp);
    kprintf("esp = 0x%x\n", regs->esp);
    kprintf("ebx = 0x%x\n", regs->ebx);
    kprintf("edx = 0x%x\n", regs->edx);
    kprintf("ecx = 0x%x\n", regs->ecx);
    kprintf("eax = 0x%x\n", regs->eax);
    kprintf("int = 0x%x\n", regs->int_no);
    kprintf("err = 0x%x\n", regs->err_code);
    kprintf("eip = 0x%x\n", regs->eip);
    kprintf("cs  = 0x%x\n", regs->cs);
    kprintf("efl = 0x%x\n", regs->eflags);
    kprintf("esp = 0x%x\n", regs->useresp);
    kprintf("ss  = 0x%x\n", regs->ss);
}
