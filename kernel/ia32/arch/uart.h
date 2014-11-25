#ifndef IA32_ARCH_UART_H
#define IA32_ARCH_UART_H

// see http://www.lammertbies.nl/comm/info/uart-uart.html

#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>

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

enum uart_lcr_dlab {
    uart_lcr_dlab_on = 0x1,
    uart_lcr_dlab_off = 0x0
};

enum uart_lcr_char_size {
    uart_lcr_five_bits = 0x0,
    uart_lcr_six_bits = 0x1,
    uart_lcr_seven_bits = 0x2,
    uart_lcr_eight_bits = 0x3
};

enum uart_lcr_stop_bits {
    uart_lcr_one_stop_bit = 0x0,
    uart_lcr_one_and_half_stop_bits = 0x1,
    uart_lcr_two_stop_bits = 0x1
};

enum uart_lcr_parity {
    uart_lcr_no_parity = 0x0,
    uart_lcr_odd_parity = 0x1,
    uart_lcr_even_parity = 0x3,
    uart_lcr_mark_parity = 0x5,
    uart_lcr_space_parity = 0x7
};

typedef uint8_t uart_lcr_t;
#define UART_LCR_GET_CHAR_SIZE(uart) getbits(uart, 0, 1)
#define UART_LCR_GET_STOP_BITS(uart) getbit(uart, 2)
#define UART_LCR_GET_PARITY(uart) getbits(uart, 3, 5)
#define UART_LCR_GET_BREAK_SIG_DISABLED(uart) getbit(uart, 6)
#define UART_LCR_GET_DLAB(uart) getbit(uart, 7)
#define UART_LCR_SET_CHAR_SIZE(uart, value) withbits(uart, 0, 1, value)
#define UART_LCR_SET_STOP_BITS(uart, value) withbit(uart, 2, value)
#define UART_LCR_SET_PARITY(uart, value) withbits(uart, 3, 5, value)
#define UART_LCR_SET_BREAK_SIG_DISABLED(uart, value) withbit(uart, 6, value)
#define UART_LCR_SET_DLAB(uart, value) withbit(uart, 7, value)

typedef uint8_t uart_ier_t;
#define UART_IER_GET_DATA_AVAILABLE(uart) getbit(uart, 0)
#define UART_IER_GET_THR_EMPTY(uart) getbit(uart, 1)
#define UART_IER_GET_LSR_CHANGE(uart) getbit(uart, 2)
#define UART_IER_GET_MSR_CHANGE(uart) getbit(uart, 3)
#define UART_IER_GET_SLEEP_MODE(uart) getbit(uart, 4)
#define UART_IER_GET_LOW_POWER_MODE(uart) getbit(uart, 5)
#define UART_IER_SET_DATA_AVAILABLE(uart, value) withbit(uart, 0, value)
#define UART_IER_SET_THR_EMPTY(uart, value) withbit(uart, 1, value)
#define UART_IER_SET_LSR_CHANGE(uart, value) withbit(uart, 2, value)
#define UART_IER_SET_MSR_CHANGE(uart, value) withbit(uart, 3, value)
#define UART_IER_SET_SLEEP_MODE(uart, value) withbit(uart, 4, value)
#define UART_IER_SET_LOW_POWER_MODE(uart, value) withbit(uart, 5, value)

enum uart_iir_cause {
    uart_iir_modem_status_change = 0, // check modem status reg
    uart_iir_thr_empty = 1, // can write to transmit fifo
    uart_iir_recvd_data_available = 2, // can read from receive fifo
    uart_iir_line_status_change = 3, // check line status reg
    uart_iir_char_timeout = 6 // can read from receive fifo but fifo is not full
};

enum uart_iir_fifo_status {
    uart_iir_no_fifo = 0,
    uart_iir_fifo_reserved_condition = 1,
    uart_iir_unusable_fifo = 2,
    uart_iir_fifo_enabled = 3
};

