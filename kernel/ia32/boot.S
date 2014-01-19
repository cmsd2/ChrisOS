# based on code from osdev.org
.section .text

# Multiboot constants
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.set KERNEL_VMA, 0xc0000000      # 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VMA >> 22)

# this section is linked in the high half,
# which isn't ideal, but we adjust the addresses and cope.
.section .setup
.align 4096

_bootstrap_page_directory:
# page 1 4MB starting at 0x0
.int 0x83
# pages 2-767
.fill 767, 4, 0
# page 768
.int 0x83
# page 769-1024
.fill 255, 4, 0
_bootstrap_page_directory_end:

gdt_desc:
	.short (gdt_end - gdt - 1) # size-1 of the GDT
	.int (gdt - KERNEL_VMA) # physical address of GDT
 
.align 8
gdt:
	# 3 entries, each 8 bytes long:

	# null gate:
	.int 0, 0

	# code selector 0x08: base 0x00000000, limit 0xFFFFFFFF, 
	# type 0x9A, granularity 0xCF
	.byte 0xFF, 0xFF, 0, 0, 0, 0x9A, 0xCF, 0x00

	# data selector 0x10: base 0x00000000, limit 0xFFFFFFFF, 
	# type 0x92, granularity 0xCF
	.byte 0xFF, 0xFF, 0, 0, 0, 0x92, 0xCF, 0x00

	# ring 3 entries
	#.byte 0xFF, 0xFF, 0, 0, 0, 0xFA, 0xCF, 0x00
	#.byte 0xFF, 0xFF, 0, 0, 0, 0xF2, 0xCF, 0x00
gdt_end:
 


.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bootstrap_stack
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.globl _start
_start:
	# TODO: is this necessary or wise?
	cli

	# save multiboot info
	movl %eax, (_magic - KERNEL_VMA)
	movl %ebx, (_bootstrap_multiboot_info - KERNEL_VMA)

    # simple identity gdt. no tricks.
	mov $gdt_desc, %ebx
	subl $KERNEL_VMA, %ebx
	lgdt (%ebx)

    # setup bootstrap paging so that kernel is mapped to higher half
    mov %cr4, %eax
    or $0x10, %eax
    mov %eax, %cr4

    mov $_bootstrap_page_directory - KERNEL_VMA, %eax
    mov %eax, %cr3
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

	jmp $0x08, $_start_higher_half

_start_higher_half:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
 
	mov $stack_top, %esp
 
	call kmain
 
	cli
	hlt
.Lhang:
	jmp .Lhang
 
# this function does the same thing of the 'start' one, this time with
# the real GDT
.globl gdt_flush
gdt_flush:
	lgdt (gp)
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	jmp $0x08, $flush2
 
flush2:
	ret
 

.section .bss
 

