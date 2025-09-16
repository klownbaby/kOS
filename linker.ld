ENTRY(_start)
 
SECTIONS
{
    . = 0x00100000;

    _kernel_start = .;

	  /* Grub */
		.multiboot BLOCK(4K) : ALIGN(4K)
		{
				*(.multiboot)
		}

	  .boot BLOCK(4K) : ALIGN(4K)
	  {
	      *(.boot)
	  }

	  /* Dynamic modules entry points (drivers) */
	  .dynmod BLOCK(4K) : ALIGN(4K)
	  {
		    _dynmod_start = .;
		    /* Ordered driver entries */
		    *(SORT(.dynmod.init*))
		    /* Unordered driver entries */
		    *(.dynmod)
		    _dynmod_end = .;
		}

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
		    _data_start = .;
				*(.data)
		}
 
		/* Read-write data (uninitialized) and stack */
		.bss ALIGN(4K) : AT(ADDR(.bss))
		{
				*(COMMON)
				*(.bss)
		}

    _kernel_end = .;
}
