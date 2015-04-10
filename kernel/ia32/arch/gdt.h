/*
 * From JamesM's tutorials at http://www.jamesmolloy.co.uk/tutorial_html
 */

#ifndef IA32_ARCH_GDT_H
#define IA32_ARCH_GDT_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <arch/tss.h>

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __packed;

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __packed;

enum gdt_access_bits {
    gdt_access_accessed_bit = 0,
    gdt_access_rw_bit = 1,
    gdt_access_conforming_or_expand_down_bit = 2,
    gdt_access_code_bit = 3,
    gdt_access_unused_always_1_bit = 4,
    gdt_access_dpl_1_bit = 5,
    gdt_access_dpl_2_bit = 6,
    gdt_access_present_bit = 7
};

#define GDT_SET_ACCESS_ACCESSED(access) setbit(access, gdt_access_accessed_bit)
#define GDT_CLR_ACCESS_ACCESSED(access) clrbit(access, gdt_access_accessed_bit)
#define GDT_SET_ACCESS_RW(access) setbit(access, gdt_access_rw_bit)
#define GDT_CLR_ACCESS_RW(access) clrbit(access, gdt_access_rw_bit)
#define GDT_SET_ACCESS_CONFORMING_OR_EXPAND_DOWN(access) setbit(access, gdt_access_conforming_or_expand_down_bit)
#define GDT_CLR_ACCESS_CONFORMING_OR_EXPAND_DOWN(access) clrbit(access, gdt_access_conforming_or_expand_down_bit)
#define GDT_SET_ACCESS_CODE(access) setbit(access, gdt_access_code_bit)
#define GDT_CLR_ACCESS_CODE(access) clrbit(access, gdt_access_code_bit)
#define GDT_SET_ACCESS_UNUSED_ALWAYS_1(access) setbit(access, gdt_access_unused_always_1_bit)
#define GDT_CLR_ACCESS_UNUSED_ALWAYS_1(access) clrbit(access, gdt_access_unused_always_1_bit)
#define GDT_SET_ACCESS_DPL_1(access) setbit(access, gdt_access_dpl_1_bit)
#define GDT_CLR_ACCESS_DPL_1(access) clrbit(access, gdt_access_dpl_1_bit)
#define GDT_SET_ACCESS_DPL_2(access) setbit(access, gdt_access_dpl_2_bit)
#define GDT_CLR_ACCESS_DPL_2(access) clrbit(access, gdt_access_dpl_2_bit)
#define GDT_SET_ACCESS_PRESENT(access) setbit(access, gdt_access_present_bit)
#define GDT_CLR_ACCESS_PRESENT(access) clrbit(access, gdt_access_present_bit)

#define GDT_SET_TSS_TYPE(access) (((unsigned char *)access)[0] |= 9)
#define GDT_CLR_TSS_TYPE(access) (((unsigned char *)access)[0] &= ~9)
#define GDT_SET_TSS_BUSY(access) setbit(access, gdt_access_rw_bit)
#define GDT_CLR_TSS_BUSY(access) clrbit(access, gdt_access_rw_bit)
#define GDT_SET_TSS_DPL_1(access) setbit(access, gdt_access_dpl_1_bit)
#define GDT_CLR_TSS_DPL_1(access) clrbit(access, gdt_access_dpl_1_bit)
#define GDT_SET_TSS_DPL_2(access) setbit(access, gdt_access_dpl_2_bit)
#define GDT_CLR_TSS_DPL_2(access) clrbit(access, gdt_access_dpl_2_bit)
#define GDT_SET_TSS_PRESENT(access) setbit(access, gdt_access_present_bit)
#define GDT_CLR_TSS_PRESENT(access) clrbit(access, gdt_access_present_bit)

enum gdt_granularity_bits {
    gdt_gran_avl_bit = 4,
    gdt_gran_64_bit = 5,
    gdt_gran_32_bit = 6,
    gdt_gran_granularity_bit = 7
};

#define GDT_SET_GRAN_AVL(gran) setbit(gran, gdt_gran_avl_bit)
#define GDT_CLR_GRAN_AVL(gran) clrbit(gran, gdt_gran_avl_bit)
#define GDT_SET_GRAN_64(gran) setbit(gran, gdt_gran_64_bit)
#define GDT_CLR_GRAN_64(gran) clrbit(gran, gdt_gran_64_bit)
#define GDT_SET_GRAN_32(gran) setbit(gran, gdt_gran_32_bit)
#define GDT_CLR_GRAN_32(gran) clrbit(gran, gdt_gran_32_bit)
#define GDT_SET_GRAN_GRANULARITY(gran) setbit(gran, gdt_gran_granularity_bit)
#define GDT_CLR_GRAN_GRANULARITY(gran) clrbit(gran, gdt_gran_granularity_bit)

void gdt_flush();

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_set_tss(int num, unsigned long base, unsigned long limit, bool busy);

void gdt_install();

#define gdt_kernel_code_32_segment() 0x08
#define gdt_kernel_data_32_segment() 0x10
#define gdt_user_code_32_segment() 0x18
#define gdt_user_data_32_segment() 0x20

unsigned char gdt_kernel_code_access();
unsigned char gdt_kernel_data_access();
unsigned char gdt_user_code_access();
unsigned char gdt_user_data_access();

unsigned char gdt_access(bool rw, bool code, int ring, bool present);

unsigned char gdt_32bit_granularity(bool segment_4k_granularity);

void tss_flush(void);
void tss_set_stack(uintptr_t esp);

#endif
