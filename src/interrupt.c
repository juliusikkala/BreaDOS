#include "interrupt.h"
#include "apic.h"
#define INTERRUPT_GATE 0x0E
#define PRESENT_BIT 0x80

volatile struct idt_entry
{
    uint16_t offset_0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type;
    uint16_t offset_1;
    uint32_t offset_2;
    uint32_t zero;
} idt[256] = {0};

#pragma pack(push)
#pragma pack(1)
struct idtr
{
    uint16_t limit;
    volatile struct idt_entry* table_base;
} idtr = { sizeof(idt), idt };
#pragma pack(pop)

void init_interrupts()
{
    asm volatile("lidt %0" : : "m" (idtr));
    init_apic();
}

void set_isr(
    uint8_t vector,
    void interrupt (*isr)(struct interrupt_frame* frame)
){
    uint64_t offset = (uint64_t) isr;
    volatile struct idt_entry* entry = &idt[vector];
    entry->offset_0 = offset & 0xFFFF;
    entry->offset_1 = (offset >> 16) & 0xFFFF;
    entry->offset_2 = offset >> 32;
    entry->selector = 8;
    entry->ist = 0;
    entry->type = PRESENT_BIT | INTERRUPT_GATE;
}

void clear_isr(uint8_t vector)
{
    volatile struct idt_entry* entry = &idt[vector];
    entry->offset_0 = 0;
    entry->offset_1 = 0;
    entry->offset_2 = 0;
    entry->selector = 0;
    entry->ist = 0;
    entry->type = 0;
}
