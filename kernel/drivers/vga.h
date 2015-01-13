#ifndef DRIVER_VGA_H
#define DRIVER_VGA_H

#include <stdint.h>

void vga_init(void);

// text mode
/* Hardware text mode color constants. */
enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

#define VGA_PORT_3C0 0x3c0

#define VGA_PORT_3C2 0x3c2
#define VGA_PORT_3CC 0x3cc

#define VGA_PORT_3C4 0x3c4
#define VGA_DATA_3C5 0x3c5

#define VGA_PORT_3CE 0x3ce
#define VGA_DATA_3CF 0x3cf

#define VGA_PORT_3D4 0x3d4
#define VGA_DATA_3D5 0x3d5

#define VGA_PORT_3C6 0x3c6
#define VGA_DATA_3C6 0x3c6

#define VGA_PORT_3C7 0x3c7
#define VGA_PORT_3C8 0x3c8
#define VGA_DATA_3C9 0x3c9

void vga_scroll_row(void);
void vga_move_cursor(int row, int col);
uint8_t vga_make_color(enum vga_color fg, enum vga_color bg);
uint16_t vga_make_vgaentry(char c, uint8_t color);
uint8_t vga_read_misc_output_reg(void);
void vga_write_misc_output_reg(uint8_t value);
void vga_put_char(char c, uint8_t colour, int row, int col);
int vga_width(void);
int vga_height(void);

#endif
