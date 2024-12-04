#include "scheduler.h"
#include "io.h"

void timer_handler(void) {
    tick++;

    outb(0x20, 0x20);

    // Switch to the next task
    scheduler_switch_task();
} 