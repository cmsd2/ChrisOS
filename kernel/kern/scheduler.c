#include <sys/scheduler.h>
#include <utlist.h>
#include <sys/thread.h>
#include <sys/process.h>
#include <utils/kprintf.h>
#include <arch/interrupts.h>
#include <sys/spinlock.h>
#include <arch/ticks.h>

struct thread * _runnable_threads;
struct thread * _blocked_threads;
struct spinlock _sched_lock;
uint32_t _last_scheduled_time_ticks;

void scheduler_init() {
    spinlock_init(&_sched_lock);
}

uint32_t scheduler_lock() {
    return spinlock_acquire(&_sched_lock);
}

void scheduler_unlock(uint32_t flags) {
    spinlock_release(&_sched_lock, flags);
}

// simple round-robin scheduler with no priorities
// can be called from IRQ context
// note there are 2 ways to leave this function:
// one in a straight line, as coded.
// the other is inside process_context_switch which
// will end up executing stack_switch and then
// thread_entry_point.
void scheduler_yield() {
    uint32_t flags1 = interrupts_enter_cli();

    uint32_t flags2 = scheduler_lock();

    struct thread * new_thread = scheduler_next_thread();

    scheduler_unlock(flags2);

    if(!new_thread) {
        panic("no thread to switch to");
    } else if(new_thread != current_thread()) {
        process_context_switch(new_thread);
    } else {
        //
    }

    interrupts_leave_cli(flags1);
}

void scheduler_wait_object_acquire(uint32_t * ptr, uint32_t value) {
    struct thread * next_thread;
    uint32_t flags1 = interrupts_enter_cli();
    uint32_t flags2 = scheduler_lock();

    while(__sync_val_compare_and_swap(ptr, 0, value) != value) {
        scheduler_make_blocked(current_thread());

        next_thread = scheduler_next_thread();
        assert(next_thread);

        scheduler_unlock(flags2);

        process_context_switch(next_thread);

        flags2 = scheduler_lock();
    }

    scheduler_unlock(flags2);
    interrupts_leave_cli(flags1);
}

void scheduler_block() {
    uint32_t flags1 = interrupts_enter_cli();
    uint32_t flags2 = scheduler_lock();

    scheduler_make_blocked(current_thread());

    scheduler_unlock(flags2);

    scheduler_yield();
    interrupts_leave_cli(flags1);
}

struct thread * scheduler_next_thread() {
    struct thread * old_thread = current_thread();

    uint32_t previous_ticks = _last_scheduled_time_ticks;
    uint32_t current_ticks = _last_scheduled_time_ticks = ticks_since_boot();
    uint32_t elapsed_ticks = current_ticks - previous_ticks;

    if(old_thread) {
        // should always be non null except for initial bootstrap
        old_thread->time_slice -= elapsed_ticks;

        if(old_thread->state == thread_running) {
            if(old_thread->time_slice > 0) {
                return old_thread;
            } else {
                old_thread->state = thread_runnable;
            }
        }
    }

    struct thread * next_thread = _runnable_threads;
    if(!next_thread) {
        scheduler_print_threads();
        kprintf("old thread: %s state=%d\n", old_thread ? old_thread->name : "", old_thread ? old_thread->state : -1);
        panic("no next thread available");
    }

    /*    if(old_thread != next_thread) {
        kprintf("switching from %s to %s\n", old_thread ? old_thread->name : "", next_thread->name);
        }*/

    // move to back
    scheduler_remove_runnable_thread(next_thread);
    scheduler_add_runnable_thread(next_thread);

    next_thread->state = thread_running;

    return next_thread;
}

void scheduler_make_runnable(struct thread * t) {
    uint32_t flags = scheduler_lock();

    //kprintf("making thread runnable %s\n", t->name ? t->name : "");

    if(t->state != thread_runnable) {
        scheduler_remove_thread(t);

        t->state = thread_runnable;

        scheduler_add_runnable_thread(t);
    }

    scheduler_unlock(flags);
}

void scheduler_make_blocked(struct thread * t) {
    uint32_t flags = scheduler_lock();

    if(t->state != thread_blocked) {
        scheduler_remove_thread(t);

        t->state = thread_blocked;

        assert(t != _blocked_threads);
        scheduler_add_blocked_thread(t);
    }

    scheduler_unlock(flags);
}

void scheduler_add_thread(struct thread * t) {
    uint32_t flags = scheduler_lock();
    assert(t->state != thread_running);

    if(t->state == thread_created) {
        t->state = thread_runnable;
    }

    if(t->state == thread_runnable) {
        assert(t != _runnable_threads);
        scheduler_add_runnable_thread(t);
    } else {
        assert(t != _blocked_threads);
        scheduler_add_blocked_thread(t);
    }

    scheduler_unlock(flags);
}

void scheduler_remove_thread(struct thread * t) {
    if(t->state == thread_running || t->state == thread_runnable) {
        scheduler_remove_runnable_thread(t);
    } else {
        scheduler_remove_blocked_thread(t);
    }
}

void scheduler_add_runnable_thread(struct thread * t) {
    assert(t != _runnable_threads);
    t->time_slice = TIME_SLICE;
    DL_APPEND2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_add_blocked_thread(struct thread * t) {
    assert(t != _blocked_threads);
    DL_APPEND2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
}

void scheduler_remove_runnable_thread(struct thread * t) {
    DL_DELETE2(_runnable_threads, t, scheduler_thread_prev, scheduler_thread_next);
    t->scheduler_thread_next = 0;
    t->scheduler_thread_prev = 0;
}

void scheduler_remove_blocked_thread(struct thread * t) {
    DL_DELETE2(_blocked_threads, t, scheduler_thread_prev, scheduler_thread_next);
    t->scheduler_thread_next = 0;
    t->scheduler_thread_prev = 0;
}

int scheduler_count_runnable_threads() {
    uint32_t flags = scheduler_lock();
    struct thread * t;
    int count = 0;
    DL_COUNT2(_runnable_threads, t, count, scheduler_thread_next);
    scheduler_unlock(flags);
    return count;
}

int scheduler_count_blocked_threads() {
    uint32_t flags = scheduler_lock();
    struct thread * t;
    int count = 0;
    DL_COUNT2(_blocked_threads, t, count, scheduler_thread_next);
    scheduler_unlock(flags);
    return count;
}

void scheduler_print_threads() {
    uint32_t flags = scheduler_lock();

    struct thread * t;
    kprintf("runnable: ");
    DL_FOREACH2(_runnable_threads, t, scheduler_thread_next) {
        kprintf("%s 0x%lx (prev=0x%lx, next=0x%lx)\n", t->name, t, t->scheduler_thread_prev, t->scheduler_thread_next);
    }
    kprintf("\n");

    kprintf("blocked: ");
    DL_FOREACH2(_blocked_threads, t, scheduler_thread_next) {
        kprintf("%s 0x%lx (prev=0x%lx, next=0x%lx)\n", t->name, t, t->scheduler_thread_prev, t->scheduler_thread_next);
    }
    kprintf("\n");

    scheduler_unlock(flags);
}
