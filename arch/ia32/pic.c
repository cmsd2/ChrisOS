#include <arch/pic.h>

void pic_disable(void) {
	__asm__ volatile (
		"mov $0xff, %%al\n\t"
		"out %%al, $0xa1\n\t"
		"out %%al, $0x21\n\t"
		::: "%al"
	);
}
