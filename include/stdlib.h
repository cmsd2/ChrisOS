#ifndef STDLIB_H
#define STDLIB_H

#ifndef _KERNEL
  #error No stdlib or libc implemented yet
#endif

#ifdef _KERNEL
#include <mm/malloc.h>
#include <utils/string.h>
#include <sys/systm.h>
#endif

#ifdef _KERNEL
  #define exit(code) (panic(__FILE__ ":" S__LINE__ " exit called with code: " #code ))
#endif

#endif
