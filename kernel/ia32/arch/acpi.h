#ifndef X86_ACPI_H
#define X86_ACPI_H

#include <acpi.h>

void acpi_init(void);
const char * acpi_strerror(ACPI_STATUS status);
void acpi_perror(const char * msg, ACPI_STATUS status);
void acpi_early_init(void);

#endif
