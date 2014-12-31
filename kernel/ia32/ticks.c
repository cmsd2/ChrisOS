#include <arch/ticks.h>
#include <arch/interrupts.h>
#include <sys/param.h>
#include <arch/pit.h>
#include <utils/kprintf.h>
#include <arch/apic.h>
#include <arch/pic.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/timer.h>

/*
setup the pit as a frequency divider
one interrupt every 0.01s for scheduling
get actual time by reading back current counter value as needed
 */
static ticks_counter_t _ticks_since_boot;
static uint32_t _tick_freq;
static useconds_t _tick_quantum_usecs;

void ticks_init() {
    hal_install_irq_handler(2 /*PIT_IRQ*/, ticks_irq_handler, NULL, "ticks", NULL);

    unsigned int pit_freq = PIT_FREQUENCY; // Hz
    unsigned int counter = pit_freq / HZ;
    _tick_freq = pit_freq / counter;
    _tick_quantum_usecs = (unsigned int)(1000000 / _tick_freq);
    kprintf("ticks HZ=%d _tick_freq=%d pit_freq=%d counter=%d _tick_quantum_usecs=%d\n", HZ, _tick_freq, pit_freq, counter, _tick_quantum_usecs);
    assert(_tick_quantum_usecs > 0);

    pit_square_wave(pit_channel_0, counter);

    hal_unmask_irq(2);
}

enum hal_fast_irq_handler_result
ticks_irq_handler(uint32_t int_no, struct registers * regs, void * data) {
    _ticks_since_boot++;

    struct timeval now;
    get_time_utc(&now);

    struct timeval diff = {.tv_sec = 0, .tv_usec = _tick_quantum_usecs};

    time_add(&now, &diff);
    set_time_utc(&now);

    timers_advance_clock(_tick_quantum_usecs);

    suseconds_t next_delay = timers_next_delay();
    if(next_delay == 0) {
        timers_wake_thread();
    }

    return IRQ_HANDLED;
}

useconds_t ticks_usecs_since_tick() {
    return 0;
}

ticks_counter_t ticks_since_boot() {
    return _ticks_since_boot;
}

useconds_t ticks_quantum_usecs() {
    return _tick_quantum_usecs;
}

uint32_t ticks_frequency() {
    return _tick_freq;
}
