#include <arch/msrs.h>
#include <arch/cpuid.h>

bool msrs_available() {
	unsigned int basic;
	cpuid_features(&basic, 0);

	return (basic & CPUID_FEAT_EDX_MSR) ? true : false;
}

void msrs_get(unsigned int msr, unsigned int * low, unsigned int * high) {
	__asm__ volatile(
		"rdmsr"
		: "=a" (*low), "=d" (*high)
		: "c" (msr)
	);
}

void msrs_set(unsigned int msr, unsigned int low, unsigned int high) {
	__asm__ volatile (
		"wrmsr"
		:
		: "a" (low), "d" (high), "c" (msr)
	);
}

