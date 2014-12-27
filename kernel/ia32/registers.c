#include <arch/registers.h>

void push_flags_register() {
    __asm__("pushfl");
}

void pop_flags_register() {
    __asm__("popfl");
}
