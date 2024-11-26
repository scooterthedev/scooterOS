#include <keyboard.h>
#include <vga.h>
#include <io.h>

static uint8_t keycode_to_ascii(uint8_t scancode) {
    // Simplified scancode to ASCII mapping
    static char scancode_map[256] = {
        [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
        [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
        [0x0A] = '9', [0x0B] = '0', [0x1C] = '\n', [0x39] = ' ',
    };
    return scancode_map[scancode];
}

void keyboard_init() {
    // This enables the keyboard interrupt
    set_idt_gate(33, (uint64_t)keyboard_interrupt_handler, 0, 0x8E);
    enable_interrupts();
}

void keyboard_interrupt_handler_c() {
    uint8_t scancode = inb(0x60);
    char c = keycode_to_ascii(scancode);
    if (c != 0) {
        vga_print_char(c);
    }
    // Send End of Interrupt (EOI) to PIC
    outb(0x20, 0x20);
}