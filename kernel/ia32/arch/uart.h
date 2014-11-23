#ifndef IA32_ARCH_UART_H
#define IA32_ARCH_UART_H

// see http://www.lammertbies.nl/comm/info/uart-uart.html

#include <stdint.h>
#include <stdbool.h>

#define UART_UART_BAUD 115200

enum uart_model {
    uart_unknown = 0,
    uart_8250,
    uart_16450,
    uart_16550,
    uart_16550A,
    uart_16750,
    uart_unsupported
};
// indexed by enum uart_model
extern char * uart_models[];

struct uart_caps {
    enum uart_model model;
    bool has_fifo;
    bool scratch_reg;
    uint8_t receive_fifo_size;
    uint8_t transmit_fifo_size;
};

enum uart_port_register {
    UART_RECEIVER_BUFFER_REG = 0,
    UART_TRANSMIT_HOLDING_REG = 0,
    UART_DIVISOR_LATCH_LSB = 0,

    UART_INTERRUPT_ENABLE_REG = 1,
    UART_DIVISOR_LATCH_MSB = 1,

    UART_INTERRUPT_IDENT_REG = 2,
    UART_FIFO_CTRL_REG = 2,

    UART_LINE_CTRL_REG = 3,
    UART_MODEM_CTRL_REG = 4,
    UART_LINE_STATUS_REG = 5,
    UART_MODEM_STATUS_REG = 6,
    UART_SCRATCH_REG = 7
};

enum uart_port {
    UART_COM1 = 0x3f8,
    UART_COM2 = 0x2f8,
    UART_COM3 = 0x3e8,
    UART_COM4 = 0x2e8
};

enum uart_line_ctrl_dlab {
    dlab_on = 0x1,
    dlab_off = 0x0
};

enum uart_line_ctrl_char_size {
    five_bits = 0x0,
    six_bits = 0x1,
    seven_bits = 0x2,
    eight_bits = 0x3
};

enum uart_line_ctrl_stop_bits {
    one_stop_bit = 0x0,
    one_and_half_stop_bits = 0x1,
    two_stop_bits = 0x1
};

enum uart_line_ctrl_parity {
    no_parity = 0x0,
    odd_parity = 0x1,
    even_parity = 0x3,
    mark_parity = 0x5,
    space_parity = 0x7
};

union uart_line_ctrl_reg {
    uint8_t value;
    struct {
        uint8_t char_size : 2;
        uint8_t stop_bits : 1;
        uint8_t parity : 3;
        uint8_t reserved_6 : 1;
        uint8_t dlab : 1;
    } fields;
};

enum uart_line_ctrl_value {
    UART_LINE_DLAB = dlab_on << 7,
    UART_LINE_CHAR_SIZE_MASK = 0x3,
    UART_LINE_5BIT = five_bits,
    UART_LINE_6BIT = six_bits,
    UART_LINE_7BIT = seven_bits,
    UART_LINE_8BIT = eight_bits,
    UART_LINE_STOP_BITS = two_stop_bits << 2,
    UART_LINE_PARITY_MASK = 0x7 << 3,
    UART_LINE_PARITY_NONE = no_parity << 3,
    UART_LINE_PARITY_ODD = odd_parity << 3,
    UART_LINE_PARITY_EVEN = even_parity << 3,
    UART_LINE_PARITY_MARK = mark_parity << 3,
    UART_LINE_PARITY_SPACE = space_parity << 3
};

union uart_interrupt_enable_reg {
    uint8_t value;
    struct {
        uint8_t data_available : 1;
        uint8_t transmitter_empty : 1;
        uint8_t break_or_error : 1;
        uint8_t status_change : 1;
        uint8_t unused : 4;
    } fields;
};

enum uart_int_cause {
    modem_status_change = 0, // check modem status reg
    thr_empty = 1, // can write to transmit fifo
    recvd_data_available = 2, // can read from receive fifo
    line_status_change = 3, // check line status reg
    char_timeout = 6 // can read from receive fifo but fifo is not full
};

enum uart_int_fifo_status {
    no_fifo = 0,
    fifo_reserved_condition = 1,
    unusable_fifo = 2,
    fifo_enabled = 3
};

union uart_int_id_reg {
    uint8_t value;
    struct {
        uint8_t interrupt_pending : 1;
        uint8_t cause : 3;
        uint8_t reserved_4 : 1;
        uint8_t sixty_four_byte_buffer_enabled : 1;
        uint8_t fifo_status : 2;
    } fields;
};

enum uart_fifo_int_trigger_level {
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

union uart_fifo_ctrl_reg {
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

union uart_modem_ctrl_reg {
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

union uart_line_status_reg {
    uint8_t value;
    struct {
        uint8_t data_available : 1;
        uint8_t overrun_error : 1;
        uint8_t parity_error : 1;
        uint8_t framing_error : 1;
        uint8_t break_signal_recvd : 1;
        uint8_t thr_empty : 1;
        uint8_t thr_empty_line_idle : 1;
        uint8_t erroneous_data : 1;
    } fields;
};

union uart_modem_status_reg {
    uint8_t value;
    struct {
        uint8_t change_in_clear_to_send : 1;
        uint8_t change_in_data_set_ready : 1;
        uint8_t trailing_edge_ring_indicator : 1;
        uint8_t change_in_carrier_detect : 1;
        uint8_t clear_to_send : 1;
        uint8_t data_set_ready : 1;
        uint8_t ring_indicator : 1;
        uint8_t carrier_detect : 1;
    } fields;
};

void uart_init(void);
void uart_enable(enum uart_port port);
uint8_t uart_read_reg(enum uart_port port, enum uart_port_register reg);
void uart_write_reg(enum uart_port port, enum uart_port_register reg, uint8_t value);
uint16_t uart_calc_uart_divisor(uint16_t desired_baud);
void uart_write_baud(enum uart_port port, uint16_t baud);
bool uart_is_transmit_fifo_empty(enum uart_port port);
void uart_transmit_fifo_spinwait(enum uart_port port);
void uart_fingerprint_uart(enum uart_port port, struct uart_caps * result);
void uart_print_info(struct uart_caps * caps);
void uart_putc_sync(enum uart_port port, char c);
void uart_puts_sync(enum uart_port port, const char * s);

#endif
