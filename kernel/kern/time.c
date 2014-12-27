#include <sys/time.h>

static struct timeval _time_now_utc;

void get_time_utc(struct timeval * tv) {
    *tv = _time_now_utc;
}

void set_time_utc(struct timeval * tv) {
    _time_now_utc = *tv;
}

void time_add(struct timeval * dest, struct timeval * diff) {
    struct timeval temp = *diff;
    time_normalise(dest);
    time_normalise(&temp);
    dest->tv_sec += diff->tv_sec;
    dest->tv_usec += diff->tv_usec;
}

void time_normalise(struct timeval * tv) {
    long usecs_per_sec = 1000000;
    time_t s = tv->tv_usec / usecs_per_sec;
    time_t us = tv->tv_usec % usecs_per_sec;
    tv->tv_sec += s;
    tv->tv_usec = us;
    if(tv->tv_usec < 0) {
        tv->tv_usec += usecs_per_sec;
        tv->tv_sec--;
    }
}

int time_cmp(struct timeval * a, struct timeval * b) {
    struct timeval tmp_a = *a;
    struct timeval tmp_b = *b;

    time_normalise(&tmp_a);
    time_normalise(&tmp_b);

    if(a->tv_sec < b->tv_sec) {
        return -1;
    }
    if(a->tv_sec > b->tv_sec) {
        return 1;
    }
    return a->tv_usec - b->tv_usec;
}
