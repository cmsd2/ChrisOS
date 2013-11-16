#ifndef ASSERT_H
#define ASSERT_H

#include <kernel/panic.h>

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define assert_failed(expr) (panic(__FILE__ ":" S__LINE__ " assertion failed: " #expr ), 0)

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else
#define assert(expr) ( (expr) || assert_failed( (expr) ) )
#endif

#endif
