/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#include <arch/gdt.h>
#include <sys/param.h>
#include <boot/layout.h>
#include <arch/tss.h>

#define GDT_ENTRY_COUNT 6
struct gdt_entry gdt[GDT_ENTRY_COUNT];
struct gdt_ptr gp;
struct tss_entry tss;

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
    unsigned char gran = gdt_32bit_granularity(true);

    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRY_COUNT) - 1;
    gp.base = (unsigned int)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, gdt_kernel_code_access(), gran);
    gdt_set_gate(2, 0, 0xFFFFFFFF, gdt_kernel_data_access(), gran);
    gdt_set_gate(3, 0, 0xFFFFFFFF, gdt_user_code_access(), gran);
    gdt_set_gate(4, 0, 0xFFFFFFFF, gdt_user_data_access(), gran);

    gdt_set_tss(5, (unsigned long)&tss - KERNEL_VMA, sizeof(tss), false);

    tss.ss0 = 0x10;
    tss.esp0 = 0;
    tss.cs = 0x8 | 0x3;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = (0x10 | 0x3);

    gdt_flush();
    tss_flush();
}

void tss_set_stack(uintptr_t esp) {
    tss.esp0 = esp;
}

unsigned char gdt_kernel_code_access() {
    return gdt_access(true, true, 0, true);
}

unsigned char gdt_kernel_data_access() {
    return gdt_access(true, false, 0, true);
}

unsigned char gdt_user_code_access() {
    return gdt_access(true, true, 3, true);
}

unsigned char gdt_user_data_access() {
    return gdt_access(true, false, 3, true);
}

unsigned char gdt_access(bool rw, bool code, int ring, bool present) {
    int access = 0;
    if(rw) {
        GDT_SET_ACCESS_RW(&access);
    }
    if(code) {
        GDT_SET_ACCESS_CODE(&access);
    }
    GDT_SET_ACCESS_UNUSED_ALWAYS_1(&access);
    if(isset(&ring, 0)) {
        GDT_SET_ACCESS_DPL_1(&access);
    }
    if(isset(&ring, 1)) {
        GDT_SET_ACCESS_DPL_2(&access);
    }
    if(present) {
        GDT_SET_ACCESS_PRESENT(&access);
    }
    return (unsigned char)access;
}

unsigned char gdt_32bit_granularity(bool segment_4k_granularity) {
    int gran = 0;
    GDT_SET_GRAN_32(&gran);
    if(segment_4k_granularity) {
        GDT_SET_GRAN_GRANULARITY(&gran);
    }
    return (unsigned char)gran;
}

void gdt_set_tss(int num, unsigned long base, unsigned long limit, bool busy) {
    uint8_t access = 0;
    uint8_t gran = 0;

    GDT_SET_TSS_TYPE(&access);
    if(busy) {
        GDT_SET_TSS_BUSY(&access);
    }
    GDT_SET_TSS_DPL_1(&access);
    GDT_SET_TSS_DPL_2(&access);
    GDT_SET_TSS_PRESENT(&access);

    gdt_set_gate(num, base, limit, access, gran);
}
