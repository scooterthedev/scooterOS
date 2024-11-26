#include "../include/explorer.h"
#include "../../drivers/vga/vga.h"
#include "../../fs/fat32/fat32.h"

void explorer_init() {
    // Initialize file explorer
    vga_print("Initializing File Explorer...\n");
}

void display_directory(const char* path) {
    // Display the contents of the specified directory
    vga_print("Displaying directory: ");
    vga_print(path);
    vga_print("\n");
} 