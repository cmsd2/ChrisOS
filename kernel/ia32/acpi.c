#include <arch/acpi.h>
#include <arch/acpica_osl.h>
#include <acpi.h>
#include <utils/kprintf.h>

const char * const ACPI_STATUS_OK = "ok";
const char * const ACPI_STATUS_ERROR = "error";
const char * const ACPI_STATUS_NO_MEMORY = "out of memory";

#define ACPI_MAX_INIT_TABLES 16
static ACPI_TABLE_DESC _acpi_tables[ACPI_MAX_INIT_TABLES];

bool _acpi_tables_loaded = false;

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

bool acpi_tables_init(void) {
    ACPI_STATUS result = AcpiInitializeTables(_acpi_tables, ACPI_MAX_INIT_TABLES, TRUE);
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiInitializeTables", result);
        return false;
    }

    _acpi_tables_loaded = true;
    return true;
}

void acpi_init(void) {
    ACPI_STATUS result;
   
    result = AcpiInitializeSubsystem();
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiInitializeSubsystem", result);
        return;
    } else {
        kprintf("Acpi subsystem initialised\n");
    }

    result = AcpiReallocateRootTable();
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiReallocateRootTable", result);
    } else {
        kprintf("Acpi root tables copied\n");
    }

    result = AcpiLoadTables();
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiLoadTables", result);
    } else {
        kprintf("Acpi tables loaded\n");
    }

    _acpi_tables_loaded = true;

    /* do stuff */

#ifdef ENABLE_ACPI_MODE
    result = AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiEnableSubsystem", result);
    } else {
        kprintf("Acpi hardware initialized\n");
    }
    
    result = AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);
    if(ACPI_FAILURE(result)) {
        acpi_perror("AcpiInitializeObjects", result);
    } else {
        kprintf("Acpi namespace objects initialized\n");
    }
#endif
}

bool acpi_tables_loaded() {
    return _acpi_tables_loaded;
}

void acpi_install_acpi_event_handlers() {
}

ACPI_TABLE_FADT * acpi_get_fadt() {
    ACPI_TABLE_HEADER * table;
    ACPI_STATUS status = AcpiGetTable(ACPI_SIG_FADT, 1, &table);
    if(ACPI_FAILURE(status)) {
        return 0;
    } else if(strncmp(table->Signature, ACPI_SIG_FADT, ACPI_NAME_SIZE) == 0) {
        return (ACPI_TABLE_FADT*)table;
    }
}

uint16_t acpi_fadt_get_boot_arch_flags() {
    ACPI_TABLE_FADT * table = acpi_get_fadt();
    assert(table);
    kprintf("acpi fadt boot_flags=0x%hx\n", table->BootFlags);
    return table->BootFlags;
}

bool acpi_fadt_has_8042() {
    return (acpi_fadt_get_boot_arch_flags() & ACPI_FADT_8042) != 0;
}
