#include <sys/timer.h>
#include <mm/malloc.h>
#include <utlist.h>
#include <sys/scheduler.h>
#include <utils/kprintf.h>

#define NEAR_THRESHOLD_SECS 10

static timer_id_t _next_timer_id;

static struct timer_at_time * _far_timers;
static struct timer_at_offset * _near_timers;
static struct thread * _timer_thread;

void timers_init() {
    _timer_thread = thread_spawn_kthread(timer_thread, "timer", NULL);
    assert(_timer_thread);
}

timer_id_t timer_next_id() {
    ++_next_timer_id;
    if(_next_timer_id < 0) {
        _next_timer_id = 1;
    }
    return _next_timer_id;
}

struct timer_at_time * timer_at_time_alloc() {
    struct timer_at_time * t = (struct timer_at_time *)malloc(sizeof(struct timer_at_time));
    return t;
}

struct timer_at_offset * timer_at_offset_alloc() {
    struct timer_at_offset * t = (struct timer_at_offset *)malloc(sizeof(struct timer_at_offset));
    return t;
}

void timer_at_time_free(struct timer_at_time * t) {
    free(t);
}

void timer_at_offset_free(struct timer_at_offset * t) {
    free(t);
}

int timer_far_timers_cmp(struct timer_at_time * a, struct timer_at_time * b) {
    assert(a);
    assert(b);
    return time_cmp(&a->time, &b->time);
}

int timer_near_timers_cmp(struct timer_at_offset * a, struct timer_at_offset * b) {
    assert(a);
    assert(b);
    if(a->delay < b->delay) {
        return -1;
    }
    if(a->delay > b->delay) {
        return 1;
    }
    return 0;
}

timer_id_t timer_schedule_at(struct timeval * time, timer_callback callback) {
    assert(time);
    struct timer_at_time * timer = timer_at_time_alloc();
    if(timer) {
        timer->id = timer_next_id();
        timer->time = *time;
        timer->callback = callback;
        DL_PREPEND(_far_timers, timer);
        DL_SORT(_far_timers, timer_far_timers_cmp);
        return timer->id;
    } else {
        return -1;
    }
}

struct timer_at_offset * timer_schedule_delay_with_id(timer_id_t id, suseconds_t usecs, timer_callback callback) {
    struct timer_at_offset * timer = timer_at_offset_alloc();
    if(timer) {
        timer->id = id;
        timer->delay = usecs;
        timer->callback = callback;
        DL_PREPEND(_near_timers, timer);
        DL_SORT(_near_timers, timer_near_timers_cmp);
        return timer;
    } else {
        return NULL;
    }
}

timer_id_t timer_schedule_delay(suseconds_t useconds, timer_callback callback) {
    timer_id_t id = timer_next_id();

    struct timer_at_offset * timer = timer_schedule_delay_with_id(id, useconds, callback);
    if(timer) {
        return id;
    } else {
        return -1;
    }
}

bool timer_cancel(timer_id_t id) {
    struct timer_at_offset *nt;
    struct timer_at_time *ft;

    DL_FOREACH(_near_timers, nt) {
        if(nt->id == id) {
            DL_DELETE(_near_timers, nt);
            return true;
        }
    }

    DL_FOREACH(_far_timers, ft) {
        if(ft->id == id) {
            DL_DELETE(_far_timers, ft);
            return true;
        }
    }

    return false;
}

int timer_thread(void * data __unused) {
    while(1) {
        while(timers_handle_near_timers()) {
            // nothing
        }

        // will be woken up by ticks fast irq handler
        current_thread_sleep();
    }
}

// runs in IRQ context
// returns -1 if no timers waiting
// returns 0 if next timer is due now or since we last checked
// returns number of microseconds until next timer due otherwise
suseconds_t timers_next_delay() {
    struct timer_at_offset * timer = _near_timers;

    if(timer) {
        if(timer->delay < 0) {
            return 0;
        } else {
            return timer->delay;
        }
    } else {
        return -1;
    }
}

bool timers_handle_near_timers(void) {
    struct timer_at_offset * timer = _near_timers;

    if(timer && timer->delay <= 0) {
        if(timer->callback) {
            timer->callback(timer->id);
        }
        timer_cancel(timer->id);

        return true;
    } else {
        return false;
    }
}

void timers_update_far_timers() {
    struct timeval now;
    get_time_utc(&now);

    struct timeval near_threshold = now;
    struct timeval near_threshold_diff = {.tv_sec = NEAR_THRESHOLD_SECS, .tv_usec = 0};
    time_add(&near_threshold, &near_threshold_diff);

    struct timer_at_time *ft, *tmp;
    DL_FOREACH_SAFE(_far_timers, ft, tmp) {
        if(time_cmp(&ft->time, &near_threshold) <= 0) {
            timers_convert_far_to_near(ft);
        } else {
            return;
        }
    }
}

void timers_convert_far_to_near(struct timer_at_time * ft) {
    struct timeval now;
    get_time_utc(&now);

    suseconds_t usecs = (ft->time.tv_sec - now.tv_sec) * 1000000;
    usecs += ft->time.tv_usec - now.tv_usec;

    struct timer_at_offset * nt;

    DL_DELETE(_far_timers, ft);
    nt = timer_schedule_delay_with_id(ft->id, usecs, ft->callback);
}

// runs in IRQ context
void timers_advance_clock(useconds_t usecs) {
    struct timer_at_offset * nt;
    DL_FOREACH(_near_timers, nt) {
        nt->delay -= usecs;
    }

    timers_update_far_timers();
}

// runs in IRQ context
void timers_wake_thread() {
    assert(_timer_thread);
    thread_wake(_timer_thread);
}
