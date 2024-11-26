#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern void switch_to_user(void (*user_func)());

void user_task(void) {
    printf("User Task 2 running...\n");
    while(1) {
        printf("User Task 2 is active.\n");
        for(volatile int i = 0; i < 1000000; i++); // Busy wait
    }
} 
