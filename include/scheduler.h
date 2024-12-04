#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS 64
#define TASK_STATE_RUNNING 0
#define TASK_STATE_READY 1
#define TASK_STATE_BLOCKED 2

typedef struct task {
    uint32_t id;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t eflags;
    bool is_kernel;
    int state;
} task_t;

void scheduler_init(void);
void scheduler_add_task(task_t *task);
task_t* scheduler_next_task(void);
void scheduler_switch_task(void);
void scheduler_start(void);

void switch_task_asm(task_t *current_task, task_t *next_task);

#endif // SCHEDULER_H 