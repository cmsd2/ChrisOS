#include <sys/mutex.h>
#include <assert.h>
#include <sys/scheduler.h>
#include <utlist.h>
#include <sys/param.h>
#include <utils/kprintf.h>

bool mutex_trylock(struct mutex * m) {
    bool locked;
    struct thread * me = current_thread();
    assert(me);

    if(__sync_val_compare_and_swap(&m->owner, 0, me->tid) == me->tid) {
        m->nested++;
        locked = true;
    } else {
        locked = false;
    }

    return locked;
}

void mutex_acquire(struct mutex * m) {
    struct thread * me = current_thread();
    assert(me);

    if(__sync_val_compare_and_swap(&m->owner, 0, me->tid) != me->tid) {
        mutex_add_observer(m, me);
        scheduler_wait_object_acquire(&m->owner, me->tid);
        mutex_remove_observer(m, current_thread());
    }

    m->nested++;
}

void mutex_release(struct mutex * m) {
    m->nested--;

    if(m->nested == 0) {
        mutex_release_and_wake_next_thread(m);
    }
}

// assigns ownership to next waiting thread and wakes it
// or releases ownership if no waiting thread
void mutex_release_and_wake_next_thread(struct mutex * m) {
    struct thread * next_thread;

    uint32_t flags = spinlock_acquire(&m->waiting_threads_lock);

    next_thread = m->waiting_threads;

    //mutex_print_info(m);

    if(next_thread) {
        //kprintf("transferring ownership to %s\n", next_thread->name);

        __sync_synchronize();
        m->owner = next_thread->tid;

        scheduler_make_runnable(next_thread);
    } else {
        __sync_lock_release(&m->owner);
    }

    spinlock_release(&m->waiting_threads_lock, flags);
}

void mutex_add_observer(struct mutex * m, struct thread * t) {
    assert(t->wait_queue_next == 0);

    uint32_t flags = spinlock_acquire(&m->waiting_threads_lock);

    DL_APPEND2(m->waiting_threads, t, wait_queue_prev, wait_queue_next);

    spinlock_release(&m->waiting_threads_lock, flags);
}

void mutex_remove_observer(struct mutex * m, struct thread * t) {
    uint32_t flags = spinlock_acquire(&m->waiting_threads_lock);

    DL_DELETE2(m->waiting_threads, t, wait_queue_prev, wait_queue_next);
    t->wait_queue_next = 0;
    t->wait_queue_prev = 0;

    spinlock_release(&m->waiting_threads_lock, flags);
}

void mutex_print_info(struct mutex * m) {
    struct thread * elt;
    size_t count = 0;

    uint32_t flags = spinlock_acquire(&m->waiting_threads_lock);

    DL_FOREACH2(m->waiting_threads, elt, wait_queue_next) {
        kprintf("thread %s waiting 0x%u (prev=0x%u, next=0x%u)\n", elt->name, elt, elt->wait_queue_prev, elt->wait_queue_next);
        count++;
    }

    kprintf("mutex owner=%u nested=%u waiting=%u\n", m->owner, m->nested, count);

    spinlock_release(&m->waiting_threads_lock, flags);
}
