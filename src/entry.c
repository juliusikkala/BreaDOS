#include "text/device.h"
#include "drivers/vga.h"
#include "io.h"

void rye(struct text_device* terminal);

void kernel_main(void)
{
    init_memory();

    init_text_devices();
    init_vga();

    struct text_device* terminal;
    get_text_devices(&terminal, 1);

    set_cursor_visible(terminal, false);

    puts(terminal, "Text devices initialized.\n");
    puts(terminal, "Nothing to do.\n");

    rye(terminal);

    asm volatile("cli\n1: hlt\njmp 1b");
}
