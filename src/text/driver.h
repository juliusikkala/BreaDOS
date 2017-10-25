#ifndef BREADOS_TEXT_DRIVER_H
#define BREADOS_TEXT_DRIVER_H
#include "types.h"

struct text_device;

struct text_driver
{
    /* Name of the driver */
    const char* name;

    /* Gracefully deinitializes the driver and device. Must deallocate
     * the text_device* if necessary.
     */
    void (*const deinit)(
        struct text_device* device
    );

    /* Sets the color of the text output, picking the closest available color.
     */
    void (*const set_color_rgb)(
        struct text_device* device,
        uint32_t fg, /* Foreground color in RGB8 */
        uint32_t bg  /* Background color in RGB8 */
    );

    /* Writes a character */
    void (*const putc)(
        struct text_device* device,
        wchar c
    );

    /* Writes a string */
    void (*const puts)(
        struct text_device* device,
        const char* str
    );

    /* Writes a unicode string */
    void (*const putws)(
        struct text_device* device,
        const wchar* str
    );

    /* Sets the cursor position on screen */
    void (*const set_cursor_pos)(
        struct text_device* device,
        uint32_t x,
        uint32_t y
    );

    /* Selects whether the cursor is visible or not */
    void (*const set_cursor_visible)(
        struct text_device* device,
        bool visible
    );
};

#endif
