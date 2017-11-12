#include "device.h"
#include "memory.h"
#include "drivers/vga.h"
#include "text/text.h"

/* TODO: Make this text_device** text_devices once we have kmalloc */
struct text_device* text_devices[1];
static size_t text_device_count = 0;
static uint32_t text_device_id_counter = 0;

void init_text_devices()
{
    text_device_count = 0;
    text_device_id_counter = 0;
}

void deinit_text_devices()
{
    for(size_t i = 0; i < text_device_count; ++i)
    {
        text_devices[i]->driver->deinit(text_devices[i]);
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
    if(!text_devices) return 0;
    if(count > text_device_count) count = text_device_count;

    for(size_t i = 0; i < count; ++i)
    {
        devices[i] = text_devices[i];
    }

    return count;
}

struct text_device* get_text_device(uint32_t id)
{
    if(!text_devices) return NULL;

    for(size_t i = 0; i < text_device_count; ++i)
    {
        if(text_devices[i]->id == id)
        {
            return text_devices[i];
        }
    }
    return NULL;
}

uint32_t add_text_device(struct text_device* device)
{
    text_devices[text_device_count++] = device;
    device->id = text_device_id_counter++;

    if(text_device_count == 1) set_default_text_device(device);

    return device->id;
}

void remove_text_device(uint32_t id)
{
    if(!text_devices) return;

    for(size_t i = 0; i < text_device_count; ++i)
    {
        if(text_devices[i]->id == id)
        {
            struct text_device* device = text_devices[i];
            memmove(
                text_devices + i,
                text_devices + i + 1,
                --text_device_count - i
            );
            if(get_default_text_device() == device)
            {
                set_default_text_device(
                    text_device_count ? text_devices[0] : NULL
                );
            }
            device->driver->deinit(device);
            return;
        }
    }
}
