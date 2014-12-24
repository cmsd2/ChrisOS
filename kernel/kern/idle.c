#include <kern/idle.h>
#include <sys/standard.h>
#include <sys/cdefs.h>
#include <utils/kprintf.h>

/*
 * idle process
 *
 * is always ready to run
 * does nothing but put the cpu to sleep
 */

static struct thread * _idle_thread;

int idle_thread_start(void) {
    _idle_thread = thread_spawn_kthread(idle_thread_func, NULL);

    if(!_idle_thread) {
        return -1;
    } else {
        return 0;
    }
}

/* doesn't return */
int idle_thread_func(void * data __unused) {
    while(1) {
        kprintf("hello from the idle thread!\n"); //TODO remove this or it'll generate a lot of noise
        __asm__("hlt");
    }
}
