#include <arch/acpica_osl.h>
#include <mm/malloc.h>
#include <mm/kmem.h>
#include <utils/kprintf.h>

bool implemented = false;

/* 9.1 Environmental & ACPI Tables */

ACPI_STATUS AcpiOsInitialize(void) {
    return AE_OK;
}

ACPI_STATUS AcpiOsTerminate(void) {
    assert(implemented);
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void) {
    ACPI_SIZE addr;
    AcpiFindRootPointer(&addr);
    return addr;
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES * predefined_name, ACPI_STRING * new_value) {
    if(!predefined_name || !new_value) {
        return AE_BAD_PARAMETER;
    }

    *new_value = 0;
    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER * existing_table, ACPI_TABLE_HEADER ** new_table) {
    *new_table = NULL;
    return AE_OK;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER * existing_table, ACPI_PHYSICAL_ADDRESS * new_address, UINT32 * new_table_len) {
    *new_address = (ACPI_PHYSICAL_ADDRESS)NULL;
    return AE_OK;
}

/* 9.2 Memory Management */

#ifndef ACPI_USE_LOCAL_CACHE
ACPI_STATUS AcpiOsCreateCache(char * name, UINT16 obj_size, UINT16 max_depth, ACPI_CACHE_T ** cache_result) {
    assert(implemented);
}

ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T * cache) {
    assert(implemented);
}

ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T * cache) {
    assert(implemented);
}

void * AcpiOsAcquireObject(ACPI_CACHE_T * cache) {
    assert(implemented);
}

ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T * cache, void * obj) {
    assert(implemented);
}
#endif /* ACPI_USE_LOCAL_CACHE */

void * AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS phys_addr, ACPI_SIZE len) {
    vm_ptr_t vm_page_addr;
    
    // TODO too low level here
    uintptr_t phys_page_addr = align_address_down(phys_addr, KMEM_PAGE_SIZE);
    size_t phys_addr_offset = phys_addr - phys_page_addr;
    size_t phys_total_size = KMEM_PAGE_ALIGN(len + phys_addr_offset);
    assert(kmem_pages_map(phys_page_addr, phys_total_size / KMEM_PAGE_SIZE, true, &vm_page_addr));
    vm_ptr_t vm_addr = vm_page_addr + phys_addr_offset;
    kprintf("mapped phys_page_addr=0x%lx phys_addr=0x%lx size=0x%ld at vm_addr=0x%lx vm_page_addr=0x%lx\n", phys_page_addr, phys_addr, len, vm_addr, vm_page_addr);

    return (void*)vm_addr;
}

void AcpiOsUnmapMemory(void * virtual_addr, ACPI_SIZE len) {
    uintptr_t v_page_addr = align_address_down((uintptr_t)virtual_addr, KMEM_PAGE_SIZE);
    kmem_pages_unmap(v_page_addr, true);
}

ACPI_STATUS AcpiOsGetPhysicalAddress(void * virtual_address, ACPI_PHYSICAL_ADDRESS * physical_address_result) {
    assert(implemented);
}

void * AcpiOsAllocate(ACPI_SIZE len) {
    return kmalloc(len);
}

void AcpiOsFree(void * ptr) {
    kfree(ptr);
}

BOOLEAN AcpiOsReadable(void * ptr, ACPI_SIZE len) {
    assert(implemented);
}

BOOLEAN AcpiOsWritable(void * ptr, ACPI_SIZE len) {
    assert(implemented);
}

/* 9.3 Multithreading & Scheduling Services */

ACPI_THREAD_ID AcpiOsGetThreadId(void) {
    return 1; // TODO return threadid
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE type, ACPI_OSD_EXEC_CALLBACK fun, void * context) {
    assert(implemented);
}

void AcpiOsSleep(UINT64 millis) {
    assert(implemented);
}

void AcpiOsStall(UINT32 millis) {
    assert(implemented);
}

void AcpiOsWaitEventsComplete(void) {
    assert(implemented);
}

/* 9.4 Mutual Exclusion & Synchronization */

ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX * mutex_result) {
    // TODO implemented mutexes
    return AE_OK;
}

void AcpiOsDeleteMutex(ACPI_MUTEX mutex) {
    // TODO delete mutex
}

ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX mutex, UINT16 timeout_millis) {
    // TODO lock mutex
    return AE_OK;
}

void AcpiOsReleaseMutex(ACPI_MUTEX mutex) {
    // TODO release mutex
}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 max_units, UINT32 initial_units, ACPI_SEMAPHORE * semaphore_result) {
    // TODO create semaphore
    return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE semaphore) {
    // TODO delete semaphore
    return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE semaphore, UINT32 units, UINT16 timeout_millis) {
    // TODO wait semaphore
    return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE semaphore, UINT32 units) {
    // TODO signal semaphore
    return AE_OK;
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK * spinlock_result) {
    // TODO create spinlock
    return AE_OK;
}

void AcpiOsDeleteLock(ACPI_HANDLE spinlock) {
    // TODO delete spinlock
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK spinlock) {
    // TODO acquire spinlock
    // ret value used only inside AcpiOsReleaseLock?
    return 0;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK spinlock, ACPI_CPU_FLAGS flags) {
    // TODO release lock
}

/* 9.5 Interrupt Handling */

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 irq, ACPI_OSD_HANDLER handler, void * context) {
    assert(implemented);
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 irq, ACPI_OSD_HANDLER handler) {
    assert(implemented);
}

/* 9.6 Memory Access & Memory Mapped I/O */
ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS phys_addr, UINT64 * value, UINT32 width) {
    assert(implemented);
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS phys_addr, UINT64 value, UINT32 width) {
    assert(implemented);
}

/* 9.7 Port I/O */
ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32 * value, UINT32 width) {
    assert(implemented);
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 value, UINT32 width) {
    assert(implemented);
}

/* 9.8 PCI Configuration Space Access */
// pci id is definitely a pointer, contrary to pdf doc.
ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID * pci_id, UINT32 reg, UINT64 * value, UINT32 width) {
    assert(implemented);
}

ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID * pci_id, UINT32 reg, UINT64 value, UINT32 width) {
    assert(implemented);
}

/* 9.9 Formatted Output */
void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char * format, ...) {
    va_list ap;
    va_start(ap, format);
    //kprintf(format, ap);
    va_end(ap);
}

void AcpiOsVprintf(const char * format, va_list args) {
    //kprintf(format, args);
}

void AcpiOsRedirectOutput(void * destination) {
    assert(implemented);
}

/* 9.10 System ACPI Table Access */

/* 9.11 Miscellaneous */
UINT64 AcpiOsGetTimer(void) {
    assert(implemented);
}

ACPI_STATUS AcpiOsSignal(UINT32 acpi_sig, void * info) {
    assert(implemented);
}

ACPI_STATUS AcpiOsGetLine(char * buffer, UINT32 len, UINT32 * len_result) {
    assert(implemented);
}

