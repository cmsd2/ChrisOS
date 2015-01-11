#include <sys/spinlock.h>
#include <arch/interrupts.h>
#include <sys/thread.h>
#include <assert.h>
#include <utils/mem.h>

void spinlock_init(struct spinlock * lock) {
    memset(lock, 0, sizeof(struct spinlock));
}

// nestable spinlocks. interrupts disabled while held
uint32_t spinlock_acquire(struct spinlock * lock) {
    uint32_t flags = interrupts_enter_cli();
    tid_t me = current_thread_id();
    assert(me);

    while(__sync_val_compare_and_swap(&lock->owner, 0, me) != me) {
        // spin
    }

    lock->nested++;

    return flags;
}

void spinlock_release(struct spinlock * lock, uint32_t flags) {
    assert(false == flags_register_is_set(flags_if_bit));

    lock->nested--;

    if(lock->nested == 0) {
        __sync_lock_release(&lock->owner);
    }

    interrupts_leave_cli(flags);
}
