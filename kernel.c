#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "io.h"
#include "vga.h"
#include "keyboard.h"
#include "ui.h"
#include "userspace/userspace.h"
#include "gdt.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

#define COMMAND_BUFFER_SIZE 256
char command_buffer[COMMAND_BUFFER_SIZE];
size_t command_index = 0;

uint32_t last_status_update = 0;

volatile uint32_t tick = 0;

bool prompt_needed = true;

typedef struct {
    const char* name;
    size_t weight;
} component_t;

static const component_t components[] = {
    {"Memory Manager", 15},
    {"Process Scheduler", 20},
    {"File System", 25},
    {"Device Drivers", 20},
    {"User Interface", 15},
    {"Shell", 5}
};

static const size_t num_components = sizeof(components) / sizeof(component_t);

#define MODE_SHELL 0
#define MODE_FILE_EXPLORER 1
uint8_t current_mode = MODE_SHELL;

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
    
    terminal_putentryat(progress < 100 ? ' ' : '1', terminal_color, start_x + bar_width + 2, line);
    terminal_putentryat(progress < 10 ? ' ' : ((progress / 10) % 10) + '0', terminal_color, start_x + bar_width + 3, line);
    terminal_putentryat((progress % 10) + '0', terminal_color, start_x + bar_width + 4, line);
    terminal_putentryat('%', terminal_color, start_x + bar_width + 5, line);
}

void shell_prompt(void) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("\nScooterOS> ");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void process_command(char* command) {
    if(strcmp(command, "help") == 0) {
        terminal_writestring("\nAvailable commands:\n");
        terminal_writestring("  help - Show this help message\n");
        terminal_writestring("  clear - Clear the screen\n");
        terminal_writestring("  about - Show system information\n");
        terminal_writestring("  boot - Boot into ScooterOS\n");
    } else if(strcmp(command, "clear") == 0) {
        terminal_initialize();
    } else if(strcmp(command, "about") == 0) {
        terminal_writestring("\nScooterOS v0.0.0.0.1\n");
        terminal_writestring("A simple operating system for now\n");
    } else if(strcmp(command, "boot") == 0) {
        terminal_initialize();
        terminal_setcolor(vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK));
        
        terminal_writestring("\n\n\n    Initializing ScooterOS...\n\n");
        
        size_t total_progress = 0;
        for (size_t i = 0; i < num_components; i++) {
            terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
            terminal_writestring("\n    Loading ");
            terminal_writestring(components[i].name);
            terminal_writestring("...");
            
            size_t start_progress = total_progress;
            size_t end_progress = total_progress + components[i].weight;
            
            for (size_t p = start_progress; p <= end_progress; p++) {
                draw_loading_bar(p, 5);
                sleep(2000);
            }
            
            total_progress = end_progress;
        }
        
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal_writestring("\n\n    All components loaded successfully!\n");
        sleep(10000);
        
        terminal_initialize();
        
        init_userspace();
        
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
        terminal_writestring("Welcome to ScooterOS!\n");
        
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
        terminal_writestring("Version 0.0.0.0.1\n");
        
        terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        terminal_writestring("Be Bop Be Bop Be Bop\n");
        
        terminal_writestring("\nSwitching to user mode...\n");
        switch_to_user_mode();
    } else if (strncmp(command, "compile ", 8) == 0) {
        char* filename = command + 8;
        terminal_writestring("\nCompiling ");
        terminal_writestring(filename);
        terminal_writestring("...\n");
        
        char* source = scooterfs_read_file(filename);
        if (!source) {
            terminal_writestring("Error: Could not read source file\n");
            return;
        }
        
        lexer_t* lexer = lexer_create(source);
        parser_t* parser = parser_create(lexer);
        
        ast_node_t* ast = parser_parse(parser);
        if (!ast) {
            terminal_writestring("Error: Parsing failed\n");
            return;
        }
        
        ir_instr_t* ir = generate_ir(ast);
        if (!ir) {
            terminal_writestring("Error: IR generation failed\n");
            return;
        }
        
        char output_file[256];
        strcpy(output_file, filename);
        size_t len = strlen(output_file);
        if (len > 2 && output_file[len-2] == '.' && output_file[len-1] == 'c') {
            output_file[len-1] = 's';
        } else {
            strcat(output_file, ".s");
        }
        
        char assembly_output[4096];
        generate_assembly(ir, assembly_output);
        
        if (scooterfs_write_file(output_file, assembly_output, strlen(assembly_output))) {
            terminal_writestring("Compilation successful: ");
            terminal_writestring(output_file);
            terminal_writestring("\n");
        } else {
            terminal_writestring("Error: Failed to write output file\n");
        }
        
        parser_destroy(parser);
        lexer_destroy(lexer);
    } else {
        terminal_writestring("\nUnknown command. Type 'help' for available commands.\n");
    }
}

void update_status_bar() {
    size_t original_row = terminal_row;
    size_t original_column = terminal_column;
    uint8_t original_color = terminal_color;

    terminal_row = VGA_HEIGHT - 1;
    terminal_column = 0;
    terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY));

    // Clear the status bar line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);
    }

    terminal_row = VGA_HEIGHT - 1;
    terminal_column = 0;
    terminal_writestring(" ScooterOS | Mode: ");
    terminal_writestring(current_mode == MODE_SHELL ? "Shell" : "File Explorer");
    terminal_writestring(" | Tab to switch | Memory: OK");

    terminal_row = original_row;
    terminal_column = original_column;
    terminal_color = original_color;
}

#define PIT_FREQUENCY 1193180
#define TIMER_COMMAND_PORT 0x43
#define TIMER_DATA_PORT 0x40

void timer_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;
    outb(TIMER_COMMAND_PORT, 0x36);
    outb(TIMER_DATA_PORT, divisor & 0xFF);
    outb(TIMER_DATA_PORT, (divisor >> 8) & 0xFF);
}

void timer_handler() {
    tick++;
}

void terminal_backspace() {
    if (terminal_column > 0) {
        terminal_column--;
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    } else if (terminal_row > 0) {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    }
}

void kernel_main(void) {
    terminal_initialize();
    gdt_init();
    keyboard_init();
    ui_init();
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("Welcome to ScooterOS Boot Prompt!\n");
    terminal_writestring("Type 'boot' to start the system or 'help' for available commands.\n\n");
    
    while(1) {
        if(prompt_needed) {
            shell_prompt();
            prompt_needed = false;
        }

        char c = keyboard_read_char();
        if(c) {
            if(c == '\n') {
                command_buffer[command_index] = '\0';
                process_command(command_buffer);
                command_index = 0;
                prompt_needed = true;
            } else if(c == '\b') {
                if(command_index > 0) {
                    command_index--;
                    terminal_backspace();
                }
            } else if(command_index < COMMAND_BUFFER_SIZE - 1) {
                command_buffer[command_index++] = c;
                terminal_putchar(c);
            }
        }
    }
} 