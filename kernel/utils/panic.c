#include <terminal/terminal.h>
#include <utils/kprintf.h>
#include <arch/power.h>

void panic(const char * msg) {
	/*todo...*/
	kprintf("panic! %s\n", msg);
    power_halt();
}
