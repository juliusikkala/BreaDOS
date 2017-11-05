ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *bootstrap.o (.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *bootstrap.o (.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *bootstrap.o (.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *bootstrap.o (COMMON)
        *bootstrap.o (.bss)
    }

    . += 0;

    .text : AT(ADDR(.text) - 0)
    {
        _code = .;
        *(EXCLUDE_FILE(*bootstrap.o) .text)
        *(.rodata*)
        . = ALIGN(4096);
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - 0)
    {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - 0)
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }
}
