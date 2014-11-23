#include <arch/uart.h>
#include <utils/kprintf.h>
#include <sys/io.h>
#include <sys/standard.h>
#include <utils/string.h>

char * uart_models[] = {"unknown", "8250", "16450", "16550", "16550A", "16750", "unsupported"};

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
    return status_reg.fields.thr_empty == 1;
}

void uart_transmit_fifo_spinwait(enum uart_port port) {
    int i = 0;
    while(!uart_is_transmit_fifo_empty(port)) {
        i++;
    }
}

void uart_print_info(struct uart_caps * caps) {
    char * model_name;
    model_name = uart_models[MIN((unsigned)uart_unsupported, (unsigned)caps->model)];
    kprintf("uart info model=%s has_fifo=%s scratch_reg=%s rx_size=%hhd tx_size=%hhd\n", model_name, BOOL_TO_STR(caps->has_fifo), BOOL_TO_STR(caps->scratch_reg), caps->receive_fifo_size, caps->transmit_fifo_size);
}

void uart_fingerprint_uart(enum uart_port port, struct uart_caps * result) {
    union uart_fifo_ctrl_reg fifo_ctrl = { .value = 0 };
    fifo_ctrl.fields.enable = 1;
    fifo_ctrl.fields.clear_receive_fifo = 1;
    fifo_ctrl.fields.clear_transmit_fifo = 1;
    uart_write_reg(port, UART_FIFO_CTRL_REG, fifo_ctrl.value);

    uint8_t scratch = 0x2a;
    uart_write_reg(port, UART_SCRATCH_REG, scratch);
    if(scratch == uart_read_reg(port, UART_SCRATCH_REG)) {
        result->scratch_reg = true;
    } else {
        result->scratch_reg = false;
    }

    union uart_int_id_reg int_id;
    int_id.value = uart_read_reg(port, UART_INTERRUPT_IDENT_REG);

    if(int_id.fields.fifo_status == fifo_enabled) {
        result->has_fifo = true;
        if(int_id.fields.sixty_four_byte_buffer_enabled) {
            result->model = uart_16750;
            result->receive_fifo_size = 64;
            result->transmit_fifo_size = 64;
        } else {
            result->model = uart_16550A;
            result->receive_fifo_size = 16;
            result->transmit_fifo_size = 16;
        }
    } else if(int_id.fields.fifo_status == fifo_reserved_condition) {
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

