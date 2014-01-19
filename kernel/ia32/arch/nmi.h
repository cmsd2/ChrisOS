#ifndef IA32_ARCH_NMI_H
#define IA32_ARCH_NMI_H

#include <arch/io_ports.h>

#define NMI_SC_WRITABLE_BITS 0xf

unsigned char nmi_read_status();
void nmi_reenable_timer_2();
void nmi_enable_timer_2();
void nmi_enable_timer_0();
void nmi_timer_2_spinwait();

#endif