typedef uint8_t uart_iir_t;
#define UART_IIR_GET_INT_PENDING(uart) getbit(uart, 0)
#define UART_IIR_GET_CAUSE(uart) getbits(uart, 1, 3)
#define UART_IIR_GET_64_BYTE_BUFFER_ENABLED(uart) getbit(uart, 5)
#define UART_IIR_GET_FIFO_STATUS(uart) getbits(uart, 6, 7)
#define UART_IIR_SET_INT_PENDING(uart, value) withbit(uart, 0, value)
#define UART_IIR_SET_CAUSE(uart, value) withbits(uart, 1, 3, value)
#define UART_IIR_SET_64_BYTE_BUFFER_ENABLED(uart, value) withbit(uart, 5, value)
#define UART_IIR_SET_FIFO_STATUS(uart, value) withbits(uart, 6, 7, value)

enum uart_fcr_int_trigger_level {
    uart_fcr_one_byte = 0x0,
    
    // 16 byte buffers:
    uart_fcr_four_bytes = 0x1,
    uart_fcr_eight_bytes = 0x2,
    uart_fcr_fourteen_bytes = 0x3,

    // 64 byte buffers:
    uart_fcr_sixteen_bytes = 0x1,
    uart_fcr_thirty_two_bytes = 0x2,
    uart_fcr_fifty_six_bytes = 0x3
};

typedef uint8_t uart_fcr_t;
#define UART_FCR_GET_ENABLED(uart) getbit(uart, 0)
#define UART_FCR_GET_CLEAR_RX_FIFO(uart) getbit(uart, 1)
#define UART_FCR_GET_CLEAR_TX_FIFO(uart) getbit(uart, 2)
#define UART_FCR_GET_DMA(uart) getbit(uart, 3)
#define UART_FCR_GET_FIFO_SIZE(uart) getbit(uart, 5)
#define UART_FCR_GET_INT_TRIGGER_LEVEL(uart) getbits(uart, 6, 7)
#define UART_FCR_SET_ENABLED(uart, value) withbit(uart, 0, value)
#define UART_FCR_SET_CLEAR_RX_FIFO(uart, value) withbit(uart, 1, value)
#define UART_FCR_SET_CLEAR_TX_FIFO(uart, value) withbit(uart, 2, value)
#define UART_FCR_SET_DMA(uart, value) withbit(uart, 3, value)
#define UART_FCR_SET_FIFO_SIZE(uart, value) withbit(uart, 5, value)
#define UART_FCR_SET_INT_TRIGGER_LEVEL(uart, value) withbits(uart, 6, 7, value)

typedef uint8_t uart_mcr_t;
#define UART_MCR_GET_DATA_TERMINAL_READY(uart) getbit(uart, 0)
#define UART_MCR_GET_REQUEST_TO_SEND(uart) getbit(uart, 1)
#define UART_MCR_GET_UNUSED_AUX_OUT_1(uart) getbit(uart, 2)
#define UART_MCR_GET_INT_ENABLE_OR_AUX_OUT_2(uart) getbit(uart, 3)
#define UART_MCR_GET_UNUSED_LOOPBACK_MODE(uart) getbit(uart, 4)
#define UART_MCR_GET_UNUSED_AUTOFLOW_CTRL(uart) getbit(uart, 5)
#define UART_MCR_SET_DATA_TERMINAL_READY(uart, value) withbit(uart, 0, value)
#define UART_MCR_SET_REQUEST_TO_SEND(uart, value) withbit(uart, 1, value)
#define UART_MCR_SET_UNUSED_AUX_OUT_1(uart, value) withbit(uart, 2, value)
#define UART_MCR_SET_INT_ENABLE_OR_AUX_OUT_2(uart, value) withbit(uart, 3, value)
#define UART_MCR_SET_UNUSED_LOOPBACK_MODE(uart, value) withbit(uart, 4, value)
#define UART_MCR_SET_UNUSED_AUTOFLOW_CTRL(uart, value) withbit(uart, 5, value)

