#include "device.h"
#include "vga/driver.h"

/* TODO: Make this text_device* text_devices once we have kmalloc */
struct text_device text_devices[1];
size_t text_device_count = 0;

void init_text_devices()
{
    /* There should always be a VGA device, at least */
    text_devices[0].id = 0;
    text_devices[0].driver = &text_vga_driver;
    text_devices[0].driver->init(&text_devices[0]);

    if(text_devices[0].driver_data)
    {
        text_device_count = 1;
    }
}

void deinit_text_devices()
{
    for(size_t i = 0; i < text_device_count; ++i)
    {
        text_devices[i].driver->deinit(text_devices + i);
    }
    text_device_count = 0;
}

size_t get_text_device_count()
{
    return text_device_count;
}

size_t get_text_devices(
    struct text_device* devices[],
    size_t count
){
    if(text_devices == NULL) return 0;
    if(count > text_device_count) count = text_device_count;

    for(size_t i = 0; i < count; ++i)
    {
        devices[i] = &text_devices[i];
    }

    return text_device_count;
}

struct text_device* get_text_device(uint32_t id)
{
    if(!text_devices || !text_device_count) return NULL;

    for(size_t i = 0; i < text_device_count; ++i)
    {
        if(text_devices[i].id == id)
        {
            return text_devices+i;
        }
    }
    return NULL;
}

