#ifndef BREADOS_MEMORY_H
#define BREADOS_MEMORY_H
#include "types.h"
#define KERNEL_VMA 0xFFFFFFFF80000000

void init_memory();

void vmap_page(void* vaddr, void* paddr);
void vmap_pages(void* vaddr, void* paddr, size_t count);

void set_msr(uint32_t msr, uint64_t val);
uint64_t get_msr(uint32_t msr);

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
