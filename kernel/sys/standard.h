#ifndef STANDARD_H
#define STANDARD_H

#include <stddef.h>

#ifndef MAX
#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#endif

#ifndef MIN
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

#define LOW_BYTE(word) ((word)&0xff)
#define HIGH_BYTE(word) LOW_BYTE((word)>>8)

#define STRUCT_MEMBER_OFFSET(strct, member) ((ptrdiff_t)(&((strct *)(void*)0)->member))
#define STRUCT_START(strct, member, member_ptr) ((strct *)(((char*)(member_ptr)) - STRUCT_MEMBER_OFFSET(strct, member)))

#endif
