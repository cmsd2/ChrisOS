#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/thread.h>

// select a runnable thread and switch to it
void scheduler_schedule();

// select a runnable thread 
struct thread * scheduler_next_thread();

// mark a thread as runnable
void scheduler_make_runnable(struct thread * t);

// mark a thread as blocked
void scheduler_make_blocked(struct thread * t);

// add a new thread, initially runnable
void scheduler_add_thread(struct thread * t);

void scheduler_remove_thread(struct thread * t);
void scheduler_remove_runnable_thread(struct thread * t);
void scheduler_remove_blocked_thread(struct thread * t);

#endif
