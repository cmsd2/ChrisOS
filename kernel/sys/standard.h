#ifndef STANDARD_H
#define STANDARD_H

#include <stddef.h>

#define STRUCT_MEMBER_OFFSET(strct, member) offsetof(strct, member)
#define STRUCT_START(strct, member, member_ptr) ((strct *)(((char*)(member_ptr)) - STRUCT_MEMBER_OFFSET(strct, member)))

#endif
