#include "scheduler.h"
#include "timer.h"
#include "interrupt.h"
#include "io.h"
#include "gdt.h"
#include "keyboard.h"
#include "ui.h"
#include "userspace/userspace.h"
#include "memory.h"

// An example for now.
void example_task1(void) {
    while(1) {
        terminal_writestring("Task 1 Running\n");
        sleep(100000);
    }
}

void example_task2(void) {
    while(1) {
        terminal_writestring("Task 2 Running\n");
        sleep(100000);
    }
}

void kernel_main(void) {
    terminal_initialize();
    gdt_init();
    idt_init();
    keyboard_init();
    memory_init();
    scheduler_init();
    timer_init(50); // Initialize timer with 50Hz frequency

    // creatting the task
    task_t *task1 = (task_t *)kmalloc(sizeof(task_t));
    task1->id = 1;
    task1->eip = (uint32_t)example_task1;
    task1->eflags = 0x202; // IF flag enabled
    task1->is_kernel = true;
    task1->state = TASK_STATE_READY;
    scheduler_add_task(task1);

    // creating the other task
    task_t *task2 = (task_t *)kmalloc(sizeof(task_t));
    task2->id = 2;
    task2->eip = (uint32_t)example_task2;
    task2->eflags = 0x202; // IF flag enabled
    task2->is_kernel = true;
    task2->state = TASK_STATE_READY;
    scheduler_add_task(task2);

    scheduler_start();

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