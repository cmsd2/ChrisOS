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
  CPUID_GETVENDORSTRING,
  CPUID_GETFEATURES,
  CPUID_GETTLB,
  CPUID_GETSERIAL
};
 
// extended cpuid requests
static const unsigned int CPUID_INTELEXTENDED = 0x80000000;
static const unsigned int CPUID_INTELFEATURES = 0x80000001;
static const unsigned int CPUID_INTELBRANDSTRING = 0x80000002;
static const unsigned int CPUID_INTELBRANDSTRINGMORE = 0x80000003;
static const unsigned int CPUID_INTELBRANDSTRINGEND = 0x80000004;

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

static KERNEL_INLINE void cpuid(unsigned long code, unsigned long *a, unsigned long *d) {
	__asm__ volatile(
		"cpuid"
		: "=a" (*a), "=d" (*d)
		: "a" (code)
		: "ecx", "ebx"
	);
}

enum cpuid_vendor_id cpuid_vendor();

const char * cpuid_vendor_string(enum cpuid_vendor_id id);

#endif
