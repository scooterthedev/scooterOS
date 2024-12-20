#include "kernel/process.h"
#include "memory.h"
#include "string.h"
#include "scheduler.h"

static process_t processes[MAX_PROCESSES];
static uint32_t next_pid = 1;
static uint32_t current_process = 0;

void process_init(void) {
    memset(processes, 0, sizeof(processes));
}

process_t* process_create(const char* name, void (*entry_point)(void)) {
    if (next_pid >= MAX_PROCESSES) return NULL;
    
    process_t* process = &processes[next_pid];
    process->pid = next_pid++;
    strncpy(process->name, name, PROCESS_NAME_MAX - 1);
    process->state = PROCESS_STATE_READY;
    process->priority = 1;
    process->time_slice = 100;
    
    // Allocate stack
    void* stack = kmalloc(4096);
    if (!stack) return NULL;
    
    process->esp = (uint32_t)stack + 4096;
    process->ebp = process->esp;
    process->eip = (uint32_t)entry_point;
    
    task_t* task = kmalloc(sizeof(task_t));
    if (!task) {
        kfree(stack);
        return NULL;
    }
    
    task->id = process->pid;
    task->eip = process->eip;
    task->esp = process->esp;
    task->ebp = process->ebp;
    task->eflags = 0x202;
    task->is_kernel = true;
    task->state = TASK_STATE_READY;
    
    scheduler_add_task(task);
    
    return process;
}

void process_terminate(process_t* process) {
    if (!process) return;
    process->state = PROCESS_STATE_TERMINATED;
    // TODO: Free resources
} 