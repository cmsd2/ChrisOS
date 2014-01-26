#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <mm/vm_space.h>
#include <arch/registers.h>

typedef unsigned long pid_t;

enum process_state {
    creating = 0,
    running,
    runnable,
    blocked
};

struct process {
    struct process *next;

    struct process *parent;
    struct process *children;
    struct process *sibling;

    // for scheduler's list of runnable/blocked processes
    struct process *scheduler_proc_next;
    struct process *scheduler_proc_prev;

    pid_t pid;
    struct vm_space vm_space;

    // for resume after interrupt
    struct registers regs;

    enum process_state state;
};

extern struct process *_processes;
extern struct process _proc_zero;

// returns the process running on the current cpu
struct process * current_process(void);

struct process * process_alloc(void);
void process_free(struct process *p);

void process_system_init(void);

struct process * process_fork(struct process *p);
void process_add_child(struct process *parent, struct process *child);

void process_exit(struct process *p);

pid_t process_next_pid(void);

void current_process_save_regs(struct registers * regs);
void current_process_restore_regs(struct registers * regs);

void process_context_switch(struct process * proc);

#endif
