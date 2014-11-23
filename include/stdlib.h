#ifndef STDLIB_H
#define STDLIB_H

#ifdef _KERNEL
#include <mm/malloc.h>
#endif

#ifdef _KERNEL
  #define exit(code) (panic(__FILE__ ":" S__LINE__ " exit called with code: " #code ))
#endif

#endif
