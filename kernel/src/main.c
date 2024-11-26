#include <kernel.h>
#include <vga.h>
#include <memory.h>
#include <io.h>

extern uint8_t _binary_user_bin_start;
extern uint8_t _binary_user_bin_end;
extern uint8_t _binary_user_task_bin_start;
extern uint8_t _binary_user_task_bin_end;

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

void load_user_space() {
    // Load user_main
    uint64_t user_main_size = &_binary_user_bin_end - &_binary_user_bin_start;
    uint8_t* user_main_addr = (uint8_t*)USER_SPACE_START;

    for(uint64_t i = 0; i < user_main_size; i++) {
        user_main_addr[i] = _binary_user_bin_start + i;
    }

    tasks[0].entry = (user_task_t)user_main_addr;
    tasks[0].stack = USER_SPACE_START + user_main_size + 0x1000; // Allocate 4KB for stack

    // Load user_task
    uint64_t user_task_size = &_binary_user_task_bin_end - &_binary_user_task_bin_start;
    uint8_t* user_task_addr = (uint8_t*)(USER_SPACE_START + USER_SPACE_SIZE);

    for(uint64_t i = 0; i < user_task_size; i++) {
        user_task_addr[i] = _binary_user_task_bin_start + i;
    }

    tasks[1].entry = (user_task_t)user_task_addr;
    tasks[1].stack = USER_SPACE_START + USER_SPACE_SIZE + user_task_size + 0x1000; // Allocate 4KB for stack
}

void scheduler() {
    while(1) {
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