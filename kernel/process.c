#include <process.h>
#include <utlist.h>
#include <mm/vm_space.h>
#include <utils/mem.h>

struct process *_processes;
struct process _proc_zero;

struct process *_free_processes;

pid_t _next_pid;

// per-cpu current process pointer
// currently only 1 cpu.
struct process *_proc_cur;

struct process * current_process(void)
{
    return _proc_cur;
}

struct process * process_alloc(void) {
    if(_free_processes) {
        struct process * p = _free_processes;
        LL_DELETE(_free_processes, _free_processes);
        return p;
    }
    return (struct process *)kalloc_static(sizeof(struct process), 0);
}

void process_free(struct process *p) {
    LL_PREPEND(_free_processes, p);
}

void process_system_init(void) {
    _proc_zero.pid = process_next_pid();
    LL_PREPEND(_processes, &_proc_zero);
    _proc_cur = &_proc_zero;
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
    // proc_zero can't exit
    assert(p->pid);

    // only proc_zero doesn't have a parent
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
