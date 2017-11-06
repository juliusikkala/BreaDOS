#ifndef BREADOS_MEMORY_H
#define BREADOS_MEMORY_H
#include "types.h"

size_t wstrlen(const wchar* str);
size_t strlen(const char* str);
void* memcpy(void* dst, const void* src, size_t num);
void* memmove(void* dst, const void* src, size_t num);

volatile void* vmemcpy(
    volatile void* dst,
    volatile const void* src,
    size_t num
);
volatile void* vmemmove(
    volatile void* dst,
    volatile const void* src,
    size_t num
);

#endif
