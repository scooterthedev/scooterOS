#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void print(const char* str) {
    volatile char* vga = (volatile char*)VGA_ADDRESS;
    while (*str) {
        *vga++ = *str++;
        *vga++ = 0x07; // Light grey on black
    }
}

void user_main(void) __attribute__((section(".user_text")));
void user_main(void) {
    print("Welcome to ScooterOS User Space!\n");
    while(1) {
        print("User Task 1 running...\n");
        for(volatile int i = 0; i < 1000000; i++); // Busy wait
    }
}
