/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/gdt.h>
 
struct gdt_entry gdt[3];
struct gdt_ptr gp;
 
// Very simple: fills a GDT entry using the parameters
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;
 
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);
 
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}
 
// Sets our 3 gates and installs the real GDT through the assembler function
void gdt_install()
{
	gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
	gp.base = (unsigned int)&gdt;
 
	gdt_set_gate(0, 0, 0, 0, 0);
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
 
	gdt_flush();
}
