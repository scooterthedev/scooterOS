#include "userspace.h"
#include "../vga.h"
#include "../keyboard.h"
#include "../string.h"
#include "../ui.h"
#include "../fs/scooterfs.h"

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
        terminal_writestring("  make_file <filename> - Create a new file\n");
        terminal_writestring("  dir_make <dirname> - Create a new directory\n");
        terminal_writestring("  delete <filename> - Delete a file or directory\n");
    } else if (strncmp(command, "make_file ", 10) == 0) {
        if (scooterfs_create_file(command + 10, false)) {
            terminal_writestring("\nFile created successfully\n");
        } else {
            terminal_writestring("\nFailed to create file\n");
        }
    } else if (strncmp(command, "dir_make ", 9) == 0) {
        if (scooterfs_create_file(command + 9, true)) {
            terminal_writestring("\nDirectory created successfully\n");
        } else {
            terminal_writestring("\nFailed to create directory\n");
        }
    } else if (strncmp(command, "delete ", 7) == 0) {
        if (scooterfs_delete_file(command + 7)) {
            terminal_writestring("\nFile deleted successfully\n");
        } else {
            terminal_writestring("\nFailed to delete file\n");
        }
    } else if (strcmp(command, "files") == 0) {
        terminal_initialize();
        window_t* explorer = ui_get_active_window();
        if (explorer) {
            explorer->visible = true;
            
            scooterfs_dirent entries[UI_MAX_FILES];
            size_t count = 0;
            if (scooterfs_list_directory(entries, &count)) {
                explorer->num_files = 0;
                for (size_t i = 0; i < count && i < UI_MAX_FILES; i++) {
                    strcpy(explorer->files[i].name, entries[i].name);
                    explorer->files[i].is_directory = (entries[i].type == 2);
                    explorer->files[i].size = entries[i].size;
                    explorer->files[i].created_time = entries[i].created_time;
                    explorer->files[i].modified_time = entries[i].modified_time;
                    explorer->num_files++;
                }
            }
            
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
    } else if (strcmp(command, "clear") == 0) {
        terminal_initialize();
    } else if (strcmp(command, "about") == 0) {
        terminal_writestring("\nScooterOS v0.0.0.0.1\n");
        terminal_writestring("A simple operating system with ScooterFS!\n");
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
