#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <stdint.h>

typedef uint64_t time_t;
typedef long suseconds_t;
typedef unsigned long useconds_t;

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

void time_init(void);

uint32_t time_lock();
void time_unlock(uint32_t flags);

void get_time_utc(struct timeval * tv);
void set_time_utc(struct timeval * tv);

void time_add(struct timeval * dest, struct timeval * diff);
void time_normalise(struct timeval * tv);

int time_cmp(struct timeval * a, struct timeval * b);

#endif
