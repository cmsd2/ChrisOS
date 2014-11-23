#include <arch/uart.h>
#include <utils/kprintf.h>
#include <sys/io.h>

void uart_init() {
}

void uart_enable(enum uart_port port) {
    union uart_line_ctrl_reg line_ctrl = { .value = 0 };

    uart_write_reg(port, UART_INTERRUPT_ENABLE_REG, 0);

    line_ctrl.fields.dlab = dlab_on;
    uart_write_reg(port, UART_LINE_CTRL_REG, line_ctrl.value);

    uart_write_baud(port, 38400);

    line_ctrl.fields.dlab = dlab_off;
    line_ctrl.fields.parity = no_parity;
    line_ctrl.fields.stop_bits = one_stop_bit;
    uart_write_reg(port, UART_LINE_CTRL_REG, line_ctrl.value);

    union uart_fifo_ctrl_reg fifo_ctrl = { .value = 0 };
    fifo_ctrl.fields.enable = 1;
    fifo_ctrl.fields.clear_receive_fifo = 1;
    fifo_ctrl.fields.clear_transmit_fifo = 1;
    fifo_ctrl.fields.int_trigger_level = fourteen_bytes;
    uart_write_reg(port, UART_FIFO_CTRL_REG, fifo_ctrl.value);

    // irqs enabled, rts/dsr set
    union uart_modem_ctrl_reg modem_ctrl = { .value = 0 };
    modem_ctrl.fields.data_terminal_ready = 1;
    modem_ctrl.fields.request_to_send = 1;
    modem_ctrl.fields.int_enable_or_aux_out_2 = 1;

    uart_write_reg(port, UART_MODEM_CTRL_REG, modem_ctrl.value);
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
    union uart_line_status_reg status_reg;
    status_reg.value = uart_read_reg(port, UART_LINE_STATUS_REG);
    return status_reg.fields.thr_empty == 0;
}

void uart_transmit_fifo_spinwait(enum uart_port port) {
    int i = 0;
    while(!uart_is_transmit_fifo_empty(port)) {
        i++;
    }
}

