#include <arch/uart.h>
#include <utils/kprintf.h>
#include <sys/io.h>
#include <sys/param.h>
#include <utils/string.h>

char * uart_models[] = {"unknown", "8250", "16450", "16550", "16550A", "16750", "unsupported"};

void uart_init() {
}

void uart_enable(enum uart_port port) {
    uart_lcr_t lcr = 0;

    uart_write_reg(port, UART_INTERRUPT_ENABLE_REG, 0);

    lcr = UART_LCR_SET_DLAB(lcr, uart_lcr_dlab_on);
    uart_write_reg(port, UART_LINE_CTRL_REG, lcr);

    uart_write_baud(port, 38400);

    lcr = UART_LCR_SET_DLAB(lcr, uart_lcr_dlab_off);
    lcr = UART_LCR_SET_PARITY(lcr, uart_lcr_no_parity);
    lcr = UART_LCR_SET_STOP_BITS(lcr, uart_lcr_one_stop_bit);
    uart_write_reg(port, UART_LINE_CTRL_REG, lcr);

    uart_fcr_t fcr = 0;
    fcr = UART_FCR_SET_ENABLED(fcr, 1);
    fcr = UART_FCR_SET_CLEAR_RX_FIFO(fcr, 1);
    fcr = UART_FCR_SET_CLEAR_TX_FIFO(fcr, 1);
    fcr = UART_FCR_SET_INT_TRIGGER_LEVEL(fcr, uart_fcr_fourteen_bytes);
    uart_write_reg(port, UART_FIFO_CTRL_REG, fcr);

    // irqs enabled, rts/dsr set
    uart_mcr_t mcr = 0;
    mcr = UART_MCR_SET_DATA_TERMINAL_READY(mcr, 1);
    mcr = UART_MCR_SET_REQUEST_TO_SEND(mcr, 1);
    mcr = UART_MCR_SET_INT_ENABLE_OR_AUX_OUT_2(mcr, 1);

    uart_write_reg(port, UART_MODEM_CTRL_REG, mcr);
}

void uart_write_reg(enum uart_port port, enum uart_port_register reg, uint8_t value) {
    outb(port + reg, value);
}

uint8_t uart_read_reg(enum uart_port port, enum uart_port_register reg) {
    return inb(port + reg);
}

uint16_t uart_calc_uart_divisor(uint16_t desired_baud) {
    uint16_t result = UART_UART_BAUD / desired_baud;
    if(result == 0) {
        kprintf("spurious baud requested %hd\n", desired_baud);
    }
    return result;
}

void uart_write_baud(enum uart_port port, uint16_t baud) {
    uint16_t divisor = uart_calc_uart_divisor(baud);
    uint8_t d_hi = (uint8_t)(divisor >> 8);
    uint8_t d_lo = (uint8_t)divisor;

    uart_write_reg(port, UART_DIVISOR_LATCH_LSB, d_lo);
    uart_write_reg(port, UART_DIVISOR_LATCH_MSB, d_hi);
}

bool uart_is_transmit_fifo_empty(enum uart_port port) {
    uart_lsr_t lsr = uart_read_reg(port, UART_LINE_STATUS_REG);
    return UART_LSR_GET_THR_EMPTY(lsr) == 1;
}

bool uart_is_receive_fifo_empty(enum uart_port port) {
    uart_lsr_t lsr = uart_read_reg(port, UART_LINE_STATUS_REG);
    return UART_LSR_GET_DATA_AVAILABLE(lsr) == 0;
}

void uart_transmit_fifo_spinwait(enum uart_port port) {
    int i = 0;
    while(!uart_is_transmit_fifo_empty(port)) {
        i++;
    }
}

void uart_receive_fifo_spinwait(enum uart_port port) {
    int i = 0;
    while(uart_is_receive_fifo_empty(port)) {
        i++;
    }
}

void uart_print_info(struct uart_caps * caps) {
    char * model_name;
    model_name = uart_models[MIN((unsigned)uart_unsupported, (unsigned)caps->model)];
    kprintf("uart info model=%s has_fifo=%s scratch_reg=%s rx_size=%hhd tx_size=%hhd\n", model_name, BOOL_TO_STR(caps->has_fifo), BOOL_TO_STR(caps->scratch_reg), caps->receive_fifo_size, caps->transmit_fifo_size);
}

void uart_fingerprint_uart(enum uart_port port, struct uart_caps * result) {
    uart_fcr_t fcr = 0;
    fcr = UART_FCR_SET_ENABLED(fcr, 1);
    fcr = UART_FCR_SET_CLEAR_RX_FIFO(fcr, 1);
    fcr = UART_FCR_SET_CLEAR_TX_FIFO(fcr, 1);
    uart_write_reg(port, UART_FIFO_CTRL_REG, fcr);

    uint8_t scratch = 0x2a;
    uart_write_reg(port, UART_SCRATCH_REG, scratch);
    if(scratch == uart_read_reg(port, UART_SCRATCH_REG)) {
        result->scratch_reg = true;
    } else {
        result->scratch_reg = false;
    }

    uart_iir_t iir = uart_read_reg(port, UART_INTERRUPT_IDENT_REG);

    if(UART_IIR_GET_FIFO_STATUS(iir) == uart_iir_fifo_enabled) {
        result->has_fifo = true;
        if(UART_IIR_GET_64_BYTE_BUFFER_ENABLED(fcr)) {
            result->model = uart_16750;
            result->receive_fifo_size = 64;
            result->transmit_fifo_size = 64;
        } else {
            result->model = uart_16550A;
            result->receive_fifo_size = 16;
            result->transmit_fifo_size = 16;
        }
    } else if(UART_IIR_GET_FIFO_STATUS(fcr) == uart_iir_fifo_reserved_condition) {
        result->model = uart_16550;
        result->has_fifo = true;
        result->receive_fifo_size = 1;
        result->transmit_fifo_size = 1;
    } else {
        result->has_fifo = false;
        if(result->scratch_reg) {
            result->model = uart_16450;
        } else {
            result->model = uart_8250;
        }
    }
}

void uart_putc_sync(enum uart_port port, char c) {
    uart_transmit_fifo_spinwait(port);

    outb(port, c);
}

void uart_puts_sync(enum uart_port port, const char * s) {
    char c = *s;
    while(c) {
        uart_putc_sync(port, c);
        s++;
        c = *s;
    }
}

char uart_getc_sync(enum uart_port port) {
    uart_receive_fifo_spinwait(port);

    return (char)inb(port);
}

