#include <sys/scheduler.h>
#include <utlist.h>
#include <sys/thread.h>
#include <sys/process.h>

struct thread * _runnable_threads;
struct thread * _blocked_threads;

// simple round-robin scheduler with no priorities

void scheduler_yield() {
    struct thread * new_thread = scheduler_next_thread();

    if(new_thread) {
        process_context_switch(new_thread);
    } else {
        //
    }
}

struct thread * scheduler_next_thread() {
    struct thread * old_thread = current_thread();

    if(old_thread) {
        // should always be non null except for initial bootstrap

        if(old_thread->state == thread_running) {
            old_thread->state = thread_runnable;
        }

        scheduler_add_thread(old_thread);
    }

    struct thread * next_thread = _runnable_threads;
    assert(next_thread != NULL);

    scheduler_remove_runnable_thread(next_thread);

    next_thread->state = thread_running;

    return next_thread;
}

void scheduler_make_runnable(struct thread * t) {
    if(t->state != thread_runnable) {
        scheduler_remove_thread(t);

        t->state = thread_runnable;

        scheduler_add_runnable_thread(t);
    }
}

void scheduler_make_blocked(struct thread * t) {
    if(t->state != thread_blocked) {
        scheduler_remove_thread(t);

        t->state = thread_blocked;

        scheduler_add_blocked_thread(t);
    }
}

void scheduler_add_thread(struct thread * t) {
    if(t->state != thread_created) {
        panic("invalid new thread state");
    }

    t->state = thread_runnable;

    scheduler_add_runnable_thread(t);
}

void scheduler_remove_thread(struct thread * t) {
    if(t->state == thread_runnable) {
        scheduler_remove_runnable_thread(t);
    } else {
        scheduler_remove_blocked_thread(t);
    }
}

void scheduler_add_runnable_thread(struct thread * t) {
    DL_APPEND2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_add_blocked_thread(struct thread * t) {
    DL_APPEND2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_remove_runnable_thread(struct thread * t) {
    DL_DELETE2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_remove_blocked_thread(struct thread * t) {
    DL_DELETE2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
}
