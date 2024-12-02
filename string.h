#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

size_t strlen(const char* str);
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);

#endif 