#include <arch/pic.h>
#include <sys/io.h>
#include <utils/kprintf.h>
#include <utils/panic.h>

unsigned char pic_interrupt_for_irq(unsigned char irq) {
    if(irq < PIC_MAX_IRQS) {
        return irq + PIC_IRQ1_BASE;
    } else {
        return irq + PIC_IRQ2_BASE;
    }
}

void pic_init(void) {
    pic_map_irqs(PIC_IRQ1_BASE, PIC_IRQ2_BASE);

    pic_mask_all();
}

// mostly follows wiki.osdev.org/8259_PIC
void pic_map_irqs(unsigned int irq_base_1, unsigned int irq_base_2) {
    unsigned char mask_1, mask_2;

    // save irq masks
    mask_1 = inb(PIC1_DATA);
    mask_2 = inb(PIC2_DATA);

    outb(PIC1, ICW1_ICW1 | ICW1_ICW4_NEEDED);
    io_wait();
    outb(PIC2, ICW1_ICW1 | ICW1_ICW4_NEEDED);
    io_wait();
    outb(PIC1_DATA, irq_base_1);
    io_wait();
    outb(PIC2_DATA, irq_base_2);
    io_wait();
    outb(PIC1_DATA, 4); // irq2 is the slave
    io_wait();
    outb(PIC2_DATA, 2); // slave id
    io_wait();

    outb(PIC1_DATA, ICW4_8086_MODE);
    io_wait();
    outb(PIC2_DATA, ICW4_8086_MODE);
    io_wait();

    // restore masks
    outb(PIC1_DATA, mask_1);
    io_wait();
    outb(PIC2_DATA, mask_2);
    io_wait();
}

// masks all interrupts
void pic_mask_all(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void pic_eoi(unsigned char irq) {
    if(irq >= PIC_MAX_IRQS) {
        outb(PIC2, PIC_EOI);
    }
    //pics are chained, so no else
    outb(PIC1, PIC_EOI);
}

unsigned short pic_data_port(enum pic _pic) {
    switch(_pic) {
    case pic1:
        return PIC1_DATA;
    case pic2:
        return PIC2_DATA;
    default:
        panic("invalid pic enum");
        break;
    }
}

unsigned short pic_data_port_and_irq(unsigned char * irq) {
    unsigned short port;

    if(*irq < PIC_MAX_IRQS) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        *irq -= PIC_MAX_IRQS;
    }

    return port;
}

unsigned char pic_get_mask(enum pic _pic) {
    unsigned short port = pic_data_port(_pic);
    unsigned char value = inb(port);

    return value;
}

void pic_set_mask(enum pic _pic, unsigned char value) {
    unsigned short port = pic_data_port(_pic);

    outb(port, value);
    io_wait();
}

void pic_mask(unsigned char irq) {
    unsigned char pic_local_irq = irq;
    unsigned short port = pic_data_port_and_irq(&pic_local_irq);
    unsigned char value = inb(port);

    value |= (1 << pic_local_irq);
    outb(port, value);
    io_wait();
}

void pic_unmask(unsigned char irq) {
    unsigned short port = pic_data_port_and_irq(&irq);
    unsigned char value = inb(port);

    value &= ~(1 << irq);
    outb(port, value);
    io_wait();
}

