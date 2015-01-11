#ifndef SYS_MUTEX_H
#define SYS_MUTEX_H

#include <sys/thread.h>
#include <stdbool.h>
#include <sys/spinlock.h>

struct mutex {
    tid_t owner;
    uint32_t nested;
    struct thread * waiting_threads;
    struct spinlock waiting_threads_lock;
};

bool mutex_trylock(struct mutex * m);
void mutex_acquire(struct mutex * m);
void mutex_release(struct mutex * m);
void mutex_remove_observer(struct mutex * m, struct thread * t);
void mutex_add_observer(struct mutex * m, struct thread * t);
void mutex_release_and_wake_next_thread(struct mutex * m);
void mutex_print_info(struct mutex * m);

#endif
