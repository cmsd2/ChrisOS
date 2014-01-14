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
#include <standard.h>

struct apic_base_msr _apic_base_msr;

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

    apic_timer_init();

	apic_global_enable(cpu);

	//todo mask all the interrupts until ready to receive them

    union apic_sivr sivr = {.value = 0};
    sivr.fields.software_enable = 1;
    sivr.fields.spurious_vector = 39;
    apic_write_reg_32(APIC_REG_SIVR, sivr.value);
}

// based on wiki.osdev.org/APIC_timer
void apic_timer_init() {
    unsigned int timer_div = 16;
    unsigned int test_sleep_freq = 10; // Hz
    unsigned int pit_freq = 1193180; // Hz
    unsigned int test_sleep_time = pit_freq / test_sleep_freq;

    apic_write_reg_32(APIC_REG_LVT_TIMER, 32);

    apic_write_reg_32(APIC_REG_TIMER_DCR, timer_div);

    // enable PIT channel 2, but not PC Speaker
    unsigned char nmi_sc = inb(NMI_STATUS_AND_CONTROL);
    nmi_sc &= NMI_SC_MASK(NMI_SC_SPEAKER_ENABLE);
    nmi_sc |= NMI_SC_TMR2_ENABLE;
    outb(NMI_STATUS_AND_CONTROL, nmi_sc);

    // set PIT ch2 to one-shot mode
    pit_set_mode(pit_channel_2, pit_low_high_byte, pit_mode_1_programmable_one_shot);
    //1193180/100 Hz = 11931 = 0x2e9b
    pit_set_counter(pit_channel_2, LOW_BYTE(test_sleep_time));
    ps2_read_data(); // delay
    pit_set_counter(pit_channel_2, HIGH_BYTE(test_sleep_time));

    uint8_t tmp = inb(NMI_STATUS_AND_CONTROL);
    tmp &= NMI_SC_MASK(NMI_SC_TMR2_ENABLE);

    // pulse the timer bit low then high again to reset
    outb(NMI_STATUS_AND_CONTROL, tmp);
    outb(NMI_STATUS_AND_CONTROL, tmp | NMI_SC_TMR2_ENABLE);

    // reset apic timer counter
    unsigned int start_count = -1;
    apic_write_reg_32(APIC_REG_TIMER_ICR, start_count);

    while(!(inb(NMI_STATUS_AND_CONTROL) & NMI_SC_TMR2_OUT_STS));

    apic_write_reg_32(APIC_REG_LVT_TIMER, APIC_LVT_MASKED);

    unsigned int curcount;
    apic_read_reg_32(APIC_REG_TIMER_CCR, &curcount);

    unsigned int ticks = start_count - curcount;
    uint32_t cpu_bus_freq = ticks * timer_div * test_sleep_freq;
    //ticks_per_quantum = cpu_bus_freq / quantum / timer_div;
    kprintf("done initialising timer. freq=%d\n", cpu_bus_freq);

    //todo: save this stuff somewhere
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

