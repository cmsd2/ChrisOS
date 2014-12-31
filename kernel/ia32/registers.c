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

cr_reg_t get_cr0_register(void) {
    cr_reg_t result;
    __asm__("movl %%cr0, %0" : "=r"(result));
    return result;
}

void set_cr0_register(cr_reg_t cr0) {
    __asm__("movl %0, %%cr0" : : "r"(cr0));
}

cr_reg_t cr0_register_value_clear_bit(cr_reg_t value, enum cr0_register_bit_sel bit_sel) {
    clrbit(&value, bit_sel);
    return value;
}

cr_reg_t cr0_register_value_set_bit(cr_reg_t value, enum cr0_register_bit_sel bit_sel) {
    setbit(&value, bit_sel);
    return value;
}

bool cr0_register_value_is_set(cr_reg_t value, enum cr0_register_bit_sel bit_sel) {
    return 1 == isset(&value, bit_sel);
}

bool cr0_register_is_set(enum cr0_register_bit_sel bit_sel) {
    cr_reg_t cr0 = get_cr0_register();
    return cr0_register_value_is_set(cr0, bit_sel);
}

cr_reg_t get_cr4_register(void) {
    cr_reg_t result;
    __asm__("movl %%cr4, %0" : "=r"(result));
    return result;
}

void set_cr4_register(cr_reg_t cr0) {
    __asm__("movl %0, %%cr4" : : "r"(cr0));
}

cr_reg_t cr4_register_value_clear_bit(cr_reg_t value, enum cr4_register_bit_sel bit_sel) {
    clrbit(&value, bit_sel);
    return value;
}

cr_reg_t cr4_register_value_set_bit(cr_reg_t value, enum cr4_register_bit_sel bit_sel) {
    setbit(&value, bit_sel);
    return value;
}

bool cr4_register_value_is_set(cr_reg_t value, enum cr4_register_bit_sel bit_sel) {
    return 1 == isset(&value, bit_sel);
}

bool cr4_register_is_set(enum cr4_register_bit_sel bit_sel) {
    cr_reg_t cr4 = get_cr4_register();
    return cr4_register_value_is_set(cr4, bit_sel);
}
