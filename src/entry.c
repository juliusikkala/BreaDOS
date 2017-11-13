#include "text/text.h"
#include "drivers/vga.h"
#include "io.h"
#include "memory.h"
#include "interrupt.h"
#include "multiboot.h"

void rye();

void dump_mmap(struct multiboot_info* info)
{
    printf(
        "Memory map:\n"
        "---------------------------------\n"
    );
    uint32_t len = info->mmap_length;
    uint64_t* addr = (char*)info->mmap_addr + KERNEL_VMA + 4;
    uint64_t* end_addr = (char*)addr + len;
    while(addr < end_addr)
    {
        uint32_t size = ((uint32_t*)addr)[-1];
        uint64_t base_addr = addr[0];
        uint64_t length = addr[1];
        uint32_t type = ((uint32_t*)addr)[4];
        const char* type_string = "";

        switch(type)
        {
        case 1:
            type_string = "available";
            break;
        case 3:
            type_string = "ACPI memory";
            break;
        case 4:
            type_string = "reserved (hibernation)";
            break;
        case 5:
            type_string = "defective";
            break;
        default:
            type_string = "reserved";
            break;
        }

        printf(
            "base_addr: %p\n"
            "length:    %18u KiB\n"
            "type:      %18s (%d)\n"
            "---------------------------------\n",
            base_addr,
            length / 1024,
            type_string,
            type
        );
        addr = (char*)addr + size + 4;
    }
}

void kernel_main(struct multiboot_info* info)
{
    init_memory();
    init_interrupts();

    init_text_devices();
    init_vga();

    set_cursor_visible(false);

    puts("Text devices initialized.\n");
    puts("Nothing to do.\n");

    char* cmdline = ((char*)info->cmdline + KERNEL_VMA);
    printf(
        "Multiboot info (%p):\n"
        "flags:          %0#18b\n"
        "mem_lower:      %18u KiB\n"
        "mem_upper:      %18u KiB\n"
        "boot_device:    %18u\n"
        "cmdline:        %p (%.7s)\n"
        "mmap_length:    %18u\n"
        "mmap_addr:      %p\n",
        info,
        info->flags,
        info->mem_lower,
        info->mem_upper,
        info->boot_device,
        cmdline,
        cmdline,
        info->mmap_length,
        info->mmap_addr
    );

    dump_mmap(info);

    asm volatile("cli\n1: hlt\njmp 1b");
}
