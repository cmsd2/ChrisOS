#ifndef STDLIB_H
#define STDLIB_H

#include <sys/cdefs.h>

#ifndef _KERNEL
void abort(void) _Noreturn;
#else /* _KERNEL */

#include <mm/malloc.h>
#include <utils/string.h>
#include <sys/systm.h>

#define exit(code) (panic(__FILE__ ":" S__LINE__ " exit called with code: " #code ))
#endif

#endif
