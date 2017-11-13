#ifndef BREADOS_TEXT_TEXT_H
#define BREADOS_TEXT_TEXT_H
#include "driver.h"
#include "device.h"
#include <stdarg.h>

void set_default_text_device(struct text_device* device);
struct text_device* get_default_text_device();

/* Sets the color of the text output, picking the closest available color. */
static inline void set_color_rgb_dev(
    struct text_device* device,
    uint32_t fg, /* Foreground color in RGB8 */
    uint32_t bg  /* Background color in RGB8 */
){ device->driver->set_color_rgb(device, fg, bg); }

static inline void set_color_rgb(
    uint32_t fg, /* Foreground color in RGB8 */
    uint32_t bg  /* Background color in RGB8 */
){ set_color_rgb_dev(get_default_text_device(), fg, bg); }

/* Writes a character */
static inline void putc_dev(
    struct text_device* device,
    wchar c
){ device->driver->putc(device, c); }

static inline void putc(
    wchar c
){ putc_dev(get_default_text_device(), c); }

/* Writes a string */
static inline void puts_dev(
    struct text_device* device,
    const char* str
){ device->driver->puts(device, str); }

static inline void puts(
    const char* str
){ puts_dev(get_default_text_device(), str); }

/* Sets the cursor position on screen */
static inline void set_cursor_pos_dev(
    struct text_device* device,
    uint32_t x,
    uint32_t y
){ device->driver->set_cursor_pos(device, x, y); }

static inline void set_cursor_pos(
    uint32_t x,
    uint32_t y
){ set_cursor_pos_dev(get_default_text_device(), x, y); }

/* Selects whether the cursor is visible or not */
static inline void set_cursor_visible_dev(
    struct text_device* device,
    bool visible
){ device->driver->set_cursor_visible(device, visible); }

static inline void set_cursor_visible(
    bool visible
){ set_cursor_visible_dev(get_default_text_device(), visible); }

/* Prints a formatted string */
int printf_dev(struct text_device* device, const char* format, ...);
int printf(const char* format, ...);

int vprintf_dev(struct text_device* device, const char* format, va_list arg);
int vprintf(const char* format, va_list arg);

int sprintf(char* s, const char* format, ...);
int vsprintf(char* s, const char* format, va_list arg);

/* Converts a string to unsigned */
uint64_t strtoul(const char* str, char** endptr, int base);

/* Converts an unsigned to a string*/
char* ultostr(unsigned long u, char* str, int base);

/* Converts the string to lowercase*/
char* tolower(char* str);

/* Converts the string to uppercase*/
char* toupper(char* str);

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max_len);
char* strncpy(char* dst, const char* str, size_t num);

#endif

