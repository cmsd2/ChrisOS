#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <mm/vm_space.h>

typedef unsigned long pid_t;

struct process {
    struct process *next;

    struct process *parent;
    struct process *children;
    struct process *sibling;

    pid_t pid;
    struct vm_space vm_space;
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



#endif
