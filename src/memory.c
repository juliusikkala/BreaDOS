#include "memory.h"

size_t wstrlen(const wchar* str)
{
    const wchar* start = str;
    while(*str++);
    return str - start;
}

size_t strlen(const char* str)
{
    const char* start = str;
    while(*str++);
    return str - start;
}

void* memcpy(void* dst, const void* src, size_t num)
{
    char* dstc = (char*)dst;
    const char* srcc = (const char*)src;
    while(num--) *dstc++ = *srcc++;
    return dst;
}

void* memmove(void* dst, const void* src, size_t num)
{
    char* dstc = (char*)dst;
    const char* srcc = (const char*)src;

    if(dstc < srcc)
    {
        while(num--) *dstc++ = *srcc++;
    }
    else if(srcc < dstc)
    {
        srcc += num-1;
        dstc += num-1;
        while(num--) *dstc-- = *srcc--;
    }

    return dst;
}
