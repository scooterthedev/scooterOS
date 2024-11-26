#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern void switch_to_user(void (*user_func)());

void user_main(void) {
    printf("Welcome to ScooterOS User Space!\n");
    while(1) {
        printf("User Task 1 running...\n");
        for(volatile int i = 0; i < 1000000; i++); // Busy wait
    }
}
