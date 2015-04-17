#include <sys/cdefs.h>
#include <arch/apic.h>
#include <arch/cpuid.h>
#include <arch/msrs.h>
#include <assert.h>
#include <utils/kprintf.h>
#include <arch/paging.h>
#include <utils/mem.h>
#include <arch/io_ports.h>
#include <sys/io.h>
#include <arch/pit.h>
#include <arch/ps2.h>
#include <arch/nmi.h>
#include <sys/standard.h>
#include <arch/interrupts.h>
#include <arch/pic.h>
#include <utils/sort.h>
#include <arch/acpi.h>
#include <utlist.h>

struct apic_base_msr _apic_base_msr;
uint32_t _apic_bus_freq;
struct ioapic * _ioapics;

//TODO: provide driver irq handling system here separate from interrupts own system
//then we can automatically call apic_eoi exactly once even for shared irqs
//and also take care of specifying correct delivery, destination, trigger modes etc?

// configures local apic using model-specific registers
// uses acpi to configure i/o apic and other stuff
void apic_init(const struct cpuid_info * cpu) {
    assert(apic_available(cpu));

    apic_read_msr(cpu, &_apic_base_msr);

    apic_print_base_msr(&_apic_base_msr);

    //TODO: base addr can be 36 or more bits with pae and/or long mode
    unsigned int low, high;
    apic_get_base(&_apic_base_msr, &low, &high);

    size_t size = align_padding((uintptr_t)low, 0x400000);

    paging_identity_map(paging_pd_current(), (uintptr_t) low, size, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_NOT_CACHEABLE);

    // i32 10.6.2.2:
    // flat model IPI desination 31:28 set 0xf, 28:0 reserved (all '1')
    apic_write_reg_32(APIC_REG_DFR, 0xffffffff);

    // logical apic ID 31:24 set to 8bit logical id, 23:0 reserved
    // todo: in multiprocessor system each cpu has different logical ID
    apic_write_reg_32(APIC_REG_LDR, 0x1 << 24);

    apic_write_reg_32(APIC_REG_LVT_TIMER, APIC_LVT_SET_MASK(0, apic_lvt_masked));
    apic_write_reg_32(APIC_REG_LVT_PERF_MCR, APIC_LVT_SET_DELIVERY_MODE(0, apic_lvt_delivery_nmi));
    apic_write_reg_32(APIC_REG_LVT_LINT0, APIC_LVT_SET_MASK(0, apic_lvt_masked));
    apic_write_reg_32(APIC_REG_LVT_LINT1, APIC_LVT_SET_MASK(0, apic_lvt_masked));
    apic_write_reg_32(APIC_REG_TPR, 0);

    apic_global_enable(cpu);

    apic_remap_irqs();

    // configure i/o apic and stuff
    acpi_configure_apic();

    apic_timer_init();
}

// currently only maps spurious interrupt and enables it
void apic_remap_irqs() {
    apic_svr_t sivr = 0;
    sivr = APIC_SVR_SET_SOFTWARE_ENABLE(sivr, 1);
    sivr = APIC_SVR_SET_VECTOR(sivr, APIC_SIVR);
    apic_write_reg_32(APIC_REG_SIVR, sivr);
}

uint8_t apic_interrupt_for_irq(uint8_t irq) {
    return irq + APIC_IRQ_BASE;
}

void apic_timer_init() {
    uint32_t freqs[3];
    for(int i = 0; i < 3; i++) {
        freqs[i] = apic_measure_timer_freq();
    }
    insertion_sort((void**)freqs, 3, comp_uint);
    for(int i = 0; i < 3; i++) {
        kprintf("apic bus freq %d = 0x%x\n", i, freqs[i]);
    }

    _apic_bus_freq = freqs[1];
    kprintf("apic bus freq = 0x%x\n", _apic_bus_freq);
}

// based on wiki.osdev.org/APIC_timer
uint32_t apic_measure_timer_freq() {
    volatile unsigned int tick = 0;
    unsigned int timer_div = 16;
    unsigned int test_sleep_freq = 10; // Hz
    unsigned int pit_freq = PIT_FREQUENCY; // Hz
    unsigned int test_sleep_time = pit_freq / test_sleep_freq;
    //kprintf("test sleep time is %d\n", test_sleep_time);

    unsigned int start_count = -1;

    apic_timer_set_divider(timer_div);

    pit_one_shot(pit_channel_2, test_sleep_time);
    apic_timer_set_initial_count(start_count);

    //wait until pic counter reaches 0
    nmi_timer_2_spinwait();

    unsigned int curcount;
    curcount = apic_timer_get_current_count();
    apic_timer_set_initial_count(0);

    unsigned int ticks = start_count - curcount;
    uint32_t cpu_bus_freq = ticks * timer_div * test_sleep_freq;
    //ticks_per_quantum = cpu_bus_freq / quantum / timer_div;
    //kprintf("done initialising timer. freq=%u\n", cpu_bus_freq);

    return cpu_bus_freq;
}

