#ifndef USERSPACE_H
#define USERSPACE_H

#include <stdbool.h>
#include <stddef.h>
#include "../vga.h"
#include "../ui.h"

// Command buffer size for user shell
#define USER_COMMAND_BUFFER_SIZE 256

void init_userspace(void);
void switch_to_user_mode(void);
void user_shell_prompt(void);
void user_process_command(char* command);

extern void terminal_initialize(void);
extern void terminal_writestring(const char* data);
extern void terminal_setcolor(uint8_t color);
extern void terminal_backspace(void);
extern void terminal_putchar(char c);

#endif
