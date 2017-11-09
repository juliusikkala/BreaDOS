OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

KERNEL_LMA = 1M;
KERNEL_VMA = 0xFFFFFFFF80000000;

SECTIONS
{
    . = KERNEL_LMA;

    .bootstrap.text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *bootstrap.o (.text)
        . = ALIGN(4K);
    }

    .bootstrap.data BLOCK(4K) : ALIGN(4K)
    {
        *bootstrap.o (.data)
        . = ALIGN(4K);
    }

    .bootstrap.bss :
    {
        *bootstrap.o (.bss)
        . = ALIGN(4K);
    }

    . += KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(EXCLUDE_FILE(*bootstrap.o) .text)
        *(.rodata*)
        . = ALIGN(4K);
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA)
    {
        *(EXCLUDE_FILE(*bootstrap.o) .data)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA)
    {
        *(EXCLUDE_FILE(*bootstrap.o) .rodata*)
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA)
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }

    /DISCARD/ : {
        *(.comment)
        *(note.*)
    }
}
