#ifndef BREADOS_TEXT_DEVICE_H
#define BREADOS_TEXT_DEVICE_H
#include "types.h"
#include "driver.h"

struct text_device
{
    /* The human readable name of the text device. */
    const char* name;

    /* The driver assigned to the device. */
    const struct text_driver* driver;

    /* Driver-specific data allocated and used by the driver. */
    void* driver_data;

    /* A unique identifier assigned to the device */
    uint32_t id;

    /* Width and height of the output screen. 0 means unlimited. */
    uint32_t width, height;
};

/* Must be called before any of the other text device functions. */
void init_text_devices();

/* Deallocates all memory assigned by init_text_devices() and other
 * text device functions, and deinits all text devices.
 */
void deinit_text_devices();

/* Returns the number of text devices */
size_t get_text_device_count();

/* Returns the actual size of the devices structure. */
size_t get_text_devices(
    struct text_device* devices[],
    size_t count
);

/* Returns NULL if there is no such device */
struct text_device* get_text_device(uint32_t id);

/* Adds a new device to the internal text device list, gives it an id and
 * returns that id. Does not take ownership of the pointer, so the deinit()
 * function of the driver should deallocate the text_device if necessary.
 */
uint32_t add_text_device(struct text_device* device);

/* Removes and deinitializes the given text device. */
void remove_text_device(uint32_t id);

#endif
