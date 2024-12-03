#include "idt.h"
#include <stddef.h>

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void keyboard_handler_int();
extern void mouse_handler_int();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // Clear out the entire IDT first
    for (size_t i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Set up keyboard handler (IRQ1)
    idt_set_gate(0x21, (uint32_t)keyboard_handler_int, 0x08, 0x8E);
    
    // Set up mouse handler (IRQ12)
    idt_set_gate(0x2C, (uint32_t)mouse_handler_int, 0x08, 0x8E);

    // Load IDT
    asm volatile ("lidt %0" : : "m"(idtp));
}
