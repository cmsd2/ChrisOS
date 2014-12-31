#include <arch/ps2.h>
#include <sys/io.h>
#include <arch/acpi.h>
#include <utils/kprintf.h>
#include <utlist.h>
#include <sys/scheduler.h>
#include <arch/apic.h>
#include <arch/interrupts.h>
#include <sys/hal.h>

bool _ps2_available = false;

static struct ps2_async_command * _ps2_free_commands;
static struct ps2_async_command * _ps2_kbd_command_queue;
static uint32_t _kbd_irq = 1;
static struct thread * _ps2_kbd_thread;

//TODO this is horrible
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
    bool is_dual_channel = PS2_CTRL_CFG_GET_2ND_PS2_CLOCK_DISABLE(config) != 0 ?  true : false;
    kprintf("ps2 config: 0x%hhx dual_channel=%s\n", config, BOOL_TO_STR(is_dual_channel));
    config = PS2_CTRL_CFG_SET_1ST_PS2_INT_ENABLE(config, 0);
    config = PS2_CTRL_CFG_SET_2ND_PS2_INT_ENABLE(config, 0);
    config = PS2_CTRL_CFG_SET_1ST_PS2_TRANSLATION_ENABLE(config, 0);
    ps2_write_config(config);

    ps2_write_command(test_first_ps2_port);
    ps2_wait_output_buffer_sync();

    // should be 0
    uint8_t first_port_response;
    uint8_t second_port_response;

    first_port_response = ps2_read_data();
    kprintf("ps2 port 0 test result: 0x%hhx\n", first_port_response);

    if(is_dual_channel) {
        ps2_write_command(test_second_ps2_port);
        ps2_wait_output_buffer_sync();
        second_port_response = ps2_read_data();
        kprintf("ps2 port 1 test result: 0x%hhx\n", second_port_response);
    }

    if(first_port_response == 0) {
        ps2_write_command(enable_first_ps2_port);

        config = ps2_read_config();
        config = PS2_CTRL_CFG_SET_1ST_PS2_INT_ENABLE(config, 1);
        ps2_write_config(config);
    }

    if(is_dual_channel && second_port_response == 0) {
        ps2_write_command(enable_second_ps2_port);

        config = ps2_read_config();
        config = PS2_CTRL_CFG_SET_2ND_PS2_INT_ENABLE(config, 1);
        ps2_write_config(config);
    }

    kprintf("installing ps2 keyboard irq handler\n");
    hal_install_irq_handler(_kbd_irq, ps2_kbd_interrupt_handler, ps2_kbd_thread, "ps2_kbd", NULL);
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
    ps2_wait_output_buffer_sync();
    uint8_t response = ps2_read_data();
    if(!response == 0x55) {
        kprintf("ps2 self test failed. response: 0x%hhx\n", response);
    }

    return true;
}

void ps2_wait_output_buffer_sync() {
    kprintf("spinwaiting on ps2 output buffer...\n");
    while(!ps2_can_read_data()) {
        // nothing
    }
}

void ps2_wait_input_buffer_sync() {
    uint8_t status;
    status = ps2_read_status();
    kprintf("spinwaiting on ps2 input buffer...\n");
    while(!PS2_STATUS_GET_INPUT_BUFFER_STATUS(status)) {
        status = ps2_read_status();
    }
}

