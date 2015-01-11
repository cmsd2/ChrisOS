#ifndef SYS_SPINLOCK_H
#define SYS_SPINLOCK_H

#include <sys/thread.h>
#include <stdint.h>

struct spinlock {
    tid_t owner; // 0 for not locked
    uint32_t nested;
};

void spinlock_init(struct spinlock * lock);
uint32_t spinlock_acquire(struct spinlock * lock);
void spinlock_release(struct spinlock * lock, uint32_t flags);

#endif
