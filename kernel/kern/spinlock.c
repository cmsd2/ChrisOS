#include <sys/spinlock.h>
#include <arch/interrupts.h>
#include <sys/thread.h>

void spinlock_init(struct spinlock * lock) {
    lock->locked = 0;
}

// nestable spinlocks. interrupts disabled while held
uint32_t spinlock_acquire(struct spinlock * lock) {
    uint32_t flags = interrupts_enter_cli();
    volatile uint32_t * locked = &lock->locked;
    tid_t me = current_thread_id();

    //TODO: is this right?
    if(lock->owner != me) {
        while(*locked) {
            // spin
        }
    }

    lock->locked++;
    lock->owner = me;

    return flags;
}

void spinlock_release(struct spinlock * lock, uint32_t flags) {
    lock->locked--;
    lock->owner = 0;

    interrupts_leave_cli(flags);
}
