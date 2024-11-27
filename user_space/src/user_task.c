#include <stdint.h>

void print(const char* str);

void user_task(void) __attribute__((section(".user_task_text")));
void user_task(void) {
    print("User Task 2 running...\n");
    while(1) {
        print("User Task 2 is active.\n");
        for(volatile int i = 0; i < 1000000; i++); // Busy wait
    }
} 