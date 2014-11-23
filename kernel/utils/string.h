#ifndef KERNEL_UTILS_STRING_H
#define KERNEL_UTILS_STRING_H

#include <stddef.h>
#include <utils/mem.h>

size_t strlen(const char *str);
int strncmp(const char * a, const char * b, size_t n);
int strcmp(const char * a, const char * b);
int memcmp(const void * s1, const void *s2, size_t n);
char * strdup(const char *s1);
char * strndup(const char *s1, size_t n);

extern const char * true_str;
extern const char * false_str;
#define BOOL_TO_STR(b) (b == true ? true_str : false_str)

#endif
