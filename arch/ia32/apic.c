#include <arch/apic.h>
#include <arch/cpuid.h>
#include <arch/msrs.h>
#include <assert.h>
#include <kernel/kprintf.h>

void apic_init(const struct cpuid_info * cpu) {
	assert(apic_available(cpu));

	struct apic_base_msr msr;
	apic_read_msr(cpu, &msr);

	apic_print_base_msr(&msr);
}

void apic_print_base_msr(const struct apic_base_msr * msr) {
	uint32_t base_low, base_high;
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

void apic_get_base(const struct apic_base_msr * msr, uint32_t * low, uint32_t * high) {
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

