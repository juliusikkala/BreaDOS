#include "text/device.h"

void kernel_main(void)
{
    init_text_devices();

    struct text_device* terminal;
    get_text_devices(&terminal, 1);

    puts(terminal, "Ayy lmao\n");
}
