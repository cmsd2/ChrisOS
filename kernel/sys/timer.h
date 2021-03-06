#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include <sys/time.h>
#include <sys/cdefs.h>
#include <stdbool.h>

typedef long timer_id_t;

typedef void (*timer_callback)(timer_id_t id, void * data);

//TODO change from linked list to heap

struct timer_at_time {
    timer_id_t id;
    struct timeval time; // seconds since epoch
    struct timer_at_time * next;
    struct timer_at_time * prev;
    timer_callback callback;
    void * data;
};

struct timer_at_offset {
    timer_id_t id;
    suseconds_t delay; // microseconds from now
    struct timer_at_offset * next;
    struct timer_at_offset * prev;
    timer_callback callback;
    void * data;
};

void timers_init(void);

uint32_t timers_lock();
void timers_unlock(uint32_t flags);

timer_id_t timer_next_id(void);

struct timer_at_time * timer_at_time_alloc();
struct timer_at_offset * timer_at_offset_alloc();
void timer_at_time_free(struct timer_at_time * t);
void timer_at_offset_free(struct timer_at_offset * t);

int timer_far_timers_cmp(struct timer_at_time * a, struct timer_at_time * b);
int timer_near_timers_cmp(struct timer_at_offset * a, struct timer_at_offset * b);

timer_id_t timer_schedule_at(struct timeval * time, timer_callback callback, void * data);
timer_id_t timer_schedule_delay(suseconds_t useconds, timer_callback callback, void * data);
struct timer_at_offset * timer_schedule_delay_with_id(timer_id_t id, suseconds_t usecs, timer_callback callback, void * data);
bool timer_cancel(timer_id_t id);
int timer_thread(void * data) _Noreturn;

bool timers_handle_near_timers(void);
suseconds_t timers_next_delay();
void timers_update_far_timers();
void timers_convert_far_to_near(struct timer_at_time * ft);

void timers_advance_clock(useconds_t usecs);
void timers_wake_thread(void);

#endif
