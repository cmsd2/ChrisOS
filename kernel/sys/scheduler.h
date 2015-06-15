#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/thread.h>

#define TIME_SLICE 1

void scheduler_init(void);

// select a runnable thread and switch to it
void scheduler_yield(void);

// atomically combine make_blocked(current_thread()) and yield()
void scheduler_block(void);

// test and set ptr to value or block until we can.
// this is scheduler specific code to avoid missing wakeup
// should release with __sync_release(ptr)
void scheduler_wait_object_acquire(uint32_t * ptr, uint32_t value);

uint32_t scheduler_lock(void);
void scheduler_unlock(uint32_t flags);

// select a runnable thread
struct thread * scheduler_next_thread(void);

// mark a thread as runnable
void scheduler_make_runnable(struct thread * t);

// mark a thread as blocked
void scheduler_make_blocked(struct thread * t);

// add a new thread, initially runnable
void scheduler_add_thread(struct thread * t);
void scheduler_add_runnable_thread(struct thread * t);
void scheduler_add_blocked_thread(struct thread * t);

void scheduler_remove_thread(struct thread * t);
void scheduler_remove_runnable_thread(struct thread * t);
void scheduler_remove_blocked_thread(struct thread * t);

int scheduler_count_runnable_threads();
int scheduler_count_blocked_threads();
void scheduler_print_threads();

#endif
