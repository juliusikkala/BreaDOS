#include "vga.h"
#include "text/device.h"
#include "text/driver.h"
#include "io.h"
#include "memory.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_GRAY = 7,
	VGA_COLOR_DARK_GRAY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_YELLOW = 14,
	VGA_COLOR_WHITE = 15,
};

struct text_vga_data
{
    uint32_t x, y;
    uint8_t color;
    volatile uint16_t* buffer;
    bool cursor_visible;
} text_vga_data;

static inline uint8_t combine_vga_color(enum vga_color fg, enum vga_color bg)
{
    return fg | (bg << 4);
}

static inline uint16_t vga_char(unsigned char c, uint8_t color)
{
    return c | ((uint16_t)color << 8);
}

static inline void vga_enable_color()
{
    outb(0x3C2, inb(0x3CC)|1);
    io_pause();
}

static inline void vga_update_cursor(struct text_vga_data* data)
{
    if(data->cursor_visible)
    {
        uint16_t pos = data->y * VGA_WIDTH + data->x;
        if(pos >= VGA_WIDTH * VGA_HEIGHT) pos = VGA_WIDTH * VGA_HEIGHT - 1;

        outb(0x3D4, 0x0F);
        outb(0x3D5, pos & 0xFF);
        outb(0x3D4, 0x0E);
        outb(0x3D5, pos >> 8);
    }
}

static inline void vga_enable_cursor(struct text_vga_data* data)
{
    if(!data->cursor_visible)
    {
        /* Set cursor start line*/
        outb(0x3D4, 0x0A);
        outb(0x3D5, inb(0x3D5) & 0xC0);

        /* Set cursor end line */
        outb(0x3D4, 0x0B);
        outb(0x3D5, (inb(0x3D5) & 0xE0) | 0x1F);

        data->cursor_visible = true;

        vga_update_cursor(data);
    }
}

static inline void vga_disable_cursor(struct text_vga_data* data)
{
    if(data->cursor_visible)
    {
        outb(0x3D4, 0x0A);
        outb(0x3D5, 0x20);

        data->cursor_visible = false;
    }
}

static inline void vga_clear(struct text_vga_data* data)
{
    data->x = data->y = 0;
    vga_update_cursor(data);

    for(uint32_t i = 0; i < VGA_HEIGHT*VGA_WIDTH; ++i)
    {
        data->buffer[i] = vga_char(' ', data->color);
    }
}

static inline void vga_reset(struct text_vga_data* data)
{
    data->color = combine_vga_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_enable_color();
    vga_enable_cursor(data);
    vga_clear(data);
}

static inline void vga_newline(struct text_vga_data* data)
{
    data->x = 0;
    /* Check if there are empty lines left on screen */
    if(data->y < VGA_HEIGHT-1)
    {
        data->y++;
    }
    /* Otherwise, we'll have to make an empty line.*/
    else
    {
        /* Scroll previous lines */
        vmemmove(
            data->buffer,
            data->buffer + VGA_WIDTH,
            (VGA_HEIGHT-1)*VGA_WIDTH*2
        );
        /* Clear the last line */
        for(uint32_t x = 0; x < VGA_WIDTH; ++x)
        {
            data->buffer[(VGA_HEIGHT-1)*VGA_WIDTH + x] =
                vga_char(' ', data->color);
        }
    }
}

static inline void vga_write_char(struct text_vga_data* data, char c)
{
    switch(c)
    {
    case '\n':
        vga_newline(data);
        break;
    case '\r':
        data->x = 0;
        break;
    default:
        if(data->x == VGA_WIDTH) vga_newline(data);
        data->buffer[data->y * VGA_WIDTH + data->x] = vga_char(c, data->color);
        data->x++;
        break;
    };
}

