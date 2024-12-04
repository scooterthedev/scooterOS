#include "scheduler.h"
#include "io.h"
#include "memory.h"
#include "vga.h"

extern void switch_task_asm(task_t *current_task, task_t *next_task);

// Define a simple circular queue for tasks
static task_t *tasks[MAX_TASKS];
static int current_task_id = 0;
static int task_count = 0;

void scheduler_init(void) {
    // Initialize the scheduler task list
    for(int i = 0; i < MAX_TASKS; i++) {
        tasks[i] = 0;
    }
    current_task_id = 0;
    task_count = 0;
}

void scheduler_add_task(task_t *task) {
    if(task_count < MAX_TASKS) {
        // Allocate a stack for the task
        void *stack = kmalloc(4096); // Allocate 4KB stack
        if(!stack) {
            terminal_writestring("\nScheduler: Failed to allocate stack for task.\n");
            return;
        }

        // Initialize stack pointer (assuming stack grows downwards)
        task->esp = (uint32_t)stack + 4096 - sizeof(uint32_t); // Leave space for return address
        task->ebp = task->esp;

        tasks[task_count++] = task;
    } else {
        // Handle task overflow
        terminal_writestring("\nScheduler: Maximum task limit reached.\n");
    }
}

task_t* scheduler_next_task(void) {
    if(task_count == 0) {
        return 0;
    }
    // Simple round-robin scheduling
    current_task_id = (current_task_id + 1) % task_count;
    return tasks[current_task_id];
}

void scheduler_switch_task(void) {
    static task_t *current = NULL;
    task_t *next = scheduler_next_task();
    
    if (!next) {
        return;
    }

    if (current != next) {
        switch_task_asm(current, next);
        current = next;
    }
}

void scheduler_start(void) {
    asm volatile ("sti"); // Enable interrupts
} 