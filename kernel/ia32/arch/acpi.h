#ifndef X86_ACPI_H
#define X86_ACPI_H

#include <acpi.h>

typedef void (*acpi_subtable_handler)(ACPI_SUBTABLE_HEADER * subtable_header);

void acpi_init(void);
const char * acpi_strerror(ACPI_STATUS status);
void acpi_perror(const char * msg, ACPI_STATUS status);
bool acpi_tables_init(void);
bool acpi_tables_loaded(void);
ACPI_TABLE_MADT * acpi_get_madt(void);
uint32_t acpi_get_madt_apic_address(void);
ACPI_TABLE_FADT * acpi_get_fadt(void);
uint16_t acpi_fadt_get_boot_arch_flags();
bool acpi_fadt_has_8042();
void acpi_foreach_subtable(ACPI_TABLE_HEADER * table,
                           ACPI_SUBTABLE_HEADER * first,
                           acpi_subtable_handler handler);
void acpi_madt_foreach_subtable(acpi_subtable_handler handler);
void acpi_print_subtable_header(ACPI_SUBTABLE_HEADER * header);
void acpi_madt_print_subtables(void);
void acpi_madt_print_subtable(ACPI_SUBTABLE_HEADER * header);
void acpi_configure_apic(void);
void acpi_madt_configure_apic_subtable(ACPI_SUBTABLE_HEADER * header);
#endif
