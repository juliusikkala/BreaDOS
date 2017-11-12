#ifndef BREADOS_INTERRUPT_H
#define BREADOS_INTERRUPT_H
#include "types.h"

#if defined(__clang__) || defined(__GNUC__)
#define interrupt __attribute__((interrupt))
#else
#error "Clang or GCC required for interrupt handlers!"
#endif

void init_interrupts();

struct interrupt_frame;

void set_isr(
    uint8_t vector,
    void interrupt (*isr)(struct interrupt_frame* frame)
);

void clear_isr(uint8_t vector);

static inline void cli() { asm volatile("cli\n"); }
static inline void sti() { asm volatile("sti\n"); }

#endif

