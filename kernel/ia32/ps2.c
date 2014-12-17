#include <arch/ps2.h>
#include <sys/io.h>
#include <arch/acpi.h>
#include <utils/kprintf.h>

bool _ps2_available = false;

void ps2_init() {
    if(acpi_tables_loaded()) {
        kprintf("checking acpi for ps2 controller\n");
        _ps2_available = acpi_fadt_has_8042();
    } else {
        kprintf("no acpi. assuming ps2 controller\n");
        _ps2_available = true;
    }
    
    if(ps2_probe()) {
        kprintf("ps2 controller responding\n");
        _ps2_available = true;
    }

    if(!_ps2_available) {
        kprintf("ps2 controller absent\n");
        return;
    }
    
    kprintf("ps2 controller present\n");

    ps2_write_command(disable_first_ps2_port);
    ps2_write_command(disable_second_ps2_port);

    ps2_flush_output_buffer();

    ps2_ctrl_cfg_t config = ps2_read_config();
    kprintf("ps2 config: 0x%hhx dual_channel=%s\n", config, BOOL_TO_STR(PS2_CTRL_CFG_GET_2ND_PS2_CLOCK_DISABLE(config) == 0));
    config = PS2_CTRL_CFG_SET_1ST_PS2_INT_ENABLE(config, 0);
    config = PS2_CTRL_CFG_SET_2ND_PS2_INT_ENABLE(config, 0);
    config = PS2_CTRL_CFG_SET_1ST_PS2_TRANSLATION_ENABLE(config, 0);
    ps2_write_config(config);
}

bool ps2_probe() {
    uint8_t status;
    int buffer_size = 16;
    int bytes_read;
    status = ps2_read_status();

    if(!PS2_STATUS_GET_SYSTEM_FLAG(status)) {
        kprintf("ps2 POST flag is false: 0x%hhx\n", status);
        return false;
    }

    for(bytes_read = 0; bytes_read < buffer_size && PS2_STATUS_GET_OUTPUT_BUFFER_STATUS(status); bytes_read++) {
        ps2_read_data();
        status = ps2_read_status();
    }

    if(bytes_read == buffer_size) {
        kprintf("read %d bytes from ps2 controller and couldn't empty output buffer. status=0x%hhx\n", bytes_read, status);
        return false;
    }
    
    ps2_write_command(test_ps2_controller);
    ps2_wait_data_sync();
    uint8_t response = ps2_read_data();
    if(!response == 0x55) {
        kprintf("ps2 self test failed. response: 0x%hhx\n", response);
    }

    return true;
}

void ps2_wait_data_sync() {
    uint8_t status;
    status = ps2_read_status();
    kprintf("spinwaiting on ps2 output buffer...\n");
    while(!PS2_STATUS_GET_OUTPUT_BUFFER_STATUS(status)) {
        status = ps2_read_status();
    }
}

uint8_t ps2_read_data() {
    return inb(PS2_DATA_PORT);
}

void ps2_write_data(uint8_t data) {
    outb(PS2_DATA_PORT, data);
}

uint8_t ps2_read_status() {
    return inb(PS2_SC_PORT);
}

void ps2_write_command(uint8_t command) {
    outb(PS2_SC_PORT, command);
}

void ps2_flush_output_buffer(void) {
    ps2_status_reg_t status;
    status = ps2_read_status();
    while(PS2_STATUS_GET_OUTPUT_BUFFER_STATUS(status)) {
        ps2_read_data();
        status = ps2_read_status();
    }
}

uint8_t ps2_read_config(void) {
    ps2_write_command(read_byte_0);
    return ps2_read_data();
}

void ps2_write_config(uint8_t config) {
    ps2_write_command(write_byte_0);
    ps2_write_data(config);
}

