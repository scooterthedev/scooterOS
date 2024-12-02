#ifndef _UI_H
#define _UI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "vga.h"

#define UI_MAX_WINDOWS 16
#define UI_MAX_TITLE 32
#define UI_MAX_FILES 64

typedef struct {
    char name[256];
    bool is_directory;
    size_t size;
} file_entry_t;

typedef struct {
    int x, y;
    int width, height;
    char title[UI_MAX_TITLE];
    bool active;
    bool visible;
    file_entry_t files[UI_MAX_FILES];
    size_t num_files;
    size_t selected_index;
    size_t scroll_offset;
} window_t;

void ui_init(void);
void ui_draw_window(window_t* window);
void ui_handle_input(char c);
window_t* ui_create_window(int x, int y, int width, int height, const char* title);
void ui_refresh(void);
window_t* ui_get_active_window(void);
void ui_set_active_window(window_t* window);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_write(const char* data, size_t size);

#endif 