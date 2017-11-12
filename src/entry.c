#include "text/text.h"
#include "drivers/vga.h"
#include "io.h"
#include "memory.h"
#include "interrupt.h"
#include "multiboot.h"

void rye();

void kernel_main(struct multiboot_info* info)
{
    init_memory();
    init_interrupts();

    init_text_devices();
    init_vga();

    set_cursor_visible(false);

    puts("Text devices initialized.\n");
    puts("Nothing to do.\n");

    rye();

    asm volatile("cli\n1: hlt\njmp 1b");
}
