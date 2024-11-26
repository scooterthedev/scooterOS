#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <kernel.h>

// Function declarations
void keyboard_init(void);
void keyboard_interrupt_handler_c(void);
extern void keyboard_interrupt_handler(void);

#endif // KEYBOARD_H