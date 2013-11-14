#include <kernel/terminal.h>
#include <kernel/kprintf.h>

void panic(const char * msg) {
	/*todo...*/
	kprintf("panic! %s\n", msg);
}
