#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif
    
void panic(const char * msg) __attribute__ ((noreturn));

#ifdef __cplusplus
}
#endif

#endif
