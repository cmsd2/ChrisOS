#include <arch/cpuid.h>
#include <utils/string.h>
#include <utils/kprintf.h>
#include <utils/mem.h>

const char * cpuid_vendors[] = {
	"AMDisbetter!",
	"AuthenticAMD",
	"GenuineIntel",
	"CentaurHauls",
	"TransmetaCPU",
	"GenuineTMx86",
	"CyrixInstead",
	"CentaurHauls",
	"NexGenDriven",
	"UMC UMC UMC ",
	"SiS SiS SiS ",
	"Geode by NSC",
	"RiseRiseRise"
};

// cpuid inline functions live here
extern inline uint32_t get_flags(void);
extern inline void set_flags(uint32_t f);
extern inline void cpuid(unsigned int code, unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d);
	
bool cpuid_available(void) {
	uint32_t flags_orig, flags_tmp;

	flags_orig = get_flags();
	flags_tmp = flags_orig | 0x200000;

	set_flags(flags_tmp);
	flags_tmp = get_flags();

	set_flags(flags_orig);

	if(flags_tmp & 0x200000) {
		return true;
	} else {
		return false;
	}
}

void cpuid_features(unsigned int * basic, unsigned int * ext) {
	cpuid(CPUID_GET_FEATURES, 0, 0, ext, basic);
}

uint8_t cpuid_current_cpu_apic_id(void) {
	unsigned int b;
	cpuid(CPUID_GET_FEATURES, 0, &b, 0, 0);

	b &= ((unsigned int)CPUID_FEAT_EBX_INITIAL_APIC_ID);
	
	return (uint8_t)(b >>= 24);
}

enum cpuid_vendor_id cpuid_vendor() {
	size_t i;
	char str[12];
	unsigned * strptr = (unsigned *)str;

    cpuid(0, 0, &strptr[0], &strptr[2], &strptr[1]);
	/*__asm__ volatile("mov $0, %%eax\n\t"
		"cpuid\n\t"
		: "=b" (strptr[0]), "=d" (strptr[1]), "=c" (strptr[2])
		: "r" (strptr)
		: "%eax"
		);*/

	for(i = 0; i < sizeof(cpuid_vendors); i++) {
		if(strncmp(cpuid_vendors[i], str, 12) == 0) {
			return i;
		}
	}

	return CPUID_VENDOR_UNKNOWN;
}

const char * cpuid_vendor_string(enum cpuid_vendor_id id) {
	if(id == -1) {
		return "unknown";
	}

	return cpuid_vendors[id];
}

unsigned int cpuid_max_basic_cpuid() {
	unsigned int max_basic_cpuid;
	cpuid(CPUID_GET_VENDOR_STRING, &max_basic_cpuid, 0, 0, 0);
	return max_basic_cpuid;
}

/* either look at cpu model or guess? */
bool cpuid_extended_function_info_available() {
	return (cpuid_max_basic_cpuid() >= 0x5) ? true : false;
}

unsigned int cpuid_max_extended_function_cpuid() {
	unsigned int max_ext_fun;
	cpuid(CPUID_INTEL_EXTENDED, &max_ext_fun, 0, 0, 0);
	return max_ext_fun;
}

void cpuid_read_info(struct cpuid_info * info) {
	memset(info, 0, sizeof(*info));

	if(!cpuid_available()) {
		return;
	}

	info->vendor_id = cpuid_vendor();
	info->vendor_string = cpuid_vendor_string(info->vendor_id);
	info->meta.max_cpuid = cpuid_max_basic_cpuid();
	
	if(!cpuid_extended_function_info_available()) {
		info->meta.max_extended_cpuid = 0;

		// footnote 1, 10-8 Vol. 3A Intel SMD
		info->address_size = CPUID_ADDRESS_SIZE_SET_PHYSICAL_BITS(0, 36);
	} else {
		/* pentium 4 and up */
		info->meta.max_extended_cpuid = cpuid_max_extended_function_cpuid();

		cpuid(CPUID_INTEL_ADDRESS_SIZE, &info->address_size, 0, 0, 0);
	}

	cpuid(CPUID_GET_FEATURES, &info->mfs, &info->apic, &info->features.ecx, &info->features.edx);
}

void cpuid_print_info(const struct cpuid_info * info) {
	kprintf("max_cpuid=%x max_ext_cpuid=%x\n", 
		info->meta.max_cpuid, 
		info->meta.max_extended_cpuid);
	kprintf("vendor_string=%s\n", info->vendor_string);
	kprintf("stepping_id=%u model=%u family_id=%u cpu_type=%u ext_model_id=%u ext_family_id=%u\n", 
		CPUID_MFS_GET_STEPPING_ID(info->mfs),
		CPUID_MFS_GET_MODEL(info->mfs),
		CPUID_MFS_GET_FAMILY_ID(info->mfs),
		CPUID_MFS_GET_CPU_TYPE(info->mfs),
		CPUID_MFS_GET_EXT_MODEL_ID(info->mfs),
		CPUID_MFS_GET_EXT_FAMILY_ID(info->mfs));
	kprintf("brand_index=%u clflush=%u max_apic_ids=%u initial_apic_id=%u\n",
		CPUID_APIC_GET_BRAND_INDEX(info->apic),
		CPUID_APIC_GET_CLFLUSH(info->apic),
		CPUID_APIC_GET_MAX_APIC_IDS(info->apic),
		CPUID_APIC_GET_INITIAL_APIC_ID(info->apic));

	kprintf("features.edx=0x%x features.ecx=0x%x\n", 
		info->features.edx, 
		info->features.ecx);

	kprintf("max_phys_addr_bits=%u max_lin_addr_bits=%u\n",
		CPUID_ADDRESS_SIZE_GET_PHYSICAL_BITS(info->address_size),
		CPUID_ADDRESS_SIZE_GET_LINEAR_BITS(info->address_size));
}
