ENTRY(_start)
 
SECTIONS
{
    . = 0x00100000;

	.multiboot BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
	}

    .boot BLOCK(4K) : ALIGN(4K)
    {
        *(.boot)
    }

    g_kernel_start = .;

	.text ALIGN(4K) : AT(ADDR(.text))
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata))
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data))
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss))
	{
		*(COMMON)
		*(.bss)
	}

    g_kernel_end = .;
}
