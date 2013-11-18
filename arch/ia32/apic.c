#include <arch/apic.h>
#include <arch/cpuid.h>
#include <arch/msrs.h>
#include <assert.h>
#include <kernel/kprintf.h>
#include <arch/paging.h>
#include <kernel/mem.h>

void apic_init(const struct cpuid_info * cpu) {
	assert(apic_available(cpu));

	struct apic_base_msr msr;
	apic_read_msr(cpu, &msr);

	apic_print_base_msr(&msr);

	//TODO: base addr can be 36 or more bits with pae and/or long mode
	unsigned int low, high;
	apic_get_base(&msr, &low, &high);

	size_t size = align_padding((uintptr_t)low, 0x400000);

	paging_identity_map(_kernel_page_dir, (uintptr_t) low, size, I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_NOT_CACHEABLE);

	apic_global_enable(cpu, &msr);

	//todo mask all the interrupts until ready to receive them
	apic_sivr_enable(&msr);
}

/*
 * can't be done after global disable, without reset.
 */
void apic_global_enable(const struct cpuid_info * cpu, struct apic_base_msr * msr) {
	if(!msr->low.fields.global_enable) {
		msr->low.fields.global_enable = 1;

		apic_write_msr(msr);
		apic_read_msr(cpu, msr);

		assert(msr->low.fields.global_enable);
	}
}

uintptr_t apic_phys_to_linear_addr(unsigned int low, unsigned int __attribute__ ((unused)) high) {
	// TODO:
	// convert 36bit or more physical address to 32 bit linear address
	// without pae, it should only be a 32 bit address?
	// apic regs should be identity mapped so phys == linear

	return low;
}

void apic_read_reg_32(const struct apic_base_msr * msr, unsigned int reg, unsigned int * value) {
	unsigned int low, high;
	apic_get_base(msr, &low, &high);

	low += reg;

	unsigned int * addr = (unsigned int *)apic_phys_to_linear_addr(low, high);

	*value = *addr;
}

void apic_write_reg_32(const struct apic_base_msr * msr, unsigned int reg, unsigned int value) {
	unsigned int low, high;
	apic_get_base(msr, &low, &high);

	low += reg;

	unsigned int * addr = (unsigned int *)apic_phys_to_linear_addr(low, high);

	*addr = value;
}

void apic_sivr_enable(const struct apic_base_msr * msr) {
	struct apic_sivr sivr_reg;
	apic_read_reg_32(msr, APIC_REG_SIVR, &sivr_reg.sivr.value);

	if(!sivr_reg.sivr.fields.software_enable) {
		sivr_reg.sivr.fields.software_enable = 1;

		apic_write_reg_32(msr, APIC_REG_SIVR, sivr_reg.sivr.value);
	}
}

void apic_sivr_disable(const struct apic_base_msr * msr) {
	struct apic_sivr sivr_reg;
	apic_read_reg_32(msr, APIC_REG_SIVR, &sivr_reg.sivr.value);

	if(sivr_reg.sivr.fields.software_enable) {
		sivr_reg.sivr.fields.software_enable = 0;

		apic_write_reg_32(msr, APIC_REG_SIVR, sivr_reg.sivr.value);
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

