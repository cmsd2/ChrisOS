#ifndef STRTOUQ_H
#define STRTOUQ_H

#include <sys/types.h>

u_quad_t strtouq(const char *nptr, char **endptr, int base);

#endif
