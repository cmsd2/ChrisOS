/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/idt.h>
#include <kernel/mem.h>
#include <kernel/panic.h>
#include <arch/layout.h>

struct idt_header _idt_header;
struct idt_entry _idt_entries[IDT_ENTRIES];

void idt_init(void) {
	_idt_header.base = (uint32_t) &_idt_entries;
	_idt_header.limit = sizeof(struct idt_entry) * IDT_ENTRIES;

	kmemset(&_idt_entries, 0, sizeof(struct idt_entry) * IDT_ENTRIES);
}

void idt_flush(void) {
	idt_flush_table((uint32_t)&_idt_header);
}

void idt_flush_table(uint32_t __idt_header) {
	__asm__("lidt (%0)"
		:
		: "r" (__idt_header)
	);
}

void idt_set_gate(uint8_t i, uint32_t offset, uint16_t selector, uint8_t flags) {
	_idt_entries[i].offset_low = offset & 0xFFFF;
	_idt_entries[i].offset_high = offset >> 16;
	_idt_entries[i].selector = selector;
	_idt_entries[i]._unused = 0;
	_idt_entries[i].flags = flags; //TODO: privilege level?
}

void idt_zero_entry(struct idt_entry *entry) {
	entry->offset_low = 0;
	entry->selector = 0;
	entry->_unused = 0;
	entry->flags = 0;
	entry->offset_high = 0;
};

