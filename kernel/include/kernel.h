#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include "../../kernel/include/io.h"

void init_gdt();
void init_idt();
void init_memory();

void kernel_main();

void set_idt_gate(uint8_t num, uint64_t handler, uint8_t ist, uint8_t flags);
void idt_flush(uint64_t);

void enable_interrupts();

#endif