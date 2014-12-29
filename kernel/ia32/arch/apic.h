#ifndef IA32_ARCH_APIC_H
#define IA32_ARCH_APIC_H

#include <stdbool.h>
#include <stdint.h>
#include <arch/cpuid.h>
#include <sys/param.h>

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
 *
 * ia32 vol3 table 10-1
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

typedef unsigned int apic_lvt_t;

#define APIC_LVT_GET_VECTOR(lvt) getbits(lvt, 0, 7)
#define APIC_LVT_GET_DELIVERY_MODE(lvt) getbits(lvt, 8, 10)
#define APIC_LVT_GET_DELIVERY_STATUS(lvt) getbit(lvt, 12)
#define APIC_LVT_GET_INPUT_PIN_POLARITY(lvt) getbit(lvt, 13)
#define APIC_LVT_GET_REMOTE_IRR(lvt) getbit(lvt, 14)
#define APIC_LVT_GET_TRIGGER_MODE(lvt) getbit(lvt, 15)
#define APIC_LVT_GET_MASK(lvt) getbit(lvt, 16)
#define APIC_LVT_GET_TIMER_MODE(lvt) getbits(lvt, 17, 18)
#define APIC_LVT_SET_VECTOR(lvt, value) withbits(lvt, 0, 7, value)
#define APIC_LVT_SET_DELIVERY_MODE(lvt, value) withbits(lvt, 8, 10, value)
#define APIC_LVT_SET_DELIVERY_STATUS(lvt, value) withbit(lvt, 12, value)
#define APIC_LVT_SET_INPUT_PIN_POLARITY(lvt, value) withbit(lvt, 13, value)
#define APIC_LVT_SET_REMOTE_IRR(lvt, value) withbit(lvt, 14, value)
#define APIC_LVT_SET_TRIGGER_MODE(lvt, value) withbit(lvt, 15, value)
#define APIC_LVT_SET_MASK(lvt, value) withbit(lvt, 16, value)
#define APIC_LVT_SET_TIMER_MODE(lvt, value) withbits(lvt, 17, 18, value)

#define APIC_LVT_PERF_GET_NMI //TODO what's this?

// ia32 vol3 10.5. fig 10-8. lvt timer reg bits 18:17
enum apic_timer_mode {
    apic_timer_one_shot = 0,
    apic_timer_periodic = 2,
    apic_timer_tsc_deadline = 4,
};

enum apic_lvt_delivery_mode {
    apic_lvt_delivery_fixed = 0,
    apic_lvt_delivery_smi = 2,
    apic_lvt_delivery_nmi = 4,
    apic_lvt_delivery_init = 5,
    apic_lvt_delivery_ext_int = 7,
};

enum apic_lvt_delivery_status {
    apic_lvt_delivery_status_idle = 0,
    apic_lvt_delivery_status_send_pending = 1,
};

enum apic_lvt_interrupt_input_polarity {
    apic_lvt_interrupt_input_polarity_active_high = 0,
    apic_lvt_interrupt_input_polarity_active_low = 2,
};

enum apic_lvt_remote_read {
    apic_lvt_remote_read_off = 0,
    apic_lvt_remote_read_on = 4,
};

enum apic_lvt_trigger_mode {
    apic_lvt_edge_triggered = 0,
    apic_lvt_level_triggered = 8,
};

enum apic_lvt_mask {
    apic_lvt_unmasked = 0,
    apic_lvt_masked = 1,
};

// ia32 vol3 fig 10-26.
struct apic_base_msr {
    unsigned int low;
    unsigned int high;
};

struct ioapic {
    uint8_t id;
    uint32_t phys_address;
    uintptr_t address;
    int irq_count;
    uint32_t irq_base;
    struct ioapic * next;
};

#define APIC_MSR_LOW_GET_BSP(msr) getbit(msr, 8)
#define APIC_MSR_LOW_GET_X2APIC_ENABLE(msr) getbit(msr, 10)
#define APIC_MSR_LOW_GET_GLOBAL_ENABLE(msr) getbit(msr, 11)
#define APIC_MSR_LOW_GET_BASE(msr) withbits(0, 0, 19, getbits(msr, 12, 31))
#define APIC_MSR_HIGH_GET_BASE(msr) withbits(0, 20, 23, getbits(msr, 0, 3))
#define APIC_MSR_GET_BASE(msr_high, msr_low) (APIC_MSR_LOW_GET_BASE(msr_low) | APIC_MSR_HIGH_GET_BASE(msr_high))
#define APIC_MSR_LOW_SET_BSP(msr, value) withbit(msr, 8, value)
#define APIC_MSR_LOW_SET_X2APIC_ENABLE(msr, value) withbit(msr, 10, value)
#define APIC_MSR_LOW_SET_GLOBAL_ENABLE(msr, value) withbit(msr, 11, value)
#define APIC_MSR_LOW_SET_BASE(msr, value) withbits(msr, 12, 31, getbits(value, 0, 19))
#define APIC_MSR_HIGH_SET_BASE(msr, value) withbits(msr, 0, 3, getbits(value, 20, 23))

