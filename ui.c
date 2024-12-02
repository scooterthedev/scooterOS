#include "ui.h"
#include "string.h"

static window_t windows[UI_MAX_WINDOWS];
static size_t num_windows = 0;
static window_t* active_window = NULL;

//TODO: Going to replace with real filesystem later
static const file_entry_t sample_files[] = {
    {.name = "Documents", .is_directory = true, .size = 0},
    {.name = "Pictures", .is_directory = true, .size = 0},
    {.name = "readme.txt", .is_directory = false, .size = 1024},
    {.name = "hello.c", .is_directory = false, .size = 256},
};

static void int_to_str(int value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    int i = 0;
    bool negative = value < 0;
    if (negative) value = -value;

    while (value > 0) {
        str[i++] = '0' + (value % 10);
        value /= 10;
    }

    if (negative) str[i++] = '-';
    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - 1 - j];
        str[i - 1 - j] = temp;
    }
}

void ui_init(void) {
    window_t* explorer = ui_create_window(2, 2, 76, 20, "File Explorer");
    
    for (size_t i = 0; i < sizeof(sample_files) / sizeof(file_entry_t); i++) {
        explorer->files[i] = sample_files[i];
        explorer->num_files++;
    }
    
    explorer->visible = true;
    active_window = explorer;
}

void ui_draw_border(window_t* window) {
    uint8_t border_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    
    for (int x = window->x; x < window->x + window->width; x++) {
        terminal_putentryat('-', border_color, x, window->y);
        terminal_putentryat('-', border_color, x, window->y + window->height - 1);
    }
    
    for (int y = window->y + 1; y < window->y + window->height - 1; y++) {
        terminal_putentryat('|', border_color, window->x, y);
        terminal_putentryat('|', border_color, window->x + window->width - 1, y);
    }
    
    terminal_putentryat('+', border_color, window->x, window->y);
    terminal_putentryat('+', border_color, window->x + window->width - 1, window->y);
    terminal_putentryat('+', border_color, window->x, window->y + window->height - 1);
    terminal_putentryat('+', border_color, window->x + window->width - 1, window->y + window->height - 1);
}

void ui_draw_file_list(window_t* window) {
    uint8_t file_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    uint8_t selected_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
    
    for (size_t i = 0; i < window->num_files; i++) {
        int y = window->y + 2 + i - window->scroll_offset;
        if (y >= window->y + 2 && y < window->y + window->height - 1) {
            uint8_t color = (i == window->selected_index) ? selected_color : file_color;
            char size_str[16];
            int_to_str(window->files[i].size, size_str);
            
            // Clear line
            for (int x = window->x + 1; x < window->x + window->width - 1; x++) {
                terminal_putentryat(' ', color, x, y);
            }
            
            // Draw file entry
            terminal_putentryat(window->files[i].is_directory ? 'D' : 'F', color, window->x + 2, y);
            terminal_putentryat(' ', color, window->x + 3, y);
            
            const char* name = window->files[i].name;
            for (size_t j = 0; name[j] && j < 50; j++) {
                terminal_putentryat(name[j], color, window->x + 4 + j, y);
            }
            
            // Draw size right-aligned
            size_t size_len = strlen(size_str);
            for (size_t j = 0; j < size_len; j++) {
                terminal_putentryat(size_str[j], color, window->x + window->width - 2 - size_len + j, y);
            }
        }
    }
}

void ui_draw_window(window_t* window) {
    if (!window->visible) return;
    ui_draw_border(window);
    ui_draw_file_list(window);
}

window_t* ui_create_window(int x, int y, int width, int height, const char* title) {
    if (num_windows >= UI_MAX_WINDOWS) return NULL;
    
    window_t* window = &windows[num_windows++];
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    strcpy(window->title, title);
    window->visible = false;
    window->selected_index = 0;
    window->scroll_offset = 0;
    
    return window;
}

void ui_handle_input(char c) {
    if (!active_window) return;
    
    switch (c) {
        case 'w':  // Up
            if (active_window->selected_index > 0) {
                active_window->selected_index--;
            }
            break;
        case 's':  // Down
            if (active_window->selected_index < active_window->num_files - 1) {
                active_window->selected_index++;
            }
            break;
    }
}

void ui_refresh(void) {
    for (size_t i = 0; i < num_windows; i++) {
        ui_draw_window(&windows[i]);
    }
}

window_t* ui_get_active_window(void) {
    return active_window;
}

void ui_set_active_window(window_t* window) {
    active_window = window;
} 