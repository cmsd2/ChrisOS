#include <sys/process.h>
#include <utlist.h>
#include <mm/vm_space.h>
#include <utils/mem.h>
#include <sys/scheduler.h>
#include <sys/thread.h>
#include <mm/malloc.h>

struct process *_processes;

pid_t _next_pid;

// per-cpu current process pointer
// currently only 1 cpu.
// TODO use more cpus
struct process *_current_proc;

struct process * current_process(void)
{
    return _current_proc;
}

struct process * process_alloc(void) {
    struct process * p;
    p = (struct process *)malloc(sizeof(struct process));
    if(p) {
        memset(p, 0, sizeof(struct process));
    }
    return p;
}

void process_free(struct process *p) {
    free(p);
}

void process_system_init(void) {
}

void process_add_process(struct process *p) {
    LL_PREPEND(_processes, p);
}

void process_remove_process(struct process *p) {
    LL_DELETE(_processes, p);
}

struct process * process_fork(struct process *p) {
    struct process * child = process_alloc();

    child->pid = process_next_pid();
    child->children = 0;

    // copy vm_space, register state etc

    process_add_child(p, child);

    process_add_process(child);

    return child;
}

void process_add_child(struct process *parent, struct process *child) {
    child->parent = parent;
    LL_PREPEND2(parent->children, child, sibling);
}

void process_exit(struct process *p) {
    // kernel_proc (pid 0) can't exit
    assert(p->pid);

    // only kernel_proc doesn't have a parent
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

void process_context_switch(struct thread * t) {
    struct process *proc = t->proc; // null for kthreads

    if(proc && (_current_proc != proc)) {
        _current_proc = proc;

        // TODO switch address space
    }

    thread_context_switch(t);
}

void process_add_thread(struct process *p, struct thread *t) {
    t->proc = p;
    LL_PREPEND(p->threads, t);
}

void process_remove_thread(struct process *p, struct thread *t) {
    LL_DELETE(p->threads, t);
}
