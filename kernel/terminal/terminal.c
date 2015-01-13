#include <terminal/terminal.h>
#include <utils/string.h>
#include <boot/layout.h>
#include <arch/uart.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t serial_console_port;

void terminal_enable_serial_console(uint16_t port) {
    serial_console_port = port;
}

void terminal_disable_serial_console() {
    serial_console_port = 0;
}

void terminal_clear(void)
{
    int viewport_height = vga_height();
    int viewport_width = vga_width();

    for ( size_t y = 0; y < viewport_height; y++ )
    {
        for ( size_t x = 0; x < viewport_width; x++ )
        {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
}

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_make_color(COLOR_LIGHT_GREY, COLOR_BLACK);

    terminal_clear();
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    vga_put_char(c, color, x, y);
}

// clear row y starting in column x
void terminal_clear_line_end(size_t x, size_t y) {
    int viewport_width = vga_width();
    while(x < viewport_width) {
        terminal_putentryat(' ', terminal_color, x, y);
        x++;
    }
}

void terminal_putchar(char c)
{
    int viewport_width = vga_width();
    int viewport_height = vga_height();

    if(c == '\n') {
        terminal_clear_line_end(terminal_column, terminal_row);
        terminal_column = viewport_width;
    } else if(c == 0x08) {
        /* backspace */
        if(terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
    }

    if (terminal_column == viewport_width) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == viewport_height) {
            vga_scroll_row();
            terminal_row--;
        }
        terminal_clear_line_end(0, terminal_row);
    }

    vga_move_cursor(terminal_row, terminal_column);

    if(serial_console_port) {
        uart_putc_sync(serial_console_port, c);
    }
}

void terminal_writestring(const char* data)
{
    size_t datalen = strlen(data);
    for ( size_t i = 0; i < datalen; i++ ) {
        terminal_putchar(data[i]);
    }
}
