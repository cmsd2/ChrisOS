#ifndef KERNEL_UTILS_STRING_H
#define KERNEL_UTILS_STRING_H

#include <stddef.h>
#include <utils/mem.h>

#ifdef __cplusplus
extern "C" {
#endif
    
size_t strlen(const char *str);
int strncmp(const char * a, const char * b, size_t n);
int strcmp(const char * a, const char * b);
int memcmp(const void * s1, const void *s2, size_t n);
char * strdup(const char *s1);
char * strndup(const char *s1, size_t n);
char * strcpy(char * dst, const char * src);
char * strncpy(char * dst, const char * src, size_t n);
char * strcat(char * dst, const char * src);
char * strncat(char * dst, const char * src, size_t n);

extern const char * true_str;
extern const char * false_str;
#define BOOL_TO_STR(b) (b == true ? true_str : false_str)

#ifdef __cplusplus
}
#endif

#endif
