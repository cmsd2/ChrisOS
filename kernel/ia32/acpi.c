#include <arch/acpi.h>
#include <arch/acpica_osl.h>
#include <acpi.h>
#include <utils/kprintf.h>

const char * const ACPI_STATUS_OK = "ok";
const char * const ACPI_STATUS_ERROR = "error";
const char * const ACPI_STATUS_NO_MEMORY = "out of memory";

const char * acpi_strerror(ACPI_STATUS status) {
    const char * status_msg;
    switch(status) {
    case AE_OK:
        status_msg = ACPI_STATUS_OK;
        break;
    case AE_ERROR:
        status_msg = ACPI_STATUS_ERROR;
        break;
    case AE_NO_MEMORY:
        status_msg = ACPI_STATUS_NO_MEMORY;
        break;
    }
    return status_msg;
}

void acpi_perror(const char * msg, ACPI_STATUS status) {
    kprintf("acpi error: %s: %s\n", msg, acpi_strerror(status));
}

void acpi_early_init(void) {
    ACPI_STATUS result = AcpiInitializeSubsystem();

    if(result != AE_OK) {
        acpi_perror("AcpiInitializeSubsystem", result);
        return;
    } else {
        kprintf("Acpi subsystem initialized\n");
    }
    //AcpiInitializeTables();
}

// after virtual memory management enabled
void acpi_late_init() {
}

void acpi_enter_acpi_mode() {
}

void acpi_install_acpi_event_handlers() {
}

void acpi_finish_init_and_start() {
}
