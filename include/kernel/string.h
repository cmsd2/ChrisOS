#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <stddef.h>

size_t strlen(const char *str);
int strncmp(const char * a, const char * b, size_t n);
int strcmp(const char * a, const char * b);

#endif