// ia32 vol3 fig 10-23. spurious interrupt vector register
typedef unsigned int apic_svr_t;
#define APIC_SVR_GET_VECTOR(svr) getbits(svr, 0, 7)
#define APIC_SVR_GET_SOFTWARE_ENABLE(svr) getbit(svr, 8)
#define APIC_SVR_GET_FOCUS_PROC_CHECKING(svr) getbit(svr, 9)
#define APIC_SVR_GET_EOI_BROADCAST_SUPRESS(svr) getbit(svr, 12)
#define APIC_SVR_SET_VECTOR(svr, value) withbits(svr, 0, 7, value)
#define APIC_SVR_SET_SOFTWARE_ENABLE(svr, value) withbit(svr, 8, value)
#define APIC_SVR_SET_FOCUS_PROC_CHECKING(svr, value) withbit(svr, 9, value)
#define APIC_SVR_SET_EOI_BROADCAST_SUPRESS(svr, value) withbit(svr, 12, value)

#define APIC_SIVR 0x37

// use ioapic to map interrupts 0 to 23 to the range 0x40 to 0x57
#define APIC_IRQ_BASE (APIC_SIVR + 1)

#define IOAPIC_REGSEL_ID 0x0
#define IOAPIC_REGSEL_VER 0x1
#define IOAPIC_REGSEL_ARB_ID 0x2

// 24 entries, one for each irq line
// each entry composed of 2 consecutive 32bit registers
#define IOAPIC_REGSEL_REDIRECT_TBL_BASE 0x10
#define IOAPIC_REGSEL_REDIRECT_TBL_END 0x3f

#define IOAPIC_REG_ID_GET_ID(value) getbits(value, 24, 27)
#define IOAPIC_REG_VER_GET_VER(value) getbits(value, 0, 7)

// equal to number of irq lines - 1. should be 23
#define IOAPIC_REG_VER_GET_MAX_REDIRECT(value) getbits(value, 16, 23)

#define IOAPIC_REG_ARB_GET_ARB_ID(value) getbits(value, 24, 27)

#define IOAPIC_IOREGSEL(apic_base) ((uint32_t*)apic_base)
#define IOAPIC_IOWIN(apic_base) ((uint32_t*)(apic_base | 0x10))

// bit 16
enum ioapic_redirect_mask {
    ioapic_redirect_unmasked = 0,
    ioapic_redirect_masked = 1
};

// bit 15
enum ioapic_trigger_mode {
    ioapic_trigger_edge = 0,
    ioapic_trigger_level = 1
};

// bit 14. readonly. level triggered only
enum ioapic_remote_irr {
    ioapic_remote_lapic_eoi = 0, // lapic has issued eoi
    ioapic_remote_lapic_delivered = 1 // lapic has accepted interrupt
};

// bit 13
enum ioapic_polarity {
    ioapic_active_high = 0,
    ioapic_active_low = 1
};

// bit 12
enum ioapic_delivery_status {
    ioapic_delivery_idle = 0,
    ioapic_delivery_pending = 1 // waiting to deliver to lapic
};

// bit 11
enum ioapic_destination_mode {
    ioapic_dest_physical_mode = 0, // set apic id in high bits of redirection entry
    ioapic_dest_logical_mode = 1 // set bitmask of processors in high bits
};

// bits 8:10
enum ioapic_delivery_mode {
    ioapic_delivery_fixed = 0, // deliver to INTR pin of all matching destination cores
    ioapic_delivery_lowest_priority = 1, // deliver to INTR pin of 1 of the matching destination cores with lowest priority
    ioapic_delivery_smi = 2, // use edge trigger mode. interrupt vector must be 0.
    ioapic_delivery_reserved_3 = 3,
    ioapic_delivery_nmi = 4, // use edge trigger mode. vector ignored.
    ioapic_delivery_init = 5, // use edge trigger mode.
    ioapic_delivery_reserved_6 = 6,
    ioapic_delivery_extint = 7 // use edge trigger mode. route via external int controller which will supply the vector.
};

enum ioapic_vector {
    ioapic_vector_min = 0x10,
    ioapic_vector_max = 0xfe
};

