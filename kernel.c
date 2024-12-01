#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// VGA colors
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_scroll() {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t current_index = y * VGA_WIDTH + x;
            const size_t next_index = (y + 1) * VGA_WIDTH + x;
            terminal_buffer[current_index] = terminal_buffer[next_index];
        }
    }

    // Clear the last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++)
        terminal_putchar(data[i]);
}

void sleep(int ticks) {
    for (volatile int i = 0; i < ticks * 10000; i++) {}
}

void clear_line(size_t line) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, line);
    }
}

void draw_loading_bar(size_t progress, size_t line) {
    const size_t bar_width = 40;
    const size_t start_x = (VGA_WIDTH - bar_width) / 2;
    
    terminal_putentryat('[', terminal_color, start_x - 1, line);
    terminal_putentryat(']', terminal_color, start_x + bar_width, line);
    
    size_t filled = (progress * bar_width) / 100;
    for (size_t x = 0; x < bar_width; x++) {
        char c = x < filled ? '=' : ' ';
        terminal_putentryat(c, terminal_color, start_x + x, line);
    }
    
    char percentage[5];
    terminal_putentryat(progress < 100 ? ' ' : '1', terminal_color, start_x + bar_width + 2, line);
    terminal_putentryat(progress < 10 ? ' ' : ((progress / 10) % 10) + '0', terminal_color, start_x + bar_width + 3, line);
    terminal_putentryat((progress % 10) + '0', terminal_color, start_x + bar_width + 4, line);
    terminal_putentryat('%', terminal_color, start_x + bar_width + 5, line);
}

void kernel_main(void) {
    terminal_initialize();
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
    
    terminal_writestring("\n\n\n    Initializing ScooterOS...\n\n");
    
    for (size_t i = 0; i <= 100; i += 5) {
        draw_loading_bar(i, 5);
        sleep(4000);
    }
    
    terminal_initialize();
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("Welcome to ScooterOS!\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("Version 0.0.0.0.1\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("Be Bop Be Bop Be Bop\n");
} 