static inline enum vga_color rgb8_to_vga(uint32_t rgb8)
{
#define COL(a,b,c) ((a<<22)|(b<<14)|(c<<6))
    switch(rgb8&0xC0C0C0)
    {
    case COL(0, 0, 0):
        return VGA_COLOR_BLACK;
    case COL(0, 0, 1):
    case COL(0, 0, 2):
    case COL(1, 0, 2):
    case COL(0, 1, 2):
    case COL(1, 1, 2):
        return VGA_COLOR_BLUE;
    case COL(0, 1, 0):
    case COL(0, 2, 0):
    case COL(1, 2, 0):
    case COL(0, 2, 1):
    case COL(1, 2, 1):
        return VGA_COLOR_GREEN;
    case COL(0, 1, 1):
    case COL(0, 2, 2):
    case COL(1, 2, 2):
        return VGA_COLOR_CYAN;
    case COL(1, 0, 0):
    case COL(2, 0, 0):
    case COL(2, 0, 1):
    case COL(2, 1, 1):
        return VGA_COLOR_RED;
    case COL(1, 0, 1):
    case COL(2, 0, 2):
    case COL(2, 1, 2):
        return VGA_COLOR_MAGENTA;
    case COL(1, 1, 0):
    case COL(2, 1, 0):
    case COL(2, 2, 0):
    case COL(2, 2, 1):
        return VGA_COLOR_BROWN;
    case COL(2, 2, 2):
        return VGA_COLOR_GRAY;
    case COL(1, 1, 1):
        return VGA_COLOR_DARK_GRAY;
    case COL(0, 0, 3):
    case COL(1, 0, 3):
    case COL(0, 1, 3):
    case COL(2, 0, 3):
    case COL(0, 2, 3):
    case COL(1, 1, 3):
    case COL(2, 1, 3):
    case COL(1, 2, 3):
    case COL(2, 2, 3):
        return VGA_COLOR_LIGHT_BLUE;
    case COL(0, 3, 0):
    case COL(1, 3, 0):
    case COL(0, 3, 1):
    case COL(2, 3, 0):
    case COL(0, 3, 2):
    case COL(1, 3, 1):
    case COL(2, 3, 1):
    case COL(1, 3, 2):
    case COL(2, 3, 2):
        return VGA_COLOR_LIGHT_GREEN;
    case COL(0, 3, 3):
    case COL(1, 3, 3):
    case COL(2, 3, 3):
        return VGA_COLOR_LIGHT_CYAN;
    case COL(3, 0, 0):
    case COL(3, 1, 0):
    case COL(3, 0, 1):
    case COL(3, 2, 0):
    case COL(3, 0, 2):
    case COL(3, 1, 1):
    case COL(3, 2, 1):
    case COL(3, 1, 2):
    case COL(3, 2, 2):
        return VGA_COLOR_LIGHT_RED;
    case COL(3, 0, 3):
    case COL(3, 1, 3):
    case COL(3, 2, 3):
        return VGA_COLOR_LIGHT_MAGENTA;
    case COL(3, 3, 0):
    case COL(3, 3, 1):
    case COL(3, 3, 2):
        return VGA_COLOR_YELLOW;
    default:
    case COL(3, 3, 3):
        return VGA_COLOR_WHITE;
    };
#undef COL
}

static void text_vga_init(
    struct text_device* device
){
    device->name = "VGA";
    device->width = VGA_WIDTH;
    device->height = VGA_HEIGHT;
    device->driver_data = &text_vga_data;

    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    data->buffer = (uint16_t*) (KERNEL_VMA + 0xB8000);
    data->cursor_visible = false;
    data->x = data->y = 0;

    vga_reset(data);
}

void text_vga_deinit(
    struct text_device* device
){
    device->driver_data = NULL;
}

void text_vga_set_color_rgb(
    struct text_device* device,
    uint32_t fg,
    uint32_t bg
){
    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    data->color = combine_vga_color(rgb8_to_vga(fg), rgb8_to_vga(bg));
}

void text_vga_putc(
    struct text_device* device,
    wchar c
){
    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    vga_write_char(data, c);
    vga_update_cursor(data);
}

void text_vga_puts(
    struct text_device* device,
    const char* str
){
    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    while(*str) vga_write_char(data, *str++);
    vga_update_cursor(data);
}

void text_vga_set_cursor_pos(
    struct text_device* device,
    uint32_t x,
    uint32_t y
){
    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    data->x = x < VGA_WIDTH ? x : VGA_WIDTH - 1;
    data->y = y < VGA_HEIGHT ? y : VGA_HEIGHT - 1;
    vga_update_cursor(data);
}

void text_vga_set_cursor_visible(
    struct text_device* device,
    bool visible
){
    struct text_vga_data* data = (struct text_vga_data*) device->driver_data;
    if(visible)
    {
        vga_enable_cursor(data);
    }
    else
    {
        vga_disable_cursor(data);
    }
}

const struct text_driver text_vga_driver = {
    .name = "VGA text mode",
    .deinit = text_vga_deinit,
    .set_color_rgb = text_vga_set_color_rgb,
    .putc = text_vga_putc,
    .puts = text_vga_puts,
    .set_cursor_pos = text_vga_set_cursor_pos,
    .set_cursor_visible = text_vga_set_cursor_visible,
};

struct text_device text_vga_device = {
    .name = "VGA",
    .driver = &text_vga_driver,
    .driver_data = &text_vga_data,
    .width = VGA_WIDTH,
    .height = VGA_HEIGHT,
};

void init_vga()
{
    text_vga_init(&text_vga_device);
    add_text_device(&text_vga_device);
}

void deinit_vga()
{
    remove_text_device(text_vga_device.id);
}
