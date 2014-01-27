#ifndef KERN_THREAD_H
#define KERN_THREAD_H

#include <arch/registers.h>

typedef unsigned int tid_t;

enum thread_state {
    thread_created = 0,
    thread_running,
    thread_runnable,
    thread_blocked,
    thread_dead
};

struct process;

struct thread {
    struct thread *next;
    struct process *proc;

    // for scheduler's list of runnable/blocked processes
    struct thread *scheduler_thread_next;
    struct thread *scheduler_thread_prev;

    tid_t tid;

    // for resume after interrupt
    struct registers regs;

    enum thread_state state;

    int exit_code;
};

struct thread * current_thread(void);

struct thread * thread_alloc(void);
void thread_free(struct thread *t);

void thread_system_init(void);

struct thread * thread_spawn(struct process *p);
void thread_exit(struct thread *t, int code);

void current_thread_save_regs(struct registers * regs);
void current_thread_restore_regs(struct registers * regs);

void thread_context_switch(struct thread *t);

#endif
