#ifndef IA32_ARCH_SERIAL_H
#define IA32_ARCH_SERIAL_H

#include <stdint.h>

#define SERIAL_UART_BAUD 115200

enum serial_port_register {
    SERIAL_DATA = 0,
    SERIAL_DATA2 = 1,
    SERIAL_INT_ENABLE = 1,
    SERIAL_FIFO_CTRL = 2,
    SERIAL_LINE_CTRL = 3,
    SERIAL_MODEM_CTRL = 4,
    SERIAL_LINE_STATUS = 5,
    SERIAL_MODEM_STATUS = 6,
    SERIAL_SCRATCH = 7
};

enum serial_port {
    SERIAL_COM1 = 0x3f8,
    SERIAL_COM2 = 0x2f8,
    SERIAL_COM3 = 0x3e8,
    SERIAL_COM4 = 0x2e8
};

enum serial_line_ctrl_dlab {
    dlab_on = 0x1,
    dlab_off = 0x0
};

enum serial_line_ctrl_char_size {
    five_bits = 0x0,
    six_bits = 0x1,
    seven_bits = 0x2,
    eight_bits = 0x3
};

enum serial_line_ctrl_stop_bits {
    one_stop_bit = 0x0,
    one_and_half_stop_bits = 0x1,
    two_stop_bits = 0x1
};

enum serial_line_ctrl_parity {
    no_parity = 0x0,
    odd_parity = 0x1,
    even_parity = 0x3,
    mark_parity = 0x5,
    space_parity = 0x7
};

union serial_line_ctrl_reg {
    uint8_t value;
    struct {
        uint8_t char_size : 2;
        uint8_t stop_bits : 1;
        uint8_t parity : 3;
        uint8_t reserved_6 : 1;
        uint8_t dlab : 1;
    } fields;
};

enum serial_line_ctrl_value {
    SERIAL_LINE_DLAB = dlab_on << 7,
    SERIAL_LINE_CHAR_SIZE_MASK = 0x3,
    SERIAL_LINE_5BIT = five_bits,
    SERIAL_LINE_6BIT = six_bits,
    SERIAL_LINE_7BIT = seven_bits,
    SERIAL_LINE_8BIT = eight_bits,
    SERIAL_LINE_STOP_BITS = two_stop_bits << 2,
    SERIAL_LINE_PARITY_MASK = 0x7 << 3,
    SERIAL_LINE_PARITY_NONE = no_parity << 3,
    SERIAL_LINE_PARITY_ODD = odd_parity << 3,
    SERIAL_LINE_PARITY_EVEN = even_parity << 3,
    SERIAL_LINE_PARITY_MARK = mark_parity << 3,
    SERIAL_LINE_PARITY_SPACE = space_parity << 3
};

union serial_interrupt_enable_reg {
    uint8_t value;
    struct {
        uint8_t data_available : 1;
        uint8_t transmitter_empty : 1;
        uint8_t break_or_error : 1;
        uint8_t status_change : 1;
        uint8_t unused : 4;
    } fields;
};

enum serial_fifo_int_trigger_level {
    one_byte = 0x0,
    
    // 16 byte buffers:
    four_bytes = 0x1,
    eight_bytes = 0x2,
    fourteen_bytes = 0x3,

    // 64 byte buffers:
    sixteen_bytes = 0x1,
    thirty_two_bytes = 0x2,
    fifty_six_bytes = 0x3
};

union serial_fifo_ctrl_reg {
    uint8_t value;
    struct {
        uint8_t enable : 1;
        uint8_t clear_receive_fifo : 1;
        uint8_t clear_transmit_fifo : 1;
        uint8_t dma : 1;
        uint8_t reserved : 1;
        uint8_t fifo_size : 1;
        uint8_t int_trigger_level : 2;
    } fields;
};

union serial_modem_ctrl_reg {
    uint8_t value;
    struct {
        uint8_t data_terminal_ready : 1;
        uint8_t request_to_send : 1;
        uint8_t unused_aux_out_1 : 1;
        uint8_t int_enable_or_aux_out_2 : 1;
        uint8_t unused_loopback_mode : 1;
        uint8_t unused_autoflow_ctrl : 1;
        uint8_t reserved_6_7 : 2;
    } fields;
};

void serial_init(void);
void serial_enable(enum serial_port port);
void serial_write_reg(enum serial_port port, enum serial_port_register reg, uint8_t value);
uint16_t serial_calc_uart_divisor(uint16_t desired_baud);
void serial_write_baud(enum serial_port port, uint16_t baud);

#endif
