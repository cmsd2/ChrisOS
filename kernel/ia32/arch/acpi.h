#ifndef X86_ACPI_H
#define X86_ACPI_H

#include <acpi.h>

void acpi_init(void);
const char * acpi_strerror(ACPI_STATUS status);
void acpi_perror(const char * msg, ACPI_STATUS status);
bool acpi_tables_init(void);
bool acpi_tables_loaded(void);
ACPI_TABLE_FADT * acpi_get_fadt(void);
uint16_t acpi_fadt_get_boot_arch_flags();
bool acpi_fadt_has_8042();

#endif