void apic_timer_set_initial_count(unsigned int start_count) {
    apic_write_reg_32(APIC_REG_TIMER_ICR, start_count);
}

void apic_timer_disable_interrupt() {
    apic_write_reg_32(APIC_REG_LVT_TIMER, APIC_LVT_SET_MASK(0, apic_lvt_masked));
}

void apic_timer_enable_interrupt(uint8_t int_no) {
    apic_write_reg_32(APIC_REG_LVT_TIMER, int_no);
}

unsigned int apic_timer_get_current_count() {
    unsigned int curcount;
    apic_read_reg_32(APIC_REG_TIMER_CCR, &curcount);
    return curcount;
}

void apic_timer_set_divider(unsigned int timer_div) {
    apic_write_reg_32(APIC_REG_TIMER_DCR, timer_div);
}

const struct apic_base_msr * apic_get_base_msr() {
    return &_apic_base_msr;
}

/*
 * can't be done after global disable, without reset.
 */
void apic_global_enable(const struct cpuid_info * cpu) {
    if(!APIC_MSR_LOW_GET_GLOBAL_ENABLE(_apic_base_msr.low) == 1) {
        _apic_base_msr.low = APIC_MSR_LOW_SET_GLOBAL_ENABLE(_apic_base_msr.low, 1);

        apic_write_msr(&_apic_base_msr);
        apic_read_msr(cpu, &_apic_base_msr);

        assert(APIC_MSR_LOW_GET_GLOBAL_ENABLE(_apic_base_msr.low) == 1);
    }
}

uintptr_t apic_phys_to_linear_addr(unsigned int low, unsigned int __unused high) {
    // TODO:
    // convert 36bit or more physical address to 32 bit linear address
    // without pae, it should only be a 32 bit address?
    // apic regs should be identity mapped so phys == linear

    return low;
}

void apic_read_reg_32(unsigned int reg, unsigned int * value) {
    unsigned int low, high;
    apic_get_base(&_apic_base_msr, &low, &high);

    low += reg;

    unsigned int * addr = (unsigned int *)apic_phys_to_linear_addr(low, high);

    *value = *addr;
}

void apic_write_reg_32(unsigned int reg, unsigned int value) {
    unsigned int low, high;
    apic_get_base(&_apic_base_msr, &low, &high);

    low += reg;

    unsigned int * addr = (unsigned int *)apic_phys_to_linear_addr(low, high);

    *addr = value;
}

void apic_sivr_enable() {
    apic_svr_t sivr;
    apic_read_reg_32(APIC_REG_SIVR, &sivr);

    if(!APIC_SVR_GET_SOFTWARE_ENABLE(sivr)) {
        sivr = APIC_SVR_SET_SOFTWARE_ENABLE(sivr, 1);

        apic_write_reg_32(APIC_REG_SIVR, sivr);
    }
}

void apic_sivr_disable() {
    apic_svr_t sivr;
    apic_read_reg_32(APIC_REG_SIVR, &sivr);

    if(!APIC_SVR_GET_SOFTWARE_ENABLE(sivr)) {
        sivr = APIC_SVR_SET_SOFTWARE_ENABLE(sivr, 0);

        apic_write_reg_32(APIC_REG_SIVR, sivr);
    }
}

void apic_print_base_msr(const struct apic_base_msr * msr) {
    unsigned int base_low, base_high;
    apic_get_base(msr, &base_low, &base_high);

    kprintf("apic_low=0x%x apic_high=0x%x bsp=%u global_enabled=%u x2_enabled=%u\n",
            base_low,
            base_high,
            APIC_MSR_LOW_GET_BSP(msr->low),
            APIC_MSR_LOW_GET_GLOBAL_ENABLE(msr->low),
            APIC_MSR_LOW_GET_X2APIC_ENABLE(msr->low));
}

bool apic_available(const struct cpuid_info * cpu) {
    unsigned int basic_features;

    assert(cpu);

    basic_features = cpu->features.edx;
    assert(basic_features);

    return (basic_features & CPUID_FEAT_EDX_APIC) ? true : false;
}

