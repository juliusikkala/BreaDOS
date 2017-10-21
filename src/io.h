#ifndef BREADOS_IO_H
#define BREADOS_IO_H
#include "types.h"

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile(
        "inb %w1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile(
        "inw %w1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile(
        "inl %w1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

static inline void outb(uint16_t port, uint8_t value)
{
    asm volatile(
        "outb %b0, %w1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline void outw(uint16_t port, uint16_t value)
{
    asm volatile(
        "outw %w0, %w1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline void outl(uint16_t port, uint32_t value)
{
    asm volatile(
        "outl %0, %w1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint8_t insb(uint16_t port, void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep insb\n"
        : "=D" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline uint16_t insw(uint16_t port, void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep insw\n"
        : "=D" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline uint32_t insl(uint16_t port, void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep insl\n"
        : "=D" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline void outsb(uint16_t port, const void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep outsb\n"
        : "=S" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline void outsw(uint16_t port, const void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep outsw\n"
        : "=S" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline void outsl(uint16_t port, const void* values, size_t count)
{
    asm volatile(
        "cld\n"
        "rep outsl\n"
        : "=S" (values), "=c" (count)
        : "0" (values), "1" (count), "d" (port)
    );
}

static inline void io_pause()
{
    asm volatile("outb %%al, $0x80");
}
#endif
