#ifndef BINARY_H
#define BINARY_H

#include <stddef.h>

/**
 * @brief Loads a binary file into memory.
 *
 * @param start Pointer to the start of the binary data.
 * @param size  Size of the binary data in bytes.
 */
void load_binary(char *start, size_t size);

#endif // BINARY_H 