void apic_read_msr(const struct cpuid_info * cpu, struct apic_base_msr * msr) {
    unsigned int max_phys_addr_bits;

    assert(cpu);

    max_phys_addr_bits = CPUID_ADDRESS_SIZE_GET_PHYSICAL_BITS(cpu->address_size);
    assert(max_phys_addr_bits);
    assert(max_phys_addr_bits >= 32);

    msrs_get(IA32_APIC_BASE, &msr->low, &msr->high);

    unsigned int high_mask = ~(0xffffffff << (max_phys_addr_bits - 32));

    msr->high &= high_mask;
}

void apic_write_msr(const struct apic_base_msr * msr) {
    assert(msr);
    assert(msr->low);

    msrs_set(IA32_APIC_BASE, msr->low, msr->high);
}

void apic_get_base(const struct apic_base_msr * msr, unsigned int * low, unsigned int * high) {
    *low = APIC_MSR_LOW_GET_BASE(msr->low) << 12;
    *high = APIC_MSR_HIGH_GET_BASE(msr->high);
}

void apic_set_base(struct apic_base_msr * msr, uint32_t low, uint32_t high) {
    msr->low = APIC_MSR_LOW_SET_BASE(msr->low, (low >> 12));
    msr->high = APIC_MSR_HIGH_SET_BASE(msr->high, high);
}

uint8_t apic_current_cpu_apic_id(void) {
    unsigned result;
    apic_read_reg_32(APIC_REG_ID, &result);
    return (uint8_t)result;
}

/* not for NMI, SMI, INIT, ExtINT, or SIPI interrupts.
   only for fixed interrupts delivered via the IRR (or ISR) registers.
   see ia32 pdf 10.8.1
 */
void apic_eoi() {
    apic_write_reg_32(APIC_REG_EOI, 0);
}

void apic_configure_lapic(uint8_t id, uint8_t processor_id, uint32_t lapic_flags) {
    // nothing. local apic already configured using MSRs
}

void apic_configure_ioapic(uint8_t id, uint32_t phys_address, uint32_t global_irq_base) {
    struct ioapic * i = ioapic_alloc();
    assert(i);
    i->id = id;
    i->phys_address = phys_address;
    i->irq_base = global_irq_base;
    ioapic_add(i);

    paging_identity_map(paging_pd_current(), i->phys_address, PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_NOT_CACHEABLE);

    i->address = i->phys_address;

    i->irq_count = ioapic_get_max_redirect(i) + 1;
}

void ioapic_print_redirection_table(struct ioapic * i) {
    uint32_t lower, upper;
    kprintf("I/O APIC Id=%hhd %d irqs\n", i->id, i->irq_count);
    for(int j = 0; j < i->irq_count; j++) {
        ioapic_read_redirection_entry(i, j, &lower, &upper);
        if(IOAPIC_RED_GET_INTERRUPT_VECTOR(lower) != 0) {
            kprintf("I/O APIC Id=%hhd IRQ=%d lower=0x%x upper=0x%x\n",
                i->id, j, lower, upper);
        }
    }
}

void apic_configure_int_override(uint8_t bus, uint8_t source_irq,
                                 uint32_t global_irq, uint16_t inti_flags) {
}

void apic_configure_nmi_source(uint32_t global_irq, uint16_t inti_flags) {
}

void apic_configure_lapic_nmi(uint8_t processor_id, uint8_t lint, uint16_t inti_flags) {
}

void apic_configure_lapic_override(uint64_t phys_address) {
}

struct ioapic * ioapic_alloc(void) {
    return (struct ioapic*)kalloc_static(sizeof(struct ioapic), 0);
}

void ioapic_add(struct ioapic * i) {
    LL_PREPEND(_ioapics, i);
    LL_SORT(_ioapics, ioapic_compare_irq_base);
}

struct ioapic * ioapic_for_irq(uint32_t irq) {
    struct ioapic * i;
    LL_FOREACH(_ioapics, i) {
        if(irq >= i->irq_base && irq < (i->irq_base + i->irq_count)) {
            return i;
        }
    }
    return NULL;
}

int ioapic_compare_irq_base(struct ioapic * a, struct ioapic * b) {
    if(a->irq_base < b->irq_base) {
        return -1;
    }
    if(a->irq_base > b->irq_base) {
        return 1;
    }
    return 0;
}

int ioapic_get_max_redirect(struct ioapic * i) {
    uint32_t version = ioapic_read_reg(i, IOAPIC_REGSEL_VER);
    return (int)IOAPIC_REG_VER_GET_MAX_REDIRECT(version);
}

