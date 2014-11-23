#include <sys/thread.h>
#include <utlist.h>
#include <utils/mem.h>
#include <sys/process.h>

struct thread *_free_threads;

tid_t _next_tid;

// TODO make per cpu
struct thread *_current_thread;

struct thread * current_thread(void) {
    return _current_thread;
}

struct thread * thread_alloc(void) {
    struct thread *t;
    if(_free_threads) {
        t = _free_threads;
        LL_DELETE(_free_threads, t);
    } else {
        t = (struct thread *)kalloc_static(sizeof(struct thread), 0);
    }
    memset(t, 0, sizeof(struct thread));
    return t;
}

void thread_free(struct thread *t) {
    LL_PREPEND(_free_threads, t);
}

void thread_system_init(void) {
}

struct thread * thread_spawn(struct process *p) {
    struct thread * t = thread_alloc();
    t->state = thread_created;
    process_add_thread(p, t);
}

void thread_exit(struct thread *t, int code) {
    t->exit_code = code;
    t->state = thread_dead;
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
    if(_current_thread != t) {
        _current_thread = t;
    }
}

