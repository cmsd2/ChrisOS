#include <sys/scheduler.h>
#include <utlist.h>
#include <sys/thread.h>
#include <sys/process.h>

struct thread * _runnable_threads;
struct thread * _blocked_threads;

// simple round-robin scheduler with no priorities

void scheduler_schedule() {
    struct thread * t = scheduler_next_thread();

    process_context_switch(t);
}

struct thread * scheduler_next_thread() {
    struct thread * t = _runnable_threads;

    scheduler_remove_runnable_thread(t);

    t->state = thread_running;

    return t;
}

void scheduler_make_runnable(struct thread * t) {
    scheduler_remove_thread(t);

    t->state = thread_runnable;

    DL_APPEND2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_make_blocked(struct thread * t) {
    scheduler_remove_thread(t);

    t->state = thread_blocked;

    DL_APPEND2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_add_thread(struct thread * t) {
    scheduler_make_runnable(t);
}

void scheduler_remove_thread(struct thread * t) {
    switch(t->state) {
    case thread_created:
    case thread_running:
        break;
    case thread_runnable:
        scheduler_remove_runnable_thread(t);
        break;
    case thread_blocked:
        scheduler_remove_blocked_thread(t);
        break;
    }
}


void scheduler_remove_runnable_thread(struct thread * t) {
    DL_DELETE2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_remove_blocked_thread(struct thread * t) {
    DL_DELETE2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

