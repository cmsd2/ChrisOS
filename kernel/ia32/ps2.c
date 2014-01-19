#include <arch/ps2.h>
#include <sys/io.h>

uint8_t ps2_read_data() {
    return inb(PS2_DATA_PORT);
}

uint8_t ps2_read_status() {
    return inb(PS2_SC_PORT);
}

void ps2_write_command(uint8_t command) {
    outb(command, PS2_SC_PORT);
}

