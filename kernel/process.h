#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 64
#define PROCESS_NAME_MAX 32

typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;
    char name[PROCESS_NAME_MAX];
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    process_state_t state;
    uint32_t time_slice;
    uint32_t priority;
} process_t;

void process_init(void);
process_t* process_create(const char* name, void (*entry_point)(void));
void process_terminate(process_t* process);
void process_schedule(void);

#endif 