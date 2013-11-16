#ifndef STANDARD_H
#define STANDARD_H

#ifdef __GNUC__
#define KERNEL_INLINE inline __attribute__((always_inline)) 
#else
#define KERNEL_INLINE inline
#endif

#endif
