#ifndef IA32_ARCH_REGISTERS_H
#define IA32_ARCH_REGISTERS_H

#include <stdint.h>
#include <stdbool.h>

// order of registers matches stack layout in our interrupt handler
struct registers
{
    // segment and paging regs
    uint32_t ds, cr2;

    // same order as pusha
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    // saved by our interrupt service routine
    uint32_t int_no;

    // cpu's interrupt setup
    uint32_t err_code;
    uint32_t eip, cs, eflags;

    // only if privilege change i.e. from ring 3 to 0:
    uint32_t useresp, ss;

};

// stack layout for using iret between kernel threads only.
// push args then eflags then use a far call.
struct iret_stack_frame {
    uint32_t ebp;
    uint32_t eip;
    uint32_t cs; // 16-bit reg padded to 32 bits
    uint32_t eflags;

    void * args[0];
};

struct stack_frame {
    // after "push %ebp; mov %esp, %ebp":
    uint32_t ebp; // 0(%ebp)
    uint32_t eip; // 4(%ebp)

    // args here
    void * args[0];
};

// stack.S relies on this layout
struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;

    struct iret_stack_frame frame;
};

typedef uint32_t flags_reg_t;

#define push_flags_register() __asm__("pushfl")
#define pop_flags_register() __asm__("popfl")

flags_reg_t get_flags_register(void);
uint16_t get_cs_register(void);

enum flags_register_bit_sel {
    flags_carry_bit = 0,
    flags_parity_bit = 2,
    flags_aux_carry_bit = 4,
    flags_zero_bit = 6,
    flags_sign_bit = 7,
    flags_trap_bit = 8,
    flags_if_bit = 9,
    flags_direction_Bit = 10,
    flags_overflow_bit = 11,
    flags_io_priv_bit_1 = 12,
    flags_io_priv_bit_2 = 13,
    flags_nt_bit = 14,
    flags_rf_bit = 16,
    flags_vm_bit = 17,
    flags_ac_bit = 18,
    flags_vif_bit = 19,
    flags_vip_bit = 20,
    flags_id_bit = 21
};

flags_reg_t flags_register_value_clear_bit(flags_reg_t value, enum flags_register_bit_sel bit_sel);
flags_reg_t flags_register_value_set_bit(flags_reg_t value, enum flags_register_bit_sel bit_sel);
bool flags_register_value_is_set(flags_reg_t value, enum flags_register_bit_sel bit_sel);

bool flags_register_is_set(enum flags_register_bit_sel bit_sel);

void registers_dump(struct registers * regs);

#endif
