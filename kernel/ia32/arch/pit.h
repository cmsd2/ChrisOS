#ifndef IA32_ARCH_PIT_H
#define IA32_ARCH_PIT_H

#include <stdint.h>

#define PIT_CH0_PORT 0x40
#define PIT_CH1_PORT 0x41
#define PIT_CH2_PORT 0x42
#define PIT_CMD_PORT 0x43

typedef uint8_t pit_mode_cmd_t;
#define PIT_MODE_CMD_GET_BCD_BINARY_MODE(pit) getbit(pit, 0)
#define PIT_MODE_CMD_GET_OPERATING_MODE(pit) getbits(pit, 1, 3)
#define PIT_MODE_CMD_GET_ACCESS_MODE(pit) getbits(pit, 4, 5)
#define PIT_MODE_CMD_GET_SELECT_CHANNEL(pit) getbits(pit, 6, 7)
#define PIT_MODE_CMD_SET_BCD_BINARY_MODE(pit, value) withbit(pit, 0, value)
#define PIT_MODE_CMD_SET_OPERATING_MODE(pit, value) withbits(pit, 1, 3, value)
#define PIT_MODE_CMD_SET_ACCESS_MODE(pit, value) withbits(pit, 4, 5, value)
#define PIT_MODE_CMD_SET_SELECT_CHANNEL(pit, value) withbits(pit, 6, 7, value)

enum pit_select_channel {
    pit_channel_0 = 0,
    pit_channel_1 = 1,
    pit_channel_2 = 2,
    pit_read_back = 3
};

enum pit_access_mode {
    pit_latch_count_value = 0,
    pit_low_byte = 1,
    pit_high_byte = 2,
    pit_low_high_byte = 3
};

enum pit_operating_mode {
    pit_mode_0_interrupt_on_terminal_count = 0,
    pit_mode_1_programmable_one_shot = 1,
    pit_mode_2_rate_generator = 2,
    pit_mode_3_square_wave_generator = 3,
    pit_mode_4_software_strobe = 4,
    pit_mode_5_hardware_strobe = 5,
    pit_mode_2b_rate_generator = 6,
    pit_mode_3b_square_wave_generator = 7
};

enum pit_bcd_binary_mode {
    pit_binary_mode = 0,
    pit_bcd_mode = 1
};

void pit_set_mode(enum pit_select_channel channel, enum pit_access_mode access_mode, enum pit_operating_mode mode);

void pit_set_counter(enum pit_select_channel channel, uint8_t value);

unsigned short pit_io_port(enum pit_select_channel channel);

void pit_one_shot(enum pit_select_channel channel, unsigned short ticks);

#endif
