#ifndef KERNEL_TERMINAL_H
#define KERNEL_TERMINAL_H

#include <stdint.h>
#include <stddef.h>

#include <drivers/vga.h>

void terminal_initialize(void);
void terminal_enable_serial_console(uint16_t port);
void terminal_disable_serial_console();
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_clear();

#endif
