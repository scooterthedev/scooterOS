#include <kernel.h>
#include <vga.h>
#include <memory.h>
#include <io.h>
#include "keyboard.h"
#include "timer.h"
#include "fat32.h"
#include "window_manager.h"
#include "explorer.h"
#include "binary.h"

extern char _binary_user_bin_start[];
extern char _binary_user_bin_end[];

#define USER_SPACE_START 0x200000 // 2MB
#define USER_SPACE_SIZE  (0x100000) // 1MB per user task

typedef void (*user_task_t)(void);

typedef struct {
    user_task_t entry;
    uint64_t stack;
} task_t;

#define MAX_TASKS 2
task_t tasks[MAX_TASKS];
int current_task = 0;

extern void switch_to_user(void (*user_func)());

extern void load_binary(char *start, size_t size);

void load_user_space() {
    size_t user_bin_size = _binary_user_bin_end - _binary_user_bin_start;
    load_binary(_binary_user_bin_start, user_bin_size);
}

void scheduler() {
    while (1) {
        switch_to_user(tasks[current_task].entry);
        current_task = (current_task + 1) % MAX_TASKS;
    }
}

void kernel_main(void) {
    // Initialize hardware
    vga_init();

    // Initialize system components
    init_gdt();
    init_idt();
    init_memory();
    keyboard_init();
    timer_init();
    fat32_init();
    window_manager_init();
    explorer_init();

    // Printing the boot message
    vga_print("ScooterOS Kernel Loaded!\n");
    vga_print("Initializing system...\n");

    // Load and setup user-space tasks
    load_user_space();

    // Start the scheduler
    scheduler();

    // Enter halt loop
    while(1) {
        __asm__ volatile("hlt");
    }
}