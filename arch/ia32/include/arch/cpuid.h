#ifndef IA32_ARCH_CPUID_H
#define IA32_ARCH_CPUID_H

#include <stdbool.h>
#include <stdint.h>

#include <standard.h>

enum cpuid_vendor_id {
CPUID_VENDOR_UNKNOWN = -1,
CPUID_VENDOR_OLDAMD = 0,
CPUID_VENDOR_AMD,
CPUID_VENDOR_INTEL,
CPUID_VENDOR_VIA,
CPUID_VENDOR_OLDTRANSMETA,
CPUID_VENDOR_TRANSMETA,
CPUID_VENDOR_CYRIX,
CPUID_VENDOR_CENTAUR,
CPUID_VENDOR_NEXGEN,
CPUID_VENDOR_UMC,
CPUID_VENDOR_SIS,
CPUID_VENDOR_NSC,
CPUID_VENDOR_RISE,
};

enum cpuid_requests {
  CPUID_GET_VENDOR_STRING,
  CPUID_GET_FEATURES,
  CPUID_GET_TLB,
  CPUID_GET_SERIAL,

  CPUID_INTEL_EXTENDED = (int)0x80000000,
  CPUID_INTEL_FEATURES = (int)0x80000001,
  CPUID_INTEL_BRAND_STRING = (int)0x80000002,
  CPUID_INTEL_BRAND_STRING_MORE = (int)0x80000003,
  CPUID_INTEL_BRAND_STRING_END = (int)0x80000004,
  CPUID_INTEL_ADDRESS_SIZE = (int)0x80000008
};

struct cpuid_info {
	struct {
		unsigned int max_cpuid;
		unsigned int max_extended_cpuid;
	} meta;

	//0x00
	enum cpuid_vendor_id vendor_id;
	const char * vendor_string;

	//0x01
	union {
		unsigned int value;
		struct {
			unsigned int stepping_id : 4;
			unsigned int model : 4;
			unsigned int family_id : 4;
			unsigned int cpu_type : 2;
			unsigned int reserved_14_15 : 2;
			unsigned int extended_model_id : 4;
			unsigned int extended_family_id : 8;
		} fields;
	} mfs;
	union {
		unsigned int value;
		struct {
			unsigned int brand_index : 8;
			unsigned int clflush : 8;
			unsigned int max_apic_ids : 8;
			unsigned int initial_apic_id : 8;
		} fields;
	} apic;
	struct {
		unsigned int ecx;
		unsigned int edx;
	} features;

	// 0x02 cache and tlb info
	// 0x03 serial number (pentium 3 only)

	// following only visible when IA32_MISC_ENABLE.BOOT_NT4[bit 22] = 0 (default)
	// 0x04 cache details, depends on ecx
	// 0x05 monitor/mwait info
	// 0x06 thermal & power management
	// 0x07 structure extended feature flags, depends on ecx
	// 0x09 direct cache access info
	// 0x0A arch perf monitoring
	// 0x0B extended topology enumeration, depends on ecx
	// 0x0D cpu extended state enumeration, depends on ecx
	// 0x0F QoS resources and capabilities, depends on ecx

	// following only available on pentium 4 and up

	// 0x80000000 extended function cpuid info
	// 0x80000001 extended cpu signature and feature bits
	// 0x80000002-4 cpu brand string
	// 0x80000006 cache info
	// 0x80000007 tsc

	// 0x80000008 max physical/linear addressable quantities
	union {
		unsigned int value;
		struct {
			unsigned int physical_address_bits : 8;
			unsigned int linear_address_bits : 8;
		} fields;
	} address_size;
};

enum {
    CPUID_FEAT_EBX_BRAND        = 0xff,
    CPUID_FEAT_EBX_CLFLUSH      = 0xff00,
    CPUID_FEAT_EBX_MAX_CPU_APIC_IDS = 0xff0000,
    CPUID_FEAT_EBX_INITIAL_APIC_ID  = (int)0xff000000,

    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_ETPRD        = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_x2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLF          = 1 << 19, 
    CPUID_FEAT_EDX_DTES         = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM1          = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = (int)(0x1 << 31)
};

static KERNEL_INLINE uint32_t get_flags(void) {
	uint32_t ret;
	__asm__("pushf\n\t"
		"pop %0"
		: "=r" (ret) 
		::);

	return ret;
}

static KERNEL_INLINE void set_flags(uint32_t f) {
	__asm__("push %0\n\t"
		"popf"
		:
		: "r" (f) 
		:);
}

bool cpuid_available(void);
unsigned int cpuid_max_basic_cpuid();

bool cpuid_extended_function_info_available();
unsigned int cpuid_max_extended_function_cpuid();

static KERNEL_INLINE void cpuid(unsigned int code, unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d) {
	unsigned int tmp[4];
	__asm__ volatile(
		"cpuid"
		: "=a" (tmp[0]), "=b" (tmp[1]), "=c" (tmp[2]), "=d" (tmp[3])
		: "a" (code)
	);
	if(a) {
		*a = tmp[0];
	}
	if(b) {
		*b = tmp[1];
	}
	if(c) {
		*c = tmp[2];
	}
	if(d) {
		*d = tmp[3];
	}
}

void cpuid_features(unsigned int * basic, unsigned int * ext);

uint8_t cpuid_current_cpu_apic_id(void);

enum cpuid_vendor_id cpuid_vendor();

const char * cpuid_vendor_string(enum cpuid_vendor_id id);

void cpuid_read_info(struct cpuid_info * info);

void cpuid_print_info(const struct cpuid_info * info);

#endif
