#ifndef X86_ACPICA_OSL_H
#define X86_ACPICA_OSL_H

#include <acpi.h>

/* 9.1 Environmental & ACPI Tables */
ACPI_STATUS AcpiOsInitialize(void);
ACPI_STATUS AcpiOsTerminate(void);
ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer(void);
ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES * predefined_name, ACPI_STRING * new_value);
ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER * existing_table, ACPI_TABLE_HEADER ** new_table);
ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER * existing_table, ACPI_PHYSICAL_ADDRESS * new_address, UINT32 * new_table_len);

/* 9.2 Memory Management */
#ifndef ACPI_USE_LOCAL_CACHE
ACPI_STATUS AcpiOsCreateCache(char * name, UINT16 obj_size, UINT16 max_depth, ACPI_CACHE_T ** cache_result);
ACPI_STATUS AcpiOsDeleteCache(ACPI_CACHE_T * cache);
ACPI_STATUS AcpiOsPurgeCache(ACPI_CACHE_T * cache);
void * AcpiOsAcquireObject(ACPI_CACHE_T * cache);
ACPI_STATUS AcpiOsReleaseObject(ACPI_CACHE_T * cache, void * obj);
#endif /* ACPI_USE_LOCAL_CACHE */
void * AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS phys_addr, ACPI_SIZE len);
void AcpiOsUnmapMemory(void * virtual_addr, ACPI_SIZE len);
ACPI_STATUS AcpiOsGetPhysicalAddress(void * virtual_address, ACPI_PHYSICAL_ADDRESS * physical_address_result);
void * AcpiOsAllocate(ACPI_SIZE len);
void AcpiOsFree(void * ptr);
BOOLEAN AcpiOsReadable(void * ptr, ACPI_SIZE len);
BOOLEAN AcpiOsWritable(void * ptr, ACPI_SIZE len);

/* 9.3 Multithreading & Scheduling Services */
ACPI_THREAD_ID AcpiOsGetThreadId(void);
ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE type, ACPI_OSD_EXEC_CALLBACK fun, void * context);
void AcpiOsSleep(UINT64 millis);
void AcpiOsStall(UINT32 millis);
void AcpiOsWaitEventsComplete(void);

/* 9.4 Mutual Exclusion & Synchronization */
ACPI_STATUS AcpiOsCreateMutex(ACPI_MUTEX * mutex_result);
void AcpiOsDeleteMutex(ACPI_MUTEX mutex);
ACPI_STATUS AcpiOsAcquireMutex(ACPI_MUTEX mutex, UINT16 timeout_millis);
void AcpiOsReleaseMutex(ACPI_MUTEX mutex);
ACPI_STATUS AcpiOsCreateSemaphore(UINT32 max_units, UINT32 initial_units, ACPI_SEMAPHORE * semaphore_result);
ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE semaphore);
ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE semaphore, UINT32 units, UINT16 timeout_millis);
ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE semaphore, UINT32 units);
ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK * spinlock_result);
void AcpiOsDeleteLock(ACPI_HANDLE spinlock);
ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK spinlock);
void AcpiOsReleaseLock(ACPI_SPINLOCK spinlock, ACPI_CPU_FLAGS flags);

/* 9.5 Interrupt Handling */
//typedef UINT32 (*ACPI_OSD_HANDLER) (void * context);
ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 irq, ACPI_OSD_HANDLER handler, void * context);
ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 irq, ACPI_OSD_HANDLER handler);

/* 9.6 Memory Access & Memory Mapped I/O */
ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS phys_addr, UINT64 * value, UINT32 width);
ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS phys_addr, UINT64 value, UINT32 width);

/* 9.7 Port I/O */
ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32 * value, UINT32 width);
ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 value, UINT32 width);

/* 9.8 PCI Configuration Space Access */
ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID * pci_id, UINT32 reg, UINT64 * value, UINT32 width);
ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID * pci_id, UINT32 reg, UINT64 value, UINT32 width);

/* 9.9 Formatted Output */
void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char * format, ...);
void AcpiOsVprintf(const char * format, va_list args);
void AcpiOsRedirectOutput(void * destination);

/* 9.10 System ACPI Table Access */

/* 9.11 Miscellaneous */
UINT64 AcpiOsGetTimer(void);
ACPI_STATUS AcpiOsSignal(UINT32 acpi_sig, void * info);
ACPI_STATUS AcpiOsGetLine(char * buffer, UINT32 len, UINT32 * len_result);

#endif
