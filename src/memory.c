#include "memory.h"

void init_memory()
{
    // Unmap identity mapping created in bootstrap.s
    uint64_t* pml4t = (uint64_t*)(KERNEL_VMA + 0x1000);
    uint64_t* pdpt = (uint64_t*)(KERNEL_VMA + 0x2000);

    *pml4t = 0;
    *pdpt = 0;

    // Flush TLB
    asm volatile(
        "movq %%cr3, %%rax\n"
        "movq %%rax, %%cr3\n"
        :
        :
        : "rax"
    );
}

void set_msr(uint32_t msr, uint64_t val)
{
    uint32_t eax = val & 0xFFFFFFFF, edx = val >> 32;
    asm(
        "wrmsr\n"
        :
        : "a" (eax), "d" (edx), "c"(msr)
    );
}

uint64_t get_msr(uint32_t msr)
{
    uint32_t eax, edx;
    asm(
        "rdmsr\n"
        : "=a" (eax), "=d" (edx)
        : "c"(msr)
    );
    return ((uint64_t)edx << 32) | eax;
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

volatile void* vmemcpy(
    volatile void* dst,
    volatile const void* src,
    size_t num
){
    volatile char* dstc = (volatile char*)dst;
    const char* srcc = (const char*)src;
    while(num--) *dstc++ = *srcc++;
    return dst;
}

volatile void* vmemmove(
    volatile void* dst,
    volatile const void* src,
    size_t num
){
    volatile char* dstc = (volatile char*)dst;
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
