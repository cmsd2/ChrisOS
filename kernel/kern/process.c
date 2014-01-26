#include <sys/process.h>
#include <utlist.h>
#include <mm/vm_space.h>
#include <utils/mem.h>
#include <sys/scheduler.h>

struct process *_processes;
struct process _idle_proc;

struct process *_free_processes;

pid_t _next_pid;

// per-cpu current process pointer
// currently only 1 cpu.
// TODO use more cpus
struct process *_proc_cur;

struct process * current_process(void)
{
    return _proc_cur;
}

struct process * process_alloc(void) {
    struct process * p;
    if(_free_processes) {
        p = _free_processes;
        LL_DELETE(_free_processes, _free_processes);
    } else {
        p = (struct process *)kalloc_static(sizeof(struct process), 0);
    }
    kmemset(p, 0, sizeof(struct process));
    return p;
}

void process_free(struct process *p) {
    LL_PREPEND(_free_processes, p);
}

void process_system_init(void) {
    _idle_proc.pid = process_next_pid();
    LL_PREPEND(_processes, &_idle_proc);
    _proc_cur = &_idle_proc;
    scheduler_add_process(&_idle_proc);
}

struct process * process_fork(struct process *p) {
    struct process * child = process_alloc();

    child->pid = process_next_pid();
    child->children = 0;

    // copy vm_space, register state etc

    process_add_child(p, child);

    LL_PREPEND(_processes, child);

    return child;
}

void process_add_child(struct process *parent, struct process *child) {
    child->parent = parent;
    LL_PREPEND2(parent->children, child, sibling);
}

void process_exit(struct process *p) {
    // idle_proc (pid 0) can't exit
    assert(p->pid);

    // only idle_proc doesn't have a parent
    assert(p->parent);

    struct process *child, *tmp;
    LL_FOREACH_SAFE2(p->children, child, tmp, sibling) {
        LL_DELETE(p->children, child);
        process_add_child(p->parent, child);
    }
}

//TODO: use a tree
bool process_pid_available(pid_t pid) {
    struct process *p;
    LL_FOREACH(_processes, p) {
        if(p->pid == pid) {
            return false;
        }
    }
    return true;
}

//TODO: fix slow and stupid loop
pid_t process_next_pid(void) {
    pid_t result;
    do {
        result = _next_pid++;
    } while(process_pid_available(result) == false);
}

void current_process_save_regs(struct registers * regs) {
    struct process * p = current_process();
    p->regs = *regs;
}

void current_process_restore_regs(struct registers * regs) {
    struct process * p = current_process();
    *regs = p->regs;
}

void process_context_switch(struct process * proc) {
    if(_proc_cur != proc) {
        _proc_cur = proc;
    }
}

