#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <mm/vm_space.h>
#include <arch/registers.h>

typedef unsigned long pid_t;

enum process_state {
    process_created = 0,
    process_alive,
    process_dead
};

struct thread;

struct process {
    struct process *next;
    struct thread *threads;

    struct process *parent;
    struct process *children;
    struct process *sibling;

    pid_t pid;
    struct vm_space vm_space;

    enum process_state state;

    int exit_code;
};

extern struct process *_processes;
extern struct process _kernel_proc;

// returns the process running on the current cpu
struct process * current_process(void);

struct process * process_alloc(void);
void process_free(struct process *p);

void process_system_init(void);

void process_add_process(struct process *p);
void process_remove_process(struct process *p);

struct process * process_fork(struct process *p);
void process_add_child(struct process *parent, struct process *child);

void process_exit(struct process *p);

pid_t process_next_pid(void);

void process_context_switch(struct thread * t);

void process_add_thread(struct process *p, struct thread *t);
void process_remove_thread(struct process *p, struct thread *t);

#endif
