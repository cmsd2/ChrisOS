#ifndef KERN_THREAD_H
#define KERN_THREAD_H

#include <arch/registers.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/timer.h>

#define STACK_SIZE 4096

typedef uint32_t tid_t;

enum thread_state {
    thread_created = 0,
    thread_running,
    thread_runnable,
    thread_blocked,
    thread_dead
};

typedef int (*thread_func)(void * data);

struct process;

struct thread {
    struct thread * next;
    struct process * proc; // null for kernel threads

    // for scheduler's list of runnable/blocked processes
    struct thread * scheduler_thread_next;
    struct thread * scheduler_thread_prev;

    // for waiting on mutexes, file handles, whatever.
    struct thread * wait_queue_next;
    struct thread * wait_queue_prev;

    const char * name;

    tid_t tid;

    // for return after interrupt servicing
    struct registers regs;

    // stack state for actual context switch
    struct context * stack_context;
    uintptr_t stack;
    size_t stack_size;

    thread_func func;
    void * data;

    timer_id_t timed_sleep_timer_id;

    enum thread_state state;

    int exit_code;
};

struct thread * current_thread(void);
tid_t current_thread_id(void);

struct thread * thread_alloc(void);
void thread_free(struct thread *t);

void thread_system_init(void);
void thread_entry_point(void * data) _Noreturn;

struct thread * thread_spawn_kthread(thread_func f, const char * name, void * data);
struct thread * thread_spawn(struct process *p, thread_func f, void * data);
void thread_start(struct thread *t);
int thread_init(struct thread *t, thread_func f, void * data);
void thread_exit(struct thread *t, int code) __attribute__ ((noreturn));

void current_thread_save_regs(struct registers * regs);
void current_thread_restore_regs(struct registers * regs);

void thread_context_switch(struct thread *t);

void current_thread_sleep(void);
void current_thread_sleep_usecs(useconds_t usecs);
void thread_wake(struct thread *t);

void thread_wake_timer_callback(timer_id_t timer_id, void * data);

#endif
