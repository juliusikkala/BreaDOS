#ifndef BREADOS_TEXT_DEVICE_H
#define BREADOS_TEXT_DEVICE_H
#include "types.h"
#include "driver.h"

enum text_device_type
{
    TEXT_DEVICE_VGA_TEXT_MODE,
};

struct text_device
{
    uint32_t id;
    const char* name;

    uint32_t width, height;
    enum text_device_type type;

    const struct text_driver* driver;
    void* driver_data;
};

void init_text_devices();
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

/* Sets the color of the text output, picking the closest available color.
 */
static inline void set_color_rgb(
    struct text_device* device,
    uint32_t fg, /* Foreground color in RGB8 */
    uint32_t bg  /* Background color in RGB8 */
){ device->driver->set_color_rgb(device, fg, bg); }

/* Writes a character */
static inline void putc(
    struct text_device* device,
    wchar c
){ device->driver->putc(device, c); }

/* Writes a string */
static inline void puts(
    struct text_device* device,
    const char* str
){ device->driver->puts(device, str); }

/* Writes a unicode string */
static inline void putws(
    struct text_device* device,
    const wchar* str
){ device->driver->putws(device, str); }

/* Sets the cursor position on screen */
static inline void set_cursor_pos(
    struct text_device* device,
    uint32_t x,
    uint32_t y
){ device->driver->set_cursor_pos(device, x, y); }

/* Selects whether the cursor is visible or not */
static inline void set_cursor_visible(
    struct text_device* device,
    bool visible
){ device->driver->set_cursor_visible(device, visible); }

#endif
