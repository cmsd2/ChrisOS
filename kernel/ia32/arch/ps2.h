#ifndef IA32_ARCH_PS2_H
#define IA32_ARCH_PS2_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/cdefs.h>
#include <arch/registers.h>

#define PS2_DATA_PORT 0x60
#define PS2_SC_PORT 0x64

#define MAX_SCANCODE_BYTES 8

typedef uint8_t ps2_status_reg_t;
#define PS2_STATUS_GET_OUTPUT_BUFFER_STATUS(ps2) getbit(ps2, 0)
#define PS2_STATUS_GET_INPUT_BUFFER_STATUS(ps2) getbit(ps2, 1)
#define PS2_STATUS_GET_SYSTEM_FLAG(ps2) getbit(ps2, 2)
#define PS2_STATUS_GET_IS_COMMAND(ps2) getbit(ps2, 3)
#define PS2_STATUS_GET_MAYBE_KEYBOARD_LOCK(ps2) getbit(ps2, 4)
#define PS2_STATUS_GET_MAYBE_RECEIVE_TIMEOUT(ps2) getbit(ps2, 5)
#define PS2_STATUS_GET_TIMEOUT_ERROR(ps2) getbit(ps2, 6)
#define PS2_STATUS_GET_PARITY_ERROR(ps2) getbit(ps2, 7)

enum ps2_controller_command {
    read_byte_0 = 0x20,
    write_byte_0 = 0x60,
    disable_second_ps2_port = 0xa7,
    enable_second_ps2_port = 0xa8,
    test_second_ps2_port = 0xa9,
    test_ps2_controller = 0xaa,
    test_first_ps2_port = 0xab,
    diagnostic_dump = 0xac,
    disable_first_ps2_port = 0xad,
    enable_first_ps2_port = 0xae,
    read_controller_input_port = 0xc0,
    copy_input_lo_to_status_hi = 0xc1,
    copy_input_hi_to_status_hi = 0xc2,
    read_controller_output_port = 0xd0,
    write_controller_output_port = 0xd1,

    // if 2 ps2 ports:
    write_first_ps2_output_buffer = 0xd2,
    write_second_ps2_output_buffer = 0xd3,
    write_second_ps2_buffer = 0xd4,

    reset_pattern = 0xf0
};

// access with command 0x20 and 0x60
typedef uint8_t ps2_ctrl_cfg_t;
#define PS2_CTRL_CFG_GET_1ST_PS2_INT_ENABLE(ps2) getbit(ps2, 0)
#define PS2_CTRL_CFG_GET_2ND_PS2_INT_ENABLE(ps2) getbit(ps2, 1)
#define PS2_CTRL_CFG_GET_SYSTEM_POST_FLAG(ps2) getbit(ps2, 2)
#define PS2_CTRL_CFG_GET_ZERO_BIT_3(ps2) getbit(ps2, 3)
#define PS2_CTRL_CFG_GET_1ST_PS2_CLOCK_DISABLE(ps2) getbit(ps2, 4)
#define PS2_CTRL_CFG_GET_2ND_PS2_CLOCK_DISABLE(ps2) getbit(ps2, 5)
#define PS2_CTRL_CFG_GET_1ST_PS2_TRANSLATION_ENABLE(ps2) getbit(ps2, 6)
#define PS2_CTRL_CFG_GET_ZERO_BIT_7(ps2) getbit(ps2, 7)
#define PS2_CTRL_CFG_SET_1ST_PS2_INT_ENABLE(ps2, value) withbit(ps2, 0, value)
#define PS2_CTRL_CFG_SET_2ND_PS2_INT_ENABLE(ps2, value) withbit(ps2, 1, value)
#define PS2_CTRL_CFG_SET_SYSTEM_POST_FLAG(ps2, value) withbit(ps2, 2, value)
#define PS2_CTRL_CFG_SET_ZERO_BIT_3(ps2, value) withbit(ps2, 3, value)
#define PS2_CTRL_CFG_SET_1ST_PS2_CLOCK_DISABLE(ps2, value) withbit(ps2, 4, value)
#define PS2_CTRL_CFG_SET_2ND_PS2_CLOCK_DISABLE(ps2, value) withbit(ps2, 5, value)
#define PS2_CTRL_CFG_SET_1ST_PS2_TRANSLATION_ENABLE(ps2, value) withbit(ps2, 6, value)
#define PS2_CTRL_CFG_SET_ZERO_BIT_7(ps2, value) withbit(ps2, 7, value)

// access using commands 0xd0 and 0xd1
typedef uint8_t ps2_ctrl_output_t;
#define PS2_CTRL_OUTPUT_GET_SYSTEM_RESET(ps2) getbit(ps2, 0)
#define PS2_CTRL_OUTPUT_GET_A20(ps2) getbit(ps2, 1)
#define PS2_CTRL_OUTPUT_GET_2ND_PS2_CLOCK(ps2) getbit(ps2, 2)
#define PS2_CTRL_OUTPUT_GET_2ND_PS2_DATA(ps2) getbit(ps2, 3)
#define PS2_CTRL_OUTPUT_GET_1ST_PS2_OUTPUT_BUFFER_FULL(ps2) getbit(ps2, 4)
#define PS2_CTRL_OUTPUT_GET_2ND_PS2_OUTPUT_BUFFER_FULL(ps2) getbit(ps2, 5)
#define PS2_CTRL_OUTPUT_GET_1ST_PS2_CLOCK(ps2) getbit(ps2, 6)
#define PS2_CTRL_OUTPUT_GET_1ST_PS2_DATA(ps2) getbit(ps2, 7)

struct ps2_async_command;

typedef void (*ps2_async_callback)(enum ps2_controller_command cmd, uint8_t data);

struct ps2_async_command {
    uint8_t cmd;
    uint8_t data;
    bool send_data;
    ps2_async_callback callback;
    struct ps2_async_command * next;
    struct ps2_async_command * prev;
};

struct ps2_async_command * ps2_send_command(struct ps2_async_command * queue, uint8_t cmd, uint8_t data, ps2_async_callback callback);
bool ps2_cancel_command(struct ps2_async_command * queue, struct ps2_async_command * cmd);

void ps2_init(void);
bool ps2_probe(void);
void ps2_wait_output_buffer_sync(void);
uint8_t ps2_read_data(void);
bool ps2_can_read_data(void);
uint8_t ps2_read_status(void);
void ps2_write_data(uint8_t data);
void ps2_write_command(uint8_t command);
void ps2_flush_output_buffer(void); // flush ps2 controller's output buffer by reading from it and discarding everything
uint8_t ps2_read_config(void);
void ps2_write_config(uint8_t config);
struct ps2_async_command * ps2_command_alloc(void);
void ps2_command_free(struct ps2_async_command * cmd);

enum hal_fast_irq_handler_result
ps2_kbd_interrupt_handler(uint32_t int_no, struct registers * regs, void * data);

int ps2_kbd_thread(void * data);
void ps2_kbd_issue_next_command(void);
bool ps2_kbd_handle_response(void);
bool ps2_kbd_handle_scancode(uint8_t * scancode, size_t length);
void ps2_kbd_print_scancode(uint8_t * scancode, size_t length);

enum ps2_driver_state {
    waiting_command,
    waiting_response,
    waiting_scancode_start,
    waiting_scancode_end
};

#endif
