#ifndef KERNEL_UTILS_STRING_H
#define KERNEL_UTILS_STRING_H

#include <stddef.h>

size_t strlen(const char *str);
int strncmp(const char * a, const char * b, size_t n);
int strcmp(const char * a, const char * b);

extern const char * true_str;
extern const char * false_str;
#define BOOL_TO_STR(b) (b == true ? true_str : false_str)

#endif
