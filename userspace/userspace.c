#include "userspace.h"
#include "../vga.h"
#include "../keyboard.h"
#include "../string.h"
#include "../ui.h"

#define USER_COMMAND_BUFFER_SIZE 256
static char user_command_buffer[USER_COMMAND_BUFFER_SIZE];
static size_t user_command_index = 0;
static bool user_prompt_needed = true;

// Process user commands
void user_process_command(char* command) {
    if (strcmp(command, "help") == 0) {
        terminal_writestring("\nAvailable commands:\n");
        terminal_writestring("  help - Show this help message\n");
        terminal_writestring("  clear - Clear the screen\n");
        terminal_writestring("  about - Show system information\n");
        terminal_writestring("  files - Opens file manager\n");
    } else if (strcmp(command, "clear") == 0) {
        terminal_initialize();
    } else if (strcmp(command, "about") == 0) {
        terminal_writestring("\nScooterOS v0.0.0.0.1\n");
        terminal_writestring("A simple operating system for now!\n");
    } else if (strcmp(command, "files") == 0) {
        terminal_initialize();
        window_t* explorer = ui_get_active_window();
        if (explorer) {
            explorer->visible = true;
            ui_refresh();
            
            while (1) {
                char c = keyboard_read_char();
                if (c == 'q') {
                    explorer->visible = false;
                    terminal_initialize();
                    break;
                }
                ui_handle_input(c);
                ui_refresh();
            }
        }
    } else {
        terminal_writestring("\nUnknown command. Type 'help' for available commands.\n");
    }
}

void user_shell_prompt(void) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("\nScooterOS> ");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void init_userspace(void) {
    // This is for clearing the command buffer
    memset(user_command_buffer, 0, USER_COMMAND_BUFFER_SIZE);
}

void switch_to_user_mode(void) {
    while (1) {
        if (user_prompt_needed) {
            user_shell_prompt();
            user_prompt_needed = false;
        }

        char c = keyboard_read_char();
        if (c) {
            if (c == '\n') {
                user_command_buffer[user_command_index] = '\0';
                user_process_command(user_command_buffer);
                user_command_index = 0;
                user_prompt_needed = true;
            } else if (c == '\b') {
                if (user_command_index > 0) {
                    user_command_index--;
                    terminal_backspace();
                }
            } else if (user_command_index < USER_COMMAND_BUFFER_SIZE - 1) {
                user_command_buffer[user_command_index++] = c;
                terminal_putchar(c);
            }
        }
    }
}
