#ifndef IA32_ARCH_PS2_H
#define IA32_ARCH_PS2_H

#include <stdint.h>

#define PS2_DATA_PORT 0x60
#define PS2_SC_PORT 0x64

uint8_t ps2_read_data();
uint8_t ps2_read_status();
void ps2_write_command(uint8_t command);

#endif
