#include "text/device.h"
#include "drivers/vga.h"
#include "io.h"

void rye(struct text_device* terminal);

void kernel_main(void)
{
    init_text_devices();
    init_vga();

    struct text_device* terminal;
    get_text_devices(&terminal, 1);

    puts(terminal, "\nText devices initialized.\n");
    puts(terminal, "Nothing to do.\n");


    set_cursor_visible(terminal, false);

    /*
    uint32_t colors[] = {
        0x000000,
        0x0000aa,
        0x00aa00,
        0x00aaaa,
        0xaa0000,
        0xaa00aa,
        0xaa5500,
        0xaaaaaa,
        0x555555,
        0x5555ff,
        0x55ff55,
        0x55ffff,
        0xff5555,
        0xff55ff,
        0xffff55,
        0xffffff
    };
    for(uint32_t i = 0;; i=(i+1)%(sizeof(colors)/sizeof(uint32_t)))
    {
        set_color_rgb(terminal, colors[i], colors[15-i]);

        for(uint32_t j = 0; j < 0x10000; ++j)
        {
            set_cursor_pos(terminal, 0, 0);
            puts(terminal, "BreaDOS 0.0");
        }
    }*/
    rye(terminal);
}
