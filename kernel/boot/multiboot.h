#ifndef KERNEL_MULTIBOOT_H
#define KERNEL_MULTIBOOT_H

#include <stdint.h>

/* loaded by boot if available */
extern uint32_t _magic;
extern uintptr_t _bootstrap_multiboot_info;
extern struct multiboot_info _multiboot_info;

void load_multiboot_info(void);

void multiboot_copy_mem_map_to_allocator(void);

void multiboot_print_info(void);
void multiboot_print_flags(void);
void multiboot_print_mem_info(void);
void multiboot_print_bootdev_info(void);
void multiboot_print_cmdline_info(void);
void multiboot_print_mods_info(void);
void multiboot_print_aout_syms_info(void);
void multiboot_print_elf_shdr_info(void);
void multiboot_print_mem_map_info(void);
void multiboot_print_drive_info(void);
void multiboot_print_config_table_info(void);
void multiboot_print_boot_loader_name_info(void);
void multiboot_print_apm_table_info(void);
void multiboot_print_vbe_info(void);
void multiboot_print_framebuffer_info(void);

#endif
