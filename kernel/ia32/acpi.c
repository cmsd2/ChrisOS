#include <arch/acpi.h>
#include <arch/acpica_osl.h>
#include <arch/apic.h>
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

ACPI_TABLE_MADT * acpi_get_madt() {
    ACPI_TABLE_HEADER * table;
    ACPI_STATUS status = AcpiGetTable(ACPI_SIG_MADT, 1, &table);
    if(ACPI_FAILURE(status)) {
        return 0;
    } else if(strncmp(table->Signature, ACPI_SIG_MADT, ACPI_NAME_SIZE) == 0) {
        return (ACPI_TABLE_MADT*)table;
    } else {
        kprintf("spurious acpi table name while looking for MADT: %s\n", table->Signature);
        return 0;
    }
}

uint32_t acpi_get_madt_apic_address() {
    ACPI_TABLE_MADT * table = acpi_get_madt();
    assert(table != NULL);
    return table->Address;
}

void acpi_madt_configure_apic_subtable(ACPI_SUBTABLE_HEADER * header) {
    ACPI_MADT_LOCAL_APIC * lapic;
    ACPI_MADT_IO_APIC * ioapic;
    ACPI_MADT_INTERRUPT_OVERRIDE * int_override;
    ACPI_MADT_NMI_SOURCE * nmi_source;
    ACPI_MADT_LOCAL_APIC_NMI * lapic_nmi;
    ACPI_MADT_LOCAL_APIC_OVERRIDE * lapic_override;
    switch(header->Type) {
    case ACPI_MADT_TYPE_LOCAL_APIC:
        lapic = (ACPI_MADT_LOCAL_APIC*)header;
        apic_configure_lapic(lapic->Id, lapic->ProcessorId, lapic->LapicFlags);
        break;
    case ACPI_MADT_TYPE_IO_APIC:
        ioapic = (ACPI_MADT_IO_APIC*)header;
        apic_configure_ioapic(ioapic->Id, ioapic->Address, ioapic->GlobalIrqBase);
        break;
    case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
        int_override = (ACPI_MADT_INTERRUPT_OVERRIDE*)header;
        apic_configure_int_override(int_override->Bus, int_override->SourceIrq,
                                    int_override->GlobalIrq, int_override->IntiFlags);
        break;
    case ACPI_MADT_TYPE_NMI_SOURCE:
        nmi_source = (ACPI_MADT_NMI_SOURCE*)header;
        apic_configure_nmi_source(nmi_source->GlobalIrq, nmi_source->IntiFlags);
        break;
    case ACPI_MADT_TYPE_LOCAL_APIC_NMI:
        lapic_nmi = (ACPI_MADT_LOCAL_APIC_NMI*)header;
        apic_configure_lapic_nmi(lapic_nmi->ProcessorId, lapic_nmi->Lint,
                                 lapic_nmi->IntiFlags);
        break;
    case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
        lapic_override = (ACPI_MADT_LOCAL_APIC_OVERRIDE*)header;
        apic_configure_lapic_override(lapic_override->Address);
        break;
    }
}

void acpi_configure_apic() {
    acpi_madt_foreach_subtable(acpi_madt_configure_apic_subtable);
}

void acpi_print_subtable_header(ACPI_SUBTABLE_HEADER * header) {
    kprintf("ACPI subtable Type=%hhd Length=%hhd\n", header->Type, header->Length);
}

