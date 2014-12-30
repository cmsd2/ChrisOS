#include <arch/registers.h>
#include <utils/kprintf.h>
#include <sys/param.h>

flags_reg_t get_flags_register() {
    uint32_t eflags;
    __asm__("pushfl\n\t"
            "popl %0"
            : "=r"(eflags));
    return eflags;
}

uint16_t get_cs_register() {
    uint16_t cs;
    __asm__("movw %%cs, %0"
            : "=r"(cs));
    return cs;
}

flags_reg_t flags_register_value_clear_bit(flags_reg_t value, enum flags_register_bit_sel bit_sel) {
    clrbit(&value, bit_sel);
    return value;
}

flags_reg_t flags_register_value_set_bit(flags_reg_t value, enum flags_register_bit_sel bit_sel) {
    setbit(&value, bit_sel);
    return value;
}

bool flags_register_value_is_set(flags_reg_t value, enum flags_register_bit_sel bit_sel) {
    return isset(&value, bit_sel);
}

bool flags_register_is_set(enum flags_register_bit_sel bit_sel) {
    flags_reg_t flags = get_flags_register();
    return flags_register_value_is_set(flags, bit_sel);
}

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
