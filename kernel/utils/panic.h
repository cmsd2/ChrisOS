#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

void panic(const char * msg) __attribute__ ((noreturn));

#endif
