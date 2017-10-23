#include "text/device.h"

void kernel_main(void)
{
    init_text_devices();

    struct text_device* terminal;
    get_text_devices(&terminal, 1);


    set_color_rgb(terminal, 0xAAAA00, 0x000000);
    puts(terminal, "BreaDOS 0.0\n");
    set_color_rgb(terminal, 0xFFFFFF, 0x000000);
    puts(terminal, "Text devices initialized.\n");
    puts(terminal, "Nothing to do.\n");
}
