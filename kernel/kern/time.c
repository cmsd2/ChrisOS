#include <sys/time.h>
#include <arch/interrupts.h>
#include <sys/spinlock.h>

static struct timeval _time_now_utc;
static struct spinlock _time_lock;

void time_init() {
    spinlock_init(&_time_lock);
}

uint32_t time_lock() {
    return spinlock_acquire(&_time_lock);
}

void time_unlock(uint32_t flags) {
    spinlock_release(&_time_lock, flags);
}

void get_time_utc(struct timeval * tv) {
    uint32_t flags = time_lock();

    *tv = _time_now_utc;

    time_unlock(flags);
}

void set_time_utc(struct timeval * tv) {
    uint32_t flags = time_lock();

    _time_now_utc = *tv;

    time_unlock(flags);
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
