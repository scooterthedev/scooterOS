#include "vga.h"

volatile uint16_t* const video_memory = (uint16_t*)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;

void vga_clear_screen(void) {
    for(int i = 0; i < 80 * 25; i++) {
        video_memory[i] = (uint16_t)' ' | (uint16_t)(0x07 << 8);
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_init(void) {
    vga_clear_screen();
}

void vga_print_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= 25) {
            // Scroll up the screen
            for(int i = 0; i < 80 * 24; i++) {
                video_memory[i] = video_memory[i + 80];
            }
            // Clear the last line
            for(int i = 80 * 24; i < 80 * 25; i++) {
                video_memory[i] = (uint16_t)' ' | (uint16_t)(0x07 << 8);
            }
            cursor_y = 24;
        }
        return;
    }

    const int index = cursor_y * 80 + cursor_x;
    video_memory[index] = (uint16_t)c | (uint16_t)(0x07 << 8);
    
    cursor_x++;
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= 25) {
            // Scroll up the screen
            for(int i = 0; i < 80 * 24; i++) {
                video_memory[i] = video_memory[i + 80];
            }
            // Clear the last line
            for(int i = 80 * 24; i < 80 * 25; i++) {
                video_memory[i] = (uint16_t)' ' | (uint16_t)(0x07 << 8);
            }
            cursor_y = 24;
        }
    }
}

void vga_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_print_char(str[i]);
    }
}