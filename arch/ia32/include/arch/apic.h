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
#define APIC_REG_ID 0x20
#define APIC_REG_VERSION 0x30
#define APIC_REG_TPR 0x80
#define APIC_REG_APR 0x90
#define APIC_REG_PPR 0xA0
#define APIC_REG_EOI 0xB0
#define APIC_REG_RRD 0xC0
#define APIC_REG_LDR 0xD0
#define APIC_REG_DFR 0xE0
#define APIC_REG_SIVR 0xF0
#define APIC_REG_ISR_BASE 0x100 // 256 bits
#define APIC_REG_TMR_BASE 0x180 // 256 bits
#define APIC_REG_IRR_BASE 0x200 // 256 bits
#define APIC_REG_ESR 0x280
#define APIC_REG_LVT_CMCI 0x2F0
#define APIC_REG_ICR_LOW 0x300
#define APIC_REG_ICR_HIGH 0x310
#define APIC_REG_LVT_TIMER 0x320
#define APIC_REG_LVT_TSR 0x330
#define APIC_REG_LVT_PERF_MCR 0x340
#define APIC_REG_LVT_LINT0 0x350
#define APIC_REG_LVT_LINT1 0x360
#define APIC_REG_LVT_ERROR 0x370
#define APIC_REG_TIMER_ICR 0x380
#define APIC_REG_TIMER_CCR 0x390
#define APIC_REG_TIMER_DCR 0x3E0

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

void apic_init(const struct cpuid_info * cpu);
bool apic_available(const struct cpuid_info * cpu);
void apic_print_base_msr(const struct apic_base_msr * msr);
uint8_t apic_current_cpu_apic_id(void);
void apic_read_msr(const struct cpuid_info * cpu, struct apic_base_msr * msr);
void apic_write_msr(const struct apic_base_msr * msr);
void apic_get_base(const struct apic_base_msr * msr, uint32_t * low, uint32_t * high);
void apic_set_base(struct apic_base_msr * msr, uint32_t low, uint32_t high);

#endif