uint32_t ioapic_read_reg(struct ioapic * i, int regsel) {
    *IOAPIC_IOREGSEL(i->address) = regsel;
    return *IOAPIC_IOWIN(i->address);
}

void ioapic_write_reg(struct ioapic * i, int regsel, uint32_t value) {
    *IOAPIC_IOREGSEL(i->address) = regsel;
    *IOAPIC_IOWIN(i->address) = value;
}

// index is between 0 and max redirect
void ioapic_read_redirection_entry(struct ioapic * i, int index,
                                   uint32_t * lower, uint32_t * upper) {
    int regsel = IOAPIC_REGSEL_REDIRECT_TBL_BASE + index * 2;
    assert(lower);
    assert(upper);
    *lower = ioapic_read_reg(i, regsel);
    *upper = ioapic_read_reg(i, regsel + 1);
}

void ioapic_write_redirection_entry(struct ioapic * i, int index,
                        uint32_t lower, uint32_t upper) {
    int regsel = IOAPIC_REGSEL_REDIRECT_TBL_BASE + index * 2;
    ioapic_write_reg(i, regsel, lower);
    ioapic_write_reg(i, regsel + 1, upper);
}

void ioapic_set_redirection(struct ioapic * i, int index,
                            uint8_t destination,
                            enum ioapic_redirect_mask masked,
                            enum ioapic_trigger_mode trigger_mode,
                            enum ioapic_polarity polarity,
                            enum ioapic_destination_mode dest_mode,
                            enum ioapic_delivery_mode delivery_mode,
                            int interrupt_vector
                            ) {

    uint32_t upper = IOAPIC_RED_SET_DESTINATION(0, destination);

    uint32_t lower = IOAPIC_RED_SET_MASK(0, masked);

    lower = IOAPIC_RED_SET_TRIGGER_MODE(lower, trigger_mode);
    lower = IOAPIC_RED_SET_POLARITY(lower, polarity);
    lower = IOAPIC_RED_SET_DEST_MODE(lower, dest_mode);
    lower = IOAPIC_RED_SET_DELIVERY_MODE(lower, delivery_mode);
    lower = IOAPIC_RED_SET_INTERRUPT_VECTOR(lower, interrupt_vector);

    ioapic_write_redirection_entry(i, index, lower, upper);
}

// TODO: this doesn't take account of Interrupt Overrides (see acpi madt)
// it seems that the PIT irq from the ISA bus is often remapped from 0 to 2
// currently we use the post-override value here
void ioapic_setup_irq(uint32_t irq, uint8_t destination,
                      enum ioapic_redirect_mask masked,
                      enum ioapic_trigger_mode trigger_mode,
                      enum ioapic_polarity polarity,
                      enum ioapic_destination_mode dest_mode,
                      enum ioapic_delivery_mode delivery_mode,
                      int interrupt_vector) {
    struct ioapic * i = ioapic_for_irq(irq);
    assert(i);

    ioapic_set_redirection(i, irq, destination,
                           ioapic_redirect_unmasked,
                           ioapic_trigger_edge,
                           ioapic_active_high,
                           ioapic_dest_physical_mode,
                           ioapic_delivery_fixed, interrupt_vector);
}

// convert a global system interrupt number to
// an interrupt vector
// doesn't account for interrupt overrides
// e.g. if ISA PIT IRQ 0 -> Global IRQ 2 -> CPU Interrupt Vector 2
// then this handles the second mapping
int ioapic_interrupt_for_irq(uint32_t irq) {
    struct ioapic * i = ioapic_for_irq(irq);
    assert(i);
    uint32_t lower, upper;
    ioapic_read_redirection_entry(i, irq - i->irq_base, &lower, &upper);
    return IOAPIC_RED_GET_INTERRUPT_VECTOR(lower);
}

void ioapic_mask_irq(uint32_t irq) {
    struct ioapic * i = ioapic_for_irq(irq);
    assert(i);
    uint32_t lower, upper;
    int index = irq - i->irq_base;
    ioapic_read_redirection_entry(i, index, &lower, &upper);
    lower = IOAPIC_RED_SET_MASK(lower, ioapic_redirect_masked);
    ioapic_write_redirection_entry(i, index, lower, upper);
}

void ioapic_unmask_irq(uint32_t irq) {
    struct ioapic * i = ioapic_for_irq(irq);
    assert(i);
    uint32_t lower, upper;
    int index = irq - i->irq_base;
    ioapic_read_redirection_entry(i, index, &lower, &upper);
    lower = IOAPIC_RED_SET_MASK(lower, ioapic_redirect_unmasked);
    ioapic_write_redirection_entry(i, index, lower, upper);
}
