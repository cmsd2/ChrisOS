#from osdev.org bare bones. believed public domain.

# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.set KERNEL_VMA, 0xc0000000      # 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VMA >> 22)

# map 1st 4MB of memory to itself,
# and also to where the higher half will run
# later will unmap 1st 4MB
.section .data
.align 4096
boot_page_directory:
	.int 0x83 # 4MB rw present
	.fill (KERNEL_PAGE_NUMBER - 1), 4, 0
	.int 0x83 # 4MB rw present
	.fill (1024 - KERNEL_PAGE_NUMBER - 1), 4, 0

# Declare a header as in the Multiboot Standard. We put this into a special
# section so we can force the header to be in the start of the final program.
# You don't need to understand all these details as it is just magic values that
# is documented in the multiboot standard. The bootloader will search for this
# magic sequence and recognize us as a multiboot kernel.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Currently the stack pointer register (esp) points at anything and using it may
# cause massive harm. Instead, we'll provide our own stack. We will allocate
# room for a small temporary stack by creating a symbol at the bottom of it,
# then allocating 16384 bytes for it, and finally creating a symbol at the top.
.section .bootstrap_stack
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# loaded physically at KERNEL_LMA
# linked as if running at KERNEL_VMA
.section .text

.set _start, (_start_lower_half - KERNEL_VMA)
.global _start

.global _start_lower_half
_start_lower_half:
	movl $(boot_page_directory - KERNEL_VMA), %ecx
	movl %ecx, %cr3

	movl %cr4, %ecx
	orl  $0x10, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	movl $0x80000000, %ecx
	movl %ecx, %cr0

	movl $_start_higher_half, %ecx
	jmp *%ecx

.global _start_higher_half
.type _start_higher_half, @function
_start_higher_half:
	#movl $0, (boot_page_directory)
	#invlpg (0)

	movl $stack_top, %esp

	# push multiboot magic number and info pointer
	pushl %ebx
	pushl %eax

	call kernel_main

	cli
	hlt
.Lhang:
	jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start_higher_half, . - _start_higher_half

