#include <arch/pit.h>
#include <utils/panic.h>
#include <assert.h>
#include <sys/io.h>
#include <arch/ps2.h>
#include <sys/standard.h>
#include <utils/kprintf.h>

void pit_set_mode(enum pit_select_channel channel, enum pit_access_mode access_mode, enum pit_operating_mode mode) {
    unsigned short port = PIT_CMD_PORT;
    assert(channel >= pit_channel_0 && channel <= pit_channel_2);

    union pit_mode_cmd_reg mode_reg;
    mode_reg.fields.bcd_binary_mode = pit_binary_mode;
    mode_reg.fields.operating_mode = mode;
    mode_reg.fields.access_mode = access_mode;
    mode_reg.fields.select_channel = channel;

    //kprintf("writing %hhx to pit port %hx\n", mode_reg.value, port);
    outb(port, mode_reg.value);
}

void pit_set_counter(enum pit_select_channel channel, uint8_t value) {
    unsigned short port = pit_io_port(channel);

    //kprintf("setting counter to %hhx on port %hx\n", value, port);
    outb(port, value);
}

unsigned short pit_io_port(enum pit_select_channel channel) {
    switch(channel) {
    case pit_channel_0:
        return PIT_CH0_PORT;
    case pit_channel_1:
        return PIT_CH1_PORT;
    case pit_channel_2:
        return PIT_CH2_PORT;
    default:
        panic("unknown pit channel");
    }
}

void pit_one_shot(enum pit_select_channel channel, unsigned short ticks) {
    pit_set_mode(channel, pit_low_high_byte, pit_mode_0_interrupt_on_terminal_count);

    pit_set_counter(channel, LOW_BYTE(ticks));
    io_wait(); // delay
    pit_set_counter(channel, HIGH_BYTE(ticks));
}
