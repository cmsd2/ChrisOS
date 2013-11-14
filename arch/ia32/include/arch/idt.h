/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#ifndef IA32_ARCH_IDT_H
#define IA32_ARCH_IDT_H

#include <stdint.h>

struct idt_header {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t _unused;
	uint8_t flags;
	uint16_t offset_high;
} __attribute__((packed));

void idt_init(void);
void idt_set_gate(uint8_t, uint32_t, uint16_t, uint8_t);
void idt_zero_entry(struct idt_entry *entry);
void idt_flush(void);
void idt_flush_table(uint32_t __idt_header);

#define IDT_ENTRIES 256

#endif

