#include "editor.h"
#include "../vga.h"
#include "../keyboard.h"
#include "../ui.h"
#include <string.h>
#include "../fs/scooterfs.h"

#define VGA_WIDTH 80
extern keyboard_state_t keyboard_state;
extern uint8_t terminal_color;

editor_t editor;
bool editor_active = false;

void editor_init(void) {
    editor.cursor_x = 0;
    editor.cursor_y = 0;
    editor.num_lines = 1;
    editor.scroll_offset = 0;
    editor.modified = false;
    editor_active = false;
    editor.filename[0] = '\0';
    
    for (size_t i = 0; i < MAX_LINES; i++) {
        editor.lines[i][0] = '\0';
    }
}

void editor_draw(void) {
    terminal_initialize();
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY));
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, 0);
    }
    
    terminal_writestring(" Editor - ");
    terminal_writestring(editor.filename[0] ? editor.filename : "Untitled");
    terminal_writestring(editor.modified ? " [Modified]" : "");
    terminal_writestring(" | Ctrl+S: Save | ESC: Exit");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    for (size_t i = 0; i < EDITOR_HEIGHT && i + editor.scroll_offset < editor.num_lines; i++) {
        size_t line_num = i + editor.scroll_offset;
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, i + 1);
        }
        for (size_t j = 0; editor.lines[line_num][j] && j < MAX_LINE_LENGTH; j++) {
            terminal_putentryat(editor.lines[line_num][j], terminal_color, j, i + 1);
        }
    }
}

bool editor_save(void) {
    if (!editor.filename[0]) return false;
    
    scooterfs_create_file(editor.filename, false);
    editor.modified = false;
    return true;
}

void editor_open(const char* filename) {
    editor_init();
    strcpy(editor.filename, filename);
    editor_active = true;
}

void editor_handle_input(char c) {
    if (c == 19 && keyboard_state.ctrl_pressed) {
        if (editor_save()) {
            editor.modified = false;
            editor_draw();
        }
        return;
    }
    
    editor.modified = true;
    
    if (c == '\n') {
        if (editor.num_lines < MAX_LINES - 1) {
            editor.cursor_x = 0;
            editor.cursor_y++;
            editor.num_lines++;
        }
    } else if (c == '\b') {
        if (editor.cursor_x > 0) {
            editor.cursor_x--;
            editor.lines[editor.cursor_y + editor.scroll_offset][editor.cursor_x] = '\0';
        }
    } else {
        if (editor.cursor_x < MAX_LINE_LENGTH - 1) {
            editor.lines[editor.cursor_y + editor.scroll_offset][editor.cursor_x] = c;
            editor.lines[editor.cursor_y + editor.scroll_offset][editor.cursor_x + 1] = '\0';
            editor.cursor_x++;
        }
    }
    editor_draw();
} 