KERNEL_LMA = 0x101000;
KERNEL_VMA = 0xC0000000;

ENTRY(_start)

SECTIONS
{
	. = KERNEL_LMA + KERNEL_VMA;

	.text  : AT(ADDR(.text) - KERNEL_VMA)
	{
		_setup = .;
		*(.setup)
		*(.multiboot)
		_code = .;
		*(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA)
	{
		_rodata = .;
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
   	{
		_data = .;
        	*(.data)
   	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA)
   	{
		_bss = .;
		*(COMMON)
       		*(.bss)
		*(.bootstrap_stack)
   	}

   	_end = .;

   	/DISCARD/ :
   	{
        	*(.comment)
   	}
}

