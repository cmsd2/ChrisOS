#include <arch/cpuid.h>
#include <kernel/string.h>
#include <kernel/kprintf.h>

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

enum cpuid_vendor_id cpuid_vendor() {
	size_t i;
	char str[12];
	uint32_t * strptr = (uint32_t *)str;

	__asm__ volatile("mov $0, %%eax\n\t"
		"cpuid\n\t"
		: "=b" (strptr[0]), "=d" (strptr[1]), "=c" (strptr[2])
		: "r" (strptr)
		: "%eax"
		);

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
