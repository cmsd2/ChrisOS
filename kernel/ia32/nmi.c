#include <arch/nmi.h>
#include <sys/io.h>
#include <stdint.h>
#include <utils/kprintf.h>

unsigned char nmi_read_status() {
    return inb(NMI_STATUS_AND_CONTROL);
}

void nmi_timer_2_spinwait() {
    int i = 0;
    while(!(inb(NMI_STATUS_AND_CONTROL) & NMI_SC_TMR2_OUT_STS)) {
        i++;
    }
    kprintf("after spinwait: nmi tmr2 status = %hhx\n", inb(NMI_STATUS_AND_CONTROL));
    kprintf("%d loops\n", i);
}

void nmi_reenable_timer_2() {
    uint8_t tmp = inb(NMI_STATUS_AND_CONTROL);
    kprintf("reenable: read %hhx from nmi sc\n", tmp);
    tmp &= NMI_SC_MASK(NMI_SC_TMR2_ENABLE) & NMI_SC_WRITABLE_BITS;

    // pulse the timer bit low then high again to reset
    kprintf("reenable: writing %hhx to nmi sc\n", tmp);
    outb(NMI_STATUS_AND_CONTROL, tmp);

    tmp |= NMI_SC_TMR2_ENABLE;
    kprintf("reenable: writing %hhx to nmi sc\n", tmp);
    outb(NMI_STATUS_AND_CONTROL, tmp);
}

void nmi_enable_timer_2() {
    unsigned char nmi_sc = nmi_read_status();

    nmi_sc &= NMI_SC_MASK(NMI_SC_SPEAKER_ENABLE);
    nmi_sc &= NMI_SC_WRITABLE_BITS;
    kprintf("enable: writing %hhx to nmi sc\n", nmi_sc);
    outb(NMI_STATUS_AND_CONTROL, nmi_sc);
    kprintf("after disable: nmi tmr2 status = %hhx\n", inb(NMI_STATUS_AND_CONTROL));
    nmi_sc |= NMI_SC_TMR2_ENABLE;
    kprintf("enable: writing %hhx to nmi sc\n", nmi_sc);
    outb(NMI_STATUS_AND_CONTROL, nmi_sc);
    kprintf("after enable: nmi tmr2 status = %hhx\n", inb(NMI_STATUS_AND_CONTROL));
}
