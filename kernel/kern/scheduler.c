#include <sys/scheduler.h>
#include <utlist.h>
#include <sys/process.h>

struct process * _runnable_procs;
struct process * _blocked_procs;

// simple round-robin scheduler with no priorities

void scheduler_schedule() {
    struct process * proc = scheduler_next_process();

    process_context_switch(proc);
}

struct process * scheduler_next_process() {
    struct process * proc = _runnable_procs;

    scheduler_remove_runnable_process(proc);

    proc->state = running;

    return proc;
}

void scheduler_make_runnable(struct process * proc) {
    scheduler_remove_process(proc);

    proc->state = runnable;

    DL_APPEND2(_runnable_procs, proc, scheduler_proc_prev, scheduler_proc_next);
}

void scheduler_make_blocked(struct process * proc) {
    scheduler_remove_process(proc);

    proc->state = blocked;

    DL_APPEND2(_blocked_procs, proc, scheduler_proc_prev, scheduler_proc_next);
}

void scheduler_add_process(struct process * proc) {
    scheduler_make_runnable(proc);
}

void scheduler_remove_process(struct process * proc) {
    switch(proc->state) {
    case creating:
    case running:
        break;
    case runnable:
        scheduler_remove_runnable_process(proc);
        break;
    case blocked:
        scheduler_remove_blocked_process(proc);
        break;
    }
}


void scheduler_remove_runnable_process(struct process * proc) {
    DL_DELETE2(_runnable_procs, proc, scheduler_proc_prev, scheduler_proc_next);
}

void scheduler_remove_blocked_process(struct process * proc) {
    DL_DELETE2(_blocked_procs, proc, scheduler_proc_prev, scheduler_proc_next);
}

