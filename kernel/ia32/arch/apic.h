#ifndef IA32_ARCH_APIC_H
#define IA32_ARCH_APIC_H

#include <stdbool.h>
#include <stdint.h>
#include <arch/cpuid.h>

#define APIC_BASE_MSR_BSP 0x100
#define APIC_BASE_MSR_X2APIC 0x400
#define APIC_BASE_MSR_GLOBAL_ENABLE 0x800

// by default apic registers mmap'd to physical page starting here
#define APIC_BASE_DEFAULT 0xFEE00000

/* offsets into the local api register address map
 * each register is composed of 1 or more 32 bit values,
 * each value being 128 bit aligned
 * registers are between 32 and 256 bits wide.
 * accesses must use individual 32 bit wide loads and stores
 */
#define APIC_REG_ID 0x20 // apic id reg
#define APIC_REG_VERSION 0x30 // apic version reg
#define APIC_REG_TPR 0x80 // task priority reg
#define APIC_REG_APR 0x90 // arbitration priority reg
#define APIC_REG_PPR 0xA0 // processor priority reg
#define APIC_REG_EOI 0xB0 // End-Of-Interrupt reg
#define APIC_REG_RRD 0xC0 // remote read reg
#define APIC_REG_LDR 0xD0 // logical destination reg
#define APIC_REG_DFR 0xE0 // destination format reg
#define APIC_REG_SIVR 0xF0 // spurious interrupt vector reg
#define APIC_REG_ISR_BASE 0x100 // in-service reg. 256 bits
#define APIC_REG_TMR_BASE 0x180 // trigger mode reg. 256 bits
#define APIC_REG_IRR_BASE 0x200 // interrupt request reg. 256 bits
#define APIC_REG_ESR 0x280 // error status reg
#define APIC_REG_LVT_CMCI 0x2F0 // cmci
#define APIC_REG_ICR_LOW 0x300 // interrupt command reg. (bits 0-31)
#define APIC_REG_ICR_HIGH 0x310 // interrupt command reg. (bits 32-63)
#define APIC_REG_LVT_TIMER 0x320 // local vector table: timer
#define APIC_REG_LVT_TSR 0x330 // local vector table thermal sensor reg
#define APIC_REG_LVT_PERF_MCR 0x340 // local vector table performance mcr
#define APIC_REG_LVT_LINT0 0x350 // local vector table lint0 pin reg
#define APIC_REG_LVT_LINT1 0x360 // local vector table lint1 pin reg
#define APIC_REG_LVT_ERROR 0x370 // local vector table error reg
#define APIC_REG_TIMER_ICR 0x380 // timer initial count reg
#define APIC_REG_TIMER_CCR 0x390 // timer current count reg
#define APIC_REG_TIMER_DCR 0x3E0 // timer divide config reg

union apic_lvt {
    unsigned int value;
    struct {
        unsigned int vector : 8;
        unsigned int reserved_8_11 : 4;
        unsigned int delivery_status : 1;
        unsigned int input_pin_polarity : 1;
        unsigned int remote_irr : 1;
        unsigned int trigger_mode : 1;
        unsigned int mask : 1;
        unsigned int timer_mode : 2;
        unsigned int reserved_19_31 : 13;
    } fields;
};

// ia32 10.5. fig 10-8. lvt timer reg bits 18:17
enum apic_timer_mode {
    APIC_TIMER_ONE_SHOT = 0,
    APIC_TIMER_PERIODIC = 0x20000,
    APIC_TIMER_TSC_DEADLINE = 0x40000,

    // field bitmask
    APIC_TIMER_MODE_MASK = 0x60000
};

enum apic_lvt_delivery_mode {
    APIC_LVT_FIXED = 0,
    APIC_LVT_SMI = 0x200,
    APIC_LVT_NMI = 0x400,
    APIC_LVT_INIT = 0x500,
    APIC_LVT_EXT_INT = 0x700,

    // field mask
    APIC_LVT_DELIVERY_MODE_MASK = 0x700
};

enum apic_lvt_delivery_status {
    APIC_LVT_IDLE = 0,
    APIC_LVT_SEND_PENDING = 0x1000,

    // field mask
    APIC_LVT_DELIVERY_STATUS_MASK = 0x1000
};

enum apic_lvt_interrupt_input_polarity {
    APIC_LVT_ACTIVE_HIGH = 0,
    APIC_LVT_ACTIVE_LOW = 0x2000,

    // field mask
    APIC_LVT_POLARITY_MASK = 0x2000
};

enum apic_lvt_remote_read {
    APIC_LVT_IRR_OFF = 0,
    APIC_LVT_IRR_ON = 0x4000
};

enum apic_lvt_trigger_mode {
    APIC_LVT_EDGE_TRIGGERED = 0,
    APIC_LVT_LEVEL_TRIGGERED = 0x8000
};

enum apic_lvt_mask {
    APIC_LVT_UNMASKED = 0,
    APIC_LVT_MASKED = 0x10000
};

struct apic_base_msr {
	union {
		unsigned int value;
		struct {
			unsigned int reserved_0_7 : 8;
			unsigned int bsp : 1; // bit 8
			unsigned int reserved_9 : 1; // bit 9
			unsigned int x2apic_enable : 1; // bit 10
			unsigned int global_enable : 1; // bit 11
			unsigned int apic_base_low_part : 20;
		} fields;
	} low;
	union {
		unsigned int value;
		struct {
			unsigned int apic_base_high;
		} fields;
	} high;
};

union apic_sivr {
	unsigned int value;
	struct {
		unsigned int spurious_vector : 8;
		unsigned int software_enable : 1;
		unsigned int focus_proc_checking : 1;
		unsigned int reserved_10_11 : 2;
		unsigned int eoi_broadcast_supress : 1;
		unsigned int reserved_13_31 : 19;
	} fields;
};

void apic_init(const struct cpuid_info * cpu);
void apic_timer_init();
bool apic_available(const struct cpuid_info * cpu);
void apic_print_base_msr(const struct apic_base_msr * msr);
uint8_t apic_current_cpu_apic_id(void);
void apic_read_msr(const struct cpuid_info * cpu, struct apic_base_msr * msr);
void apic_write_msr(const struct apic_base_msr * msr);
void apic_get_base(const struct apic_base_msr * msr, unsigned int * low, unsigned int * high);
void apic_set_base(struct apic_base_msr * msr, uint32_t low, uint32_t high);
void apic_global_enable(const struct cpuid_info * cpu);
uintptr_t apic_phys_to_linear_addr(unsigned int low, unsigned int high);
void apic_read_reg_32(unsigned int reg, unsigned int * value);
void apic_write_reg_32(unsigned int reg, unsigned int value);
void apic_sivr_enable();
void apic_sivr_disable();
const struct apic_base_msr * apic_get_base_msr();
void apic_lvt_write(unsigned int reg, const union apic_lvt * lvt);
void apic_lvt_read(unsigned int reg, union apic_lvt * lvt);
void apic_eoi();

#endif
