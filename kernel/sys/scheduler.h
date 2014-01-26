#ifndef SCHEDULER_H
#define SCHEDULER_H

// select a runnable process and switch to it
void scheduler_schedule();

// select a runnable process
struct process * scheduler_next_process();

// mark a process as runnable
void scheduler_make_runnable(struct process * proc);

// mark a process as blocked
void scheduler_make_blocked(struct process * proc);

// add a new process, initially runnable
void scheduler_add_process(struct process * proc);

void scheduler_remove_process(struct process * proc);
void scheduler_remove_runnable_process(struct process * proc);
void scheduler_remove_blocked_process(struct process * proc);

#endif
