OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_main)

SECTIONS
{
	/* . = 0xf000000000200000 ; */
	. = 0x200000 ;

	.text : ALIGN(0x1000)
	{
		*(.text)
	}
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
}

