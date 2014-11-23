#include <arch/serial.h>
#include <utils/kprintf.h>
#include <sys/io.h>

void serial_init() {
}

void serial_enable(enum serial_port port) {
    union serial_line_ctrl_reg line_ctrl = { .value = 0 };

    serial_write_reg(port, SERIAL_INT_ENABLE, 0);

    line_ctrl.fields.dlab = dlab_on;
    serial_write_reg(port, SERIAL_LINE_CTRL, line_ctrl.value);

    serial_write_baud(port, 38400);

    line_ctrl.fields.dlab = dlab_off;
    line_ctrl.fields.parity = no_parity;
    line_ctrl.fields.stop_bits = one_stop_bit;
    serial_write_reg(port, SERIAL_LINE_CTRL, line_ctrl.value);

    union serial_fifo_ctrl_reg fifo_ctrl = { .value = 0 };
    fifo_ctrl.fields.enable = 1;
    fifo_ctrl.fields.clear_receive_fifo = 1;
    fifo_ctrl.fields.clear_transmit_fifo = 1;
    fifo_ctrl.fields.int_trigger_level = fourteen_bytes;
    serial_write_reg(port, SERIAL_FIFO_CTRL, fifo_ctrl.value);

    // irqs enabled, rts/dsr set
    union serial_modem_ctrl_reg modem_ctrl = { .value = 0 };
    modem_ctrl.fields.data_terminal_ready = 1;
    modem_ctrl.fields.request_to_send = 1;
    modem_ctrl.fields.int_enable_or_aux_out_2 = 1;

    serial_write_reg(port, SERIAL_MODEM_CTRL, modem_ctrl.value);
}

void serial_write_reg(enum serial_port port, enum serial_port_register reg, uint8_t value) {
    outb(port + reg, value);
}

uint16_t serial_calc_uart_divisor(uint16_t desired_baud) {
    uint16_t result = SERIAL_UART_BAUD / desired_baud;
    if(result == 0) {
        kprintf("spurious baud requested %hd\n", desired_baud);
    }
    return result;
}

void serial_write_baud(enum serial_port port, uint16_t baud) {
    uint16_t divisor = serial_calc_uart_divisor(baud);
    uint8_t d_hi = (uint8_t)(divisor >> 8);
    uint8_t d_lo = (uint8_t)divisor;

    serial_write_reg(port, SERIAL_DATA, d_lo);
    serial_write_reg(port, SERIAL_DATA2, d_hi);
}