typedef uint8_t uart_lsr_t;
#define UART_LSR_GET_DATA_AVAILABLE(uart) getbit(uart, 0)
#define UART_LSR_GET_OVERRUN_ERROR(uart) getbit(uart, 1)
#define UART_LSR_GET_PARITY_ERROR(uart) getbit(uart, 2)
#define UART_LSR_GET_FRAMING_ERROR(uart) getbit(uart, 3)
#define UART_LSR_GET_BREAK_SIG_RECVD(uart) getbit(uart, 4)
#define UART_LSR_GET_THR_EMPTY(uart) getbit(uart, 5)
#define UART_LSR_GET_THR_EMPTY_LINE_IDLE(uart) getbit(uart, 6)
#define UART_LSR_GET_ERRONEOUS_DATA(uart) getbit(uart, 7)
#define UART_LSR_SET_DATA_AVAILABLE(uart, value) withbit(uart, 0, value)
#define UART_LSR_SET_OVERRUN_ERROR(uart, value) withbit(uart, 1, value)
#define UART_LSR_SET_PARITY_ERROR(uart, value) withbit(uart, 2, value)
#define UART_LSR_SET_FRAMING_ERROR(uart, value) withbit(uart, 3, value)
#define UART_LSR_SET_BREAK_SIG_RECVD(uart, value) withbit(uart, 4, value)
#define UART_LSR_SET_THR_EMPTY(uart, value) withbit(uart, 5, value)
#define UART_LSR_SET_THR_EMPTY_LINE_IDLE(uart, value) withbit(uart, 6, value)
#define UART_LSR_SET_ERRONEOUS_DATA(uart, value) withbit(uart, 7, value)

typedef uint8_t uart_msr_t;
#define UART_MSR_GET_CHANGE_IN_CLEAR_TO_SEND(uart) getbit(uart, 0)
#define UART_MSR_GET_CHANGE_IN_DATA_SET_READY(uart) getbit(uart, 1)
#define UART_MSR_GET_TRAILING_EDGE_RING_INDICATOR(uart) getbit(uart, 2)
#define UART_MSR_GET_CHANGE_IN_CARRIER_DETECT(uart) getbit(uart, 3)
#define UART_MSR_GET_CLEAR_TO_SEND(uart) getbit(uart, 4)
#define UART_MSR_GET_DATA_SET_READY(uart) getbit(uart, 5)
#define UART_MSR_GET_RING_INDICATOR(uart) getbit(uart, 6)
#define UART_MSR_GET_CARRIER_DETECT(uart) getbit(uart, 7)
#define UART_MSR_SET_CHANGE_IN_CLEAR_TO_SEND(uart, value) getbit(uart, 0, value)
#define UART_MSR_SET_CHANGE_IN_DATA_SET_READY(uart, value) getbit(uart, 1, value)
#define UART_MSR_SET_TRAILING_EDGE_RING_INDICATOR(uart, value) getbit(uart, 2, value)
#define UART_MSR_SET_CHANGE_IN_CARRIER_DETECT(uart, value) getbit(uart, 3, value)
#define UART_MSR_SET_CLEAR_TO_SEND(uart, value) getbit(uart, 4, value)
#define UART_MSR_SET_DATA_SET_READY(uart, value) getbit(uart, 5, value)
#define UART_MSR_SET_RING_INDICATOR(uart, value) getbit(uart, 6, value)
#define UART_MSR_SET_CARRIER_DETECT(uart, value) getbit(uart, 7, value)

void uart_init(void);
void uart_enable(enum uart_port port);
uint8_t uart_read_reg(enum uart_port port, enum uart_port_register reg);
void uart_write_reg(enum uart_port port, enum uart_port_register reg, uint8_t value);
uint16_t uart_calc_uart_divisor(uint16_t desired_baud);
void uart_write_baud(enum uart_port port, uint16_t baud);
bool uart_is_transmit_fifo_empty(enum uart_port port);
bool uart_is_receive_fifo_empty(enum uart_port port);
void uart_transmit_fifo_spinwait(enum uart_port port);
void uart_receive_fifo_spinwait(enum uart_port port);
void uart_fingerprint_uart(enum uart_port port, struct uart_caps * result);
void uart_print_info(struct uart_caps * caps);
void uart_putc_sync(enum uart_port port, char c);
void uart_puts_sync(enum uart_port port, const char * s);
char uart_getc_sync(enum uart_port port);

#endif
