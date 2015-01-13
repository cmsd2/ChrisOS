#include <drivers/vga.h>
#include <boot/layout.h>
#include <sys/io.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint16_t* vga_buffer;

void vga_init(void) {
    uint8_t misc_out = vga_read_misc_output_reg();
    misc_out |= 1;
    vga_write_misc_output_reg(misc_out);
    vga_buffer = (uint16_t*) (0xB8000 + &KERNEL_VMA);
    vga_move_cursor(0, 0);
}

void vga_scroll_row() {
    size_t index;
    uint16_t entry;
    for(int y = 1; y < VGA_HEIGHT; y++) {
        for(int x = 0; x < VGA_WIDTH; x++) {
            index = y * VGA_WIDTH + x;
            entry = vga_buffer[index];
            index -= VGA_WIDTH;
            vga_buffer[index] = entry;
        }
    }
}

void vga_move_cursor(int row, int col) {
    int position = (row * 80) + col;

    outb(VGA_PORT_3D4, 0x0F);
    outb(VGA_DATA_3D5, (unsigned char)(position&0xFF));

    outb(VGA_PORT_3D4, 0x0E);
    outb(VGA_DATA_3D5, (unsigned char )((position>>8)&0xFF));
}

uint8_t vga_make_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

uint16_t vga_make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

uint8_t vga_read_misc_output_reg(void) {
    return (uint8_t)inb(VGA_PORT_3CC);
}

void vga_write_misc_output_reg(uint8_t value) {
    outb(VGA_PORT_3C2, value);
}

void vga_put_char(char c, uint8_t colour, int row, int col) {
    const size_t index = col * VGA_WIDTH + row;
    vga_buffer[index] = vga_make_vgaentry(c, colour);
}

int vga_width() {
    return VGA_WIDTH;
}

int vga_height() {
    return VGA_HEIGHT;
}
