#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include "vga.h"

void terminal_initialize(void);
void terminal_writestring(const char* data);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

#endif 