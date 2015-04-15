#ifndef IA32_ARCH_PIC_H
#define IA32_ARCH_PIC_H

enum pic {
    pic1,
    pic2
};

#define PIC1 0x20
#define PIC2 0xa0
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20
#define PIC_MAX_IRQS 8

#define PIC_IRQ1_BASE 0x20
#define PIC_IRQ2_BASE 0x28

//see 8259A datasheet
#define ICW1_ICW4_NEEDED 0x1
#define ICW1_SINGLE_MODE 0x2
#define ICW1_ADDRESS_INTERVAL 0x4
#define ICW1_LEVEL_TRIGGER_MODE 0x8
#define ICW1_ICW1 0x10
#define ICW1_VECTOR_ADDRESS_MASK 0xe

#define ICW4_8086_MODE 0x1
#define ICW4_AUTO_EOI 0x2
#define ICW4_BUFFERED_MASTER_SLAVE 0x4
#define ICW4_BUFFERED_MODE 0x8
#define ICW4_BUFFERED_SLAVE 0xc
#define ICW4_SPECIAL_FULLY_NESTED_MODE 0x10

unsigned char pic_interrupt_for_irq(unsigned char irq);
unsigned char pic_irq_for_interrupt(unsigned char int_no);
void pic_init(void);
void pic_mask_all();
void pic_unmask_all();
void pic_map_irqs(unsigned int irq_base_1, unsigned int irq_base_2);
void pic_eoi(unsigned char irq);
void pic_mask(unsigned char irq);
void pic_unmask(unsigned char irq);
unsigned char pic_get_mask(enum pic _pic);
void pic_set_mask(enum pic _pic, unsigned char value);
unsigned short pic_data_port(enum pic _pic);
unsigned short pic_data_port_and_irq(unsigned char * irq);

#endif
