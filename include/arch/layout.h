#ifndef LAYOUT_H
#define LAYOUT_H

#include <stddef.h>

/* kernel sections.
 * each variable is the first word of its section
 * take the address of these to get the virtual memory address
 * of their section
 * see linker script
 */
extern int _code;
extern int _rodata;
extern int _data;
extern int _bss;

extern unsigned char KERNEL_VMA;
extern unsigned char KERNEL_LMA;

/*
 * kernel stack region
 */
extern int stack_bottom;
extern int stack_top;

/* end of sections.
 * variable is the first word after the section end.
 */
extern int _end;

#endif
