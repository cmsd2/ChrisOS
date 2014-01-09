#ifndef ASSERT_H
#define ASSERT_H

#ifdef _KERNEL
  #include <utils/panic.h>
#endif

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#ifdef _KERNEL
  #define kernel_assert_failed(expr) (panic(__FILE__ ":" S__LINE__ " assertion failed: " #expr ), 0)
#endif

#ifdef NDEBUG
  #define assert(ignore) ((void)0)
#else
  #ifdef _KERNEL
    #define assert(expr) ( (expr) || kernel_assert_failed( (expr) ) )
  #else
    #define assert(expr) __assert(__func__, __FILE__, __LINE__, expr)
  #endif
#endif

#endif