bool ps2_can_read_data() {
    uint8_t status = ps2_read_status();
    return 1 == PS2_STATUS_GET_OUTPUT_BUFFER_STATUS(status);
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
    while(ps2_can_read_data()) {
        ps2_read_data();
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

struct ps2_async_command * ps2_command_alloc() {
    struct ps2_async_command * cmd;
    if(_ps2_free_commands) {
        cmd = _ps2_free_commands;
        LL_DELETE(_ps2_free_commands, cmd);
    } else {
        cmd = (struct ps2_async_command *)malloc(sizeof(struct ps2_async_command));
    }
    return cmd;
}

void ps2_command_free(struct ps2_async_command * cmd) {
    LL_PREPEND(_ps2_free_commands, cmd);
}

struct ps2_async_command * ps2_send_command(struct ps2_async_command * queue,
                                            uint8_t cmd,
                                            uint8_t data,
                                            ps2_async_callback callback) {
    struct ps2_async_command * result = ps2_command_alloc();
    if(result) {
        result->cmd = cmd;
        result->data = data;
        result->callback = callback;
        DL_APPEND(queue, result);
    }
    return result;
}

bool ps2_cancel_command(struct ps2_async_command * queue, struct ps2_async_command * cmd) {
    DL_DELETE(queue, cmd);
    cmd->next = 0;
    ps2_command_free(cmd);
    return true;
}

enum hal_fast_irq_handler_result
ps2_kbd_interrupt_handler(uint32_t int_no,
                          struct registers * regs,
                          void * __unused data) {

    kprintf("ps2_kbd received interrupt\n");
    return IRQ_DEFER;
}

int ps2_kbd_thread(void * data __unused) {
    _ps2_kbd_thread = current_thread();
    size_t scancode_i = 0;
    uint8_t scancode[MAX_SCANCODE_BYTES];
    bool done;

    ps2_flush_output_buffer();

    enum ps2_driver_state state = waiting_scancode_start;
    while(1) {
        hal_mask_irq(_kbd_irq);

        done = true;
        do {
            switch(state) {
            case waiting_command:
                if(_ps2_kbd_command_queue) {
                    ps2_kbd_issue_next_command();
                    state = waiting_response;
                }
                break;
            case waiting_response:
                if(ps2_kbd_handle_response()) {
                    state = waiting_command;
                    continue;
                }
                break;
            case waiting_scancode_start:
                scancode[0] = ps2_read_data();
                kprintf("received keyboard data 0x%hhx\n", scancode[0]);
                scancode_i = 1;
                if(ps2_kbd_handle_scancode(scancode, scancode_i)) {
                    ps2_kbd_print_scancode(scancode, scancode_i);
                } else {
                    state = waiting_scancode_end;
                }
                done = !ps2_can_read_data();
                break;
            case waiting_scancode_end:
                scancode[scancode_i] = ps2_read_data();
                kprintf("received keyboard data 0x%hhx\n", scancode[scancode_i]);
                scancode_i++;
                if(ps2_kbd_handle_scancode(scancode, scancode_i)) {
                    ps2_kbd_print_scancode(scancode, scancode_i);
                    state = waiting_scancode_start;
                }
                done = !ps2_can_read_data();
                break;
            }
        } while(!done);

        scheduler_make_blocked(current_thread());
        kprintf("unmasking kbd irq\n");
        hal_unmask_irq(_kbd_irq);
        // if kbd irq arrives here, we're ok,
        // we just immediately get made runnable and rescheduled
        scheduler_yield();
    }
}

void ps2_kbd_print_scancode(uint8_t * scancode, size_t length) {
    kprintf("scancode: ");
    for(int i = 0; i < length; i++)
        kprintf(" 0x%hhx", scancode[i]);
    kprintf("\n");
}

bool ps2_kbd_handle_scancode(uint8_t * scancode, size_t length) {
    bool result = true;
    switch(length) {
    case 1:
        if(scancode[0] == 0xE0 || scancode[0] == 0xF0 || scancode[0] == 0xE1) {
            result = false;
        }
        break;
    case 2:
        if(scancode[0] == 0xE0 && scancode[1] == 0xF0 || scancode[0] == 0xE1) {
            result = false;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        if(scancode[0] == 0xE1) {
            result = false;
        }
    }
    return result;
}

void ps2_kbd_issue_next_command() {
    struct ps2_async_command * cmd = _ps2_kbd_command_queue;
    ps2_write_command(cmd->cmd);
    if(cmd->send_data) {
        ps2_write_data(cmd->data);
    }
}

bool ps2_kbd_handle_response() {
    uint8_t data = ps2_read_data();
    if(data == 0xFE) {
        return false;
    } else {
        struct ps2_async_command * cmd = _ps2_kbd_command_queue;
        assert(cmd != NULL);
        assert(cmd->callback != NULL);
        cmd->callback(cmd->cmd, data);
        ps2_cancel_command(_ps2_kbd_command_queue, cmd);
        return true;
    }
}