void acpi_madt_print_subtable(ACPI_SUBTABLE_HEADER * header) {
    ACPI_MADT_LOCAL_APIC * lapic;
    ACPI_MADT_IO_APIC * ioapic;
    ACPI_MADT_INTERRUPT_OVERRIDE * int_override;
    ACPI_MADT_NMI_SOURCE * nmi_source;
    ACPI_MADT_LOCAL_APIC_NMI * lapic_nmi;
    ACPI_MADT_LOCAL_APIC_OVERRIDE * lapic_override;
    switch(header->Type) {
    case ACPI_MADT_TYPE_LOCAL_APIC:
        lapic = (ACPI_MADT_LOCAL_APIC*)header;
        kprintf("LOCAL APIC Id=%hhd ProcessorId=%hhd LApicFlags=0x%x\n",
                lapic->Id, lapic->ProcessorId, lapic->LapicFlags);
        break;
    case ACPI_MADT_TYPE_IO_APIC:
        ioapic = (ACPI_MADT_IO_APIC*)header;
        kprintf("I/O APIC Id=%hhd Address=0x%x GlobalIrqBase=0x%x\n",
                ioapic->Id, ioapic->Address, ioapic->GlobalIrqBase);
        break;
    case ACPI_MADT_TYPE_INTERRUPT_OVERRIDE:
        int_override = (ACPI_MADT_INTERRUPT_OVERRIDE*)header;
        kprintf("Interrupt Override Bus=%hhd SourceIrq=0x%hhx GlobalIrq=0x%x IntiFlags=0x%hx\n",
                int_override->Bus, int_override->SourceIrq,
                int_override->GlobalIrq, int_override->IntiFlags);
        break;
    case ACPI_MADT_TYPE_NMI_SOURCE:
        nmi_source = (ACPI_MADT_NMI_SOURCE*)header;
        kprintf("NMI Source IntiFlags=0x%hx GlobalIrq=0x%x\n",
                nmi_source->IntiFlags, nmi_source->GlobalIrq);
        break;
    case ACPI_MADT_TYPE_LOCAL_APIC_NMI:
        lapic_nmi = (ACPI_MADT_LOCAL_APIC_NMI*)header;
        kprintf("LApic NMI ProcessorId=%hhd IntiFlags=0x%hx Lint=%hhd\n",
                lapic_nmi->ProcessorId, lapic_nmi->IntiFlags, lapic_nmi->Lint);
        break;
    case ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE:
        lapic_override = (ACPI_MADT_LOCAL_APIC_OVERRIDE*)header;
        kprintf("LApic Override Address=0x%lx\n", lapic_override->Address);
        break;
    default:
        kprintf("Subtable type was unhandled\n");
    }
}

void acpi_madt_print_subtables() {
    kprintf("ACPI MADT: Length=%d\n", acpi_get_madt()->Header.Length);
    acpi_madt_foreach_subtable(acpi_madt_print_subtable);
}

void acpi_madt_foreach_subtable(acpi_subtable_handler handler) {
    ACPI_TABLE_MADT * madt = acpi_get_madt();
    assert(madt != NULL);

    acpi_foreach_subtable(&madt->Header, (ACPI_SUBTABLE_HEADER*)(madt + 1), handler);
}

void acpi_foreach_subtable(ACPI_TABLE_HEADER * table,
                           ACPI_SUBTABLE_HEADER * first,
                           acpi_subtable_handler handler) {
    ACPI_SUBTABLE_HEADER * end = (ACPI_SUBTABLE_HEADER*)((uintptr_t)table + table->Length);
    ACPI_SUBTABLE_HEADER * subtable_header = first;
    while(subtable_header < end) {
        if(subtable_header->Length < sizeof(ACPI_SUBTABLE_HEADER))
            break;

        handler(subtable_header);

        subtable_header = (ACPI_SUBTABLE_HEADER*)((uintptr_t)subtable_header + subtable_header->Length);
    }
}

ACPI_TABLE_FADT * acpi_get_fadt() {
    ACPI_TABLE_HEADER * table;
    ACPI_STATUS status = AcpiGetTable(ACPI_SIG_FADT, 1, &table);
    if(ACPI_FAILURE(status)) {
        return 0;
    } else if(strncmp(table->Signature, ACPI_SIG_FADT, ACPI_NAME_SIZE) == 0) {
        return (ACPI_TABLE_FADT*)table;
    } else {
        kprintf("spurious acpi table name while looking for FADT: %s\n", table->Signature);
        return 0;
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
