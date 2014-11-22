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

struct apic_base_msr _apic_base_msr;
uint32_t _apic_bus_freq;

void apic_init(const struct cpuid_info * cpu) {
	assert(apic_available(cpu));

	apic_read_msr(cpu, &_apic_base_msr);

	apic_print_base_msr(&_apic_base_msr);

	//TODO: base addr can be 36 or more bits with pae and/or long mode
	unsigned int low, high;
	apic_get_base(&_apic_base_msr, &low, &high);

	size_t size = align_padding((uintptr_t)low, 0x400000);

	paging_identity_map(_kernel_page_dir, (uintptr_t) low, size, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_NOT_CACHEABLE);

    // i32 10.6.2.2:
    // flat model IPI desination 31:28 set 0xf, 28:0 reserved (all '1')
    apic_write_reg_32(APIC_REG_DFR, 0xffffffff);

    // logical apic ID 31:24 set to 8bit logical id, 23:0 reserved
    // todo: in multiprocessor system each cpu has different logical ID
    apic_write_reg_32(APIC_REG_LDR, 0x1 << 24);

    apic_write_reg_32(APIC_REG_LVT_TIMER, APIC_LVT_MASKED);
    apic_write_reg_32(APIC_REG_LVT_PERF_MCR, APIC_LVT_NMI);
    apic_write_reg_32(APIC_REG_LVT_LINT0, APIC_LVT_MASKED);
    apic_write_reg_32(APIC_REG_LVT_LINT1, APIC_LVT_MASKED);
    apic_write_reg_32(APIC_REG_TPR, 0);

	apic_global_enable(cpu);

	//todo mask all the interrupts until ready to receive them

    union apic_sivr sivr = {.value = 0};
    sivr.fields.software_enable = 1;
    sivr.fields.spurious_vector = 39;
    apic_write_reg_32(APIC_REG_SIVR, sivr.value);

    apic_timer_init();
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
    unsigned char timer_interrupt = pic_interrupt_for_irq(0);
    volatile unsigned int tick = 0;
    unsigned int timer_div = 16;
    unsigned int test_sleep_freq = 10; // Hz
    unsigned int pit_freq = 1193180; // Hz
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
    apic_write_reg_32(APIC_REG_LVT_TIMER, APIC_LVT_MASKED);
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
	if(!_apic_base_msr.low.fields.global_enable) {
		_apic_base_msr.low.fields.global_enable = 1;

		apic_write_msr(&_apic_base_msr);
		apic_read_msr(cpu, &_apic_base_msr);

		assert(_apic_base_msr.low.fields.global_enable);
	}
}

uintptr_t apic_phys_to_linear_addr(unsigned int low, unsigned int __attribute__ ((unused)) high) {
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
    union apic_sivr sivr_reg;
	apic_read_reg_32(APIC_REG_SIVR, &sivr_reg.value);

	if(!sivr_reg.fields.software_enable) {
		sivr_reg.fields.software_enable = 1;

		apic_write_reg_32(APIC_REG_SIVR, sivr_reg.value);
	}
}

void apic_sivr_disable() {
	union apic_sivr sivr_reg;
	apic_read_reg_32(APIC_REG_SIVR, &sivr_reg.value);

	if(sivr_reg.fields.software_enable) {
		sivr_reg.fields.software_enable = 0;

		apic_write_reg_32(APIC_REG_SIVR, sivr_reg.value);
	}
}

void apic_print_base_msr(const struct apic_base_msr * msr) {
	unsigned int base_low, base_high;
	apic_get_base(msr, &base_low, &base_high);

	kprintf("apic_low=0x%x apic_high=0x%x bsp=%u global_enabled=%u x2_enabled=%u\n", 
		base_low,
		base_high,
		msr->low.fields.bsp, 
		msr->low.fields.global_enable,
		msr->low.fields.x2apic_enable);
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

	max_phys_addr_bits = cpu->address_size.fields.physical_address_bits;
	assert(max_phys_addr_bits);
	assert(max_phys_addr_bits >= 32);

	msrs_get(IA32_APIC_BASE, &msr->low.value, &msr->high.value);

	unsigned int high_mask = ~(0xffffffff << (max_phys_addr_bits - 32));

	msr->high.value &= high_mask;
}

void apic_write_msr(const struct apic_base_msr * msr) {
	assert(msr);
	assert(msr->low.value);

	msrs_set(IA32_APIC_BASE, msr->low.value, msr->high.value);
}

void apic_get_base(const struct apic_base_msr * msr, unsigned int * low, unsigned int * high) {
	*low = msr->low.fields.apic_base_low_part << 12;
	*high = msr->high.fields.apic_base_high;
}

void apic_set_base(struct apic_base_msr * msr, uint32_t low, uint32_t high) {
	msr->low.fields.apic_base_low_part = low >> 12;
	msr->high.fields.apic_base_high = high;
}

uint8_t apic_current_cpu_apic_id(void) {
	return 0;
}

void apic_eoi() {
    //todo: check this EOI thing. only for FIXED delivery mode interrupts
    apic_write_reg_32(APIC_REG_EOI, 0);
}

