#include <timer.h>
#include <vga.h>
#include <io.h>

// PIT setup
void timer_init() {
    // Programmable Interval Timer (PIT) setup
    uint32_t freq = 1193182 / 50; // 50 Hz
    outb(0x43, 0x36);             // Command byte: channel 0, access mode lobyte/hibyte, mode 3
    outb(0x40, (uint8_t)(freq & 0xFF));      // Send low byte
    outb(0x40, (uint8_t)((freq >> 8) & 0xFF)); // Send high byte
}

void timer_handler_c() {
    // Increment system tick or perform scheduling
    // For now, we will just print a dot every tick
    vga_print_char('.');
    
    // Send End of Interrupt (EOI) to PIC
    outb(0x20, 0x20);
}