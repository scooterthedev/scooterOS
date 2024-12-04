#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

void idt_init(void);
void keyboard_init(void);
void timer_init(uint32_t frequency);

#endif // INTERRUPT_H 