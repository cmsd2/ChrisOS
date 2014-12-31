#include <sys/thread.h>
#include <utlist.h>
#include <utils/mem.h>
#include <sys/process.h>
#include <mm/malloc.h>
#include <arch/stack.h>
#include <sys/scheduler.h>
#include <utils/kprintf.h>
#include <arch/interrupts.h>

struct thread *_free_threads;

tid_t _next_tid;

// TODO make per cpu
struct thread *_current_thread;

struct thread * current_thread(void) {
    return _current_thread;
}

tid_t current_thread_id(void) {
    return _current_thread ? _current_thread->tid : 0;
}

tid_t thread_next_id() {
    _next_tid++;
    if(_next_tid == 0) {
        _next_tid = 1;
    }
    return _next_tid;
}

struct thread * thread_alloc(void) {
    struct thread *t = malloc(sizeof(struct thread));
    if(t) {
        memset(t, 0, sizeof(struct thread));
    }
    return t;
}

void thread_free(struct thread *t) {
    free(t);
}

void thread_system_init(void) {
}

// only place this is called is as a result of
// stack_switch inside scheduler_yield.
// interrupts disabled across context switch.
void thread_entry_point(void * data) {
    if(_current_thread != data) {
        panic("thread is not current thread");
    }

    assert(false == flags_register_is_set(flags_if_bit));
    interrupts_enable();

    int result = _current_thread->func(_current_thread->data);

    thread_exit(_current_thread, result);
}

int thread_init(struct thread * t, thread_func f, void * data) {
    t->state = thread_created;
    t->stack = (uintptr_t)malloc(STACK_SIZE);

    if(!t->stack) {
        return -1;
    }

    t->name = "";
    t->stack_size = STACK_SIZE;
    t->func = f;
    t->data = data;
    t->stack_context = stack_init(t->stack, t->stack_size, thread_entry_point, t);

    return 0;
}

struct thread * thread_spawn_kthread(thread_func f, const char * name, void * data) {
    kprintf("spawning kthread %s\n", name);
    struct thread * t = thread_alloc();
    if(t) {
        if(0 != thread_init(t, f, data)) {
            thread_free(t);
            t = 0;
        } else {
            t->name = name;
            thread_start(t);
        }
    }
    return t;
}

struct thread * thread_spawn(struct process *p, thread_func f, void * data) {
    struct thread * t = thread_alloc();
    if(t) {
        if(0 != thread_init(t, f, data)) {
            thread_free(t);
            t = 0;
        } else {
            process_add_thread(p, t);

            thread_start(t);
        }
    }
    return t;
}

void thread_start(struct thread *t) {
    scheduler_add_thread(t);
}

void thread_exit(struct thread *t, int code) {
    t->exit_code = code;
    t->state = thread_dead;

    scheduler_yield();

    panic("scheduler_yield returned control to dead thread");
}

void current_thread_save_regs(struct registers * regs) {
    struct thread * t = current_thread();
    t->regs = *regs;
}

void current_thread_restore_regs(struct registers * regs) {
    struct thread * t = current_thread();
    *regs = t->regs;
}

void thread_context_switch(struct thread *t) {
    if(_current_thread == t) {
        // ??
        return;
    }

    struct thread * old_thread = _current_thread;
    struct context ** p_old_thread_ctx = 0;

    if(old_thread) {
        p_old_thread_ctx = &old_thread->stack_context;
    }

    _current_thread = t;

    stack_switch(p_old_thread_ctx, t->stack_context);
}

void current_thread_sleep() {
    struct thread * t = current_thread();
    assert(t);

    scheduler_make_blocked(t);

    scheduler_yield();
}

void current_thread_sleep_usecs(useconds_t usecs) {
    struct thread * t = current_thread();

    t->timed_sleep_timer_id =
        timer_schedule_delay(usecs, thread_wake_timer_callback, t);

    kprintf("thread %s going to sleep for %dus. timer_id=%d\n", t->name, usecs, t->timed_sleep_timer_id);

    current_thread_sleep();

    //kprintf("thread %s woken up\n", t->name);

    if(t->timed_sleep_timer_id) {
        kprintf("thread %s cancelling timer id %d. didn't wake up naturally?\n", t->name, t->timed_sleep_timer_id);
        timer_cancel(t->timed_sleep_timer_id);
        t->timed_sleep_timer_id = 0;
    }
}

void thread_wake(struct thread * t) {
    scheduler_make_runnable(t);
}

// timer callback for use with current_thread_sleep_usecs
void thread_wake_timer_callback(timer_id_t timer_id __unused, void * data) {
    struct thread * t = (struct thread *)data;

    kprintf("thread %s got timer callback from timer %d (t->timed_sleep_timer_id=%d t->state=%d). waking up\n", t->name, timer_id, t->timed_sleep_timer_id, t->state);

    t->timed_sleep_timer_id = 0;

    scheduler_print_threads();

    thread_wake(t);

    scheduler_print_threads();
}
