#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>
#include "../vga.h"
#include "../keyboard.h"

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 80
#define EDITOR_HEIGHT 23

typedef struct {
    char filename[256];
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    size_t cursor_x;
    size_t cursor_y;
    size_t num_lines;
    size_t scroll_offset;
    size_t length;
    bool modified;
} editor_t;

extern bool editor_active;
void editor_init(void);
void editor_open(const char* filename);
void editor_draw(void);
void editor_handle_input(char c);
bool editor_save(void);

#endif 