#define IOAPIC_RED_GET_DESTINATION(upper) getbits(upper, 24, 31)
#define IOAPIC_RED_GET_MASK(value) getbit(value, 16)
#define IOAPIC_RED_GET_TRIGGER_MODE(value) getbit(value, 15)
#define IOAPIC_RED_GET_REMOTE_IRR(value) getbit(value, 14)
#define IOAPIC_RED_GET_POLARITY(value) getbit(value, 13)
#define IOAPIC_RED_GET_DELIVERY_STATUS(value) getbit(value, 12)
#define IOAPIC_RED_GET_DEST_MODE(value) getbit(value, 11)
#define IOAPIC_RED_GET_DELIVERY_MODE(value) getbits(value, 8, 10)
#define IOAPIC_RED_GET_INTERRUPT_VECTOR(value) getbits(value, 0, 7)

#define IOAPIC_RED_SET_DESTINATION(upper, value) withbits(upper, 24, 31, value)
#define IOAPIC_RED_SET_MASK(entry, value) withbit(entry, 16, value)
#define IOAPIC_RED_SET_TRIGGER_MODE(entry, value) withbit(entry, 15, value)
#define IOAPIC_RED_SET_REMOTE_IRR(entry, value) withbit(entry, 14, value)
#define IOAPIC_RED_SET_POLARITY(entry, value) withbit(entry, 13, value)
#define IOAPIC_RED_SET_DELIVERY_STATUS(entry, value) withbit(entry, 12, value)
#define IOAPIC_RED_SET_DEST_MODE(entry, value) withbit(entry, 11, value)
#define IOAPIC_RED_SET_DELIVERY_MODE(entry, value) withbits(entry, 8, 10, value)
#define IOAPIC_RED_SET_INTERRUPT_VECTOR(entry, value) withbits(entry, 0, 7, value)

void apic_init(const struct cpuid_info * cpu);
void apic_timer_init();
uint32_t apic_measure_timer_freq();
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
void apic_remap_irqs(void);
uint8_t apic_interrupt_for_irq(uint8_t irq);
const struct apic_base_msr * apic_get_base_msr();
void apic_lvt_write(unsigned int reg, apic_lvt_t lvt);
void apic_lvt_read(unsigned int reg, apic_lvt_t * lvt);
void apic_eoi();
void apic_timer_disable_interrupt();
void apic_timer_enable_interrupt(uint8_t int_no);
void apic_timer_set_initial_count(unsigned int start_count);
void apic_timer_set_divider(unsigned int div);
unsigned int apic_timer_get_current_count();

void apic_configure_lapic(uint8_t id, uint8_t processor_id, uint32_t lapic_flags);
void apic_configure_ioapic(uint8_t id, uint32_t phys_address, uint32_t global_irq_base);
void apic_configure_int_override(uint8_t bus, uint8_t source_irq,
                            uint32_t global_irq, uint16_t inti_flags);
void apic_configure_nmi_source(uint32_t global_irq, uint16_t inti_flags);
void apic_configure_lapic_nmi(uint8_t processor_id, uint8_t lint, uint16_t inti_flags);
void apic_configure_lapic_override(uint64_t phys_address);

void ioapic_print_redirection_table(struct ioapic * i);
struct ioapic * ioapic_alloc(void);
void ioapic_add(struct ioapic * i);
struct ioapic * ioapic_for_irq(uint32_t irq);
int ioapic_compare_irq_base(struct ioapic * a, struct ioapic * b);

void ioapic_remap_irqs();

uint32_t ioapic_read_reg(struct ioapic * i, int regsel);
int ioapic_get_max_redirect(struct ioapic * i);
void ioapic_write_reg(struct ioapic * i, int regsel, uint32_t value);
void ioapic_read_redirection_entry(struct ioapic * i, int index,
                                   uint32_t * lower, uint32_t * upper);
void ioapic_write_redirection_entry(struct ioapic * i, int index,
                                    uint32_t lower, uint32_t upper);

int ioapic_interrupt_for_irq(uint32_t irq);
void ioapic_set_redirection(struct ioapic * i, int index,
                            uint8_t destination,
                            enum ioapic_redirect_mask masked,
                            enum ioapic_trigger_mode trigger_mode,
                            enum ioapic_polarity polarity,
                            enum ioapic_destination_mode dest_mode,
                            enum ioapic_delivery_mode delivery_mode,
                            int interrupt_vector
                            );
void ioapic_setup_irq(uint32_t irq, uint8_t destination,
                    enum ioapic_redirect_mask masked,
                    enum ioapic_trigger_mode trigger_mode,
                    enum ioapic_polarity polarity,
                    enum ioapic_destination_mode dest_mode,
                    enum ioapic_delivery_mode delivery_mode,
                    int interrupt_vector);
void ioapic_mask_irq(uint32_t irq);
void ioapic_unmask_irq(uint32_t irq);

#endif
