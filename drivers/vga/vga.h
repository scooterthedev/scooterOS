#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_init(void);
void vga_print(const char* str);
void vga_print_char(char c);
void vga_clear_screen(void);

#endif