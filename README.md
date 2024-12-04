# ScooterOS

ScooterOS is a minimalist operating system written in C. It provides basic OS functionality such as memory management, process scheduling, a file system called ScooterFS, and a text-based user interface.

## Features

- **Memory Management**: Simple block-based memory allocation system
- **Process Scheduler**: Basic round-robin task scheduling
- **File System**: Custom ScooterFS implementation with basic file operations
- **User Interface**: Text-based UI with window management
- **Keyboard Driver**: Full keyboard support with modifier keys
- **GDT & IDT**: Proper segmentation and interrupt handling

## Building

### Prerequisites

- i686-elf cross-compiler
- GRUB
- QEMU (for testing)

### Build Instructions

1. Clone the repository
2. Run make: `make`

This will create `scooterOS.iso` in the current directory.

### Running

To run ScooterOS in QEMU:

`make run`

## Usage

When ScooterOS boots, you'll be presented with a boot prompt. Available commands are as follows:

- `boot` - Start the operating system
- `help` - Show available commands
- `clear` - Clear the screen
- `about` - Show system information

After booting, the following user commands become available:

- `help` - Show available commands
- `clear` - Clear the screen
- `about` - Show system information
- `files` - Open file manager
- `make_file <filename>` - Create a new file
- `dir_make <dirname>` - Create a new directory
- `delete <filename>` - Delete a file or directory

## System Components

1. **Boot Process**: Uses GRUB multiboot specification
2. **Memory Manager**: 4KB block-based allocation
3. **Process Scheduler**: Simple round-robin scheduling
4. **File System**: ScooterFS with basic file operations
5. **Device Drivers**: Keyboard and basic VGA support
6. **User Interface**: Text-based window management
7. **Shell**: Basic command interpreter

## Technical Details

- **Architecture**: x86 (32-bit)
- **Memory Model**: Flat memory model with basic segmentation
- **File System Block Size**: 4096 bytes
- **Maximum Files**: 64
- **Maximum Tasks**: 64
- **Display**: 80x25 VGA text mode

## Project Structure

```
/
├── boot.s              # Assembly boot code
├── kernel.c            # Main kernel code
├── string.c            # String manipulation functions
├── keyboard.c          # Keyboard driver
├── ui.c               # User interface code
├── memory.c           # Memory management
├── scheduler.c        # Task scheduler
├── context_switch.s   # Assembly context switching
├── gdt.c             # Global Descriptor Table
├── idt.c             # Interrupt Descriptor Table
├── userspace/
│   ├── userspace.c   # User mode implementation
│   └── userspace.h   # User mode headers
├── fs/
│   ├── scooterfs.c   # File system implementation
│   └── scooterfs.h   # File system headers
├── include/
│   ├── fs_common.h   # Common file system definitions
│   ├── scheduler.h   # Scheduler definitions
│   ├── interrupt.h   # Interrupt handling
│   └── io.h         # I/O operations
└── Makefile          # Build configuration
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests!

### Development Guidelines

1. Follow the existing code style
2. Add comments for non-obvious code
3. Update documentation when possible to new features

## Future Improvements

1. Implement virtual memory management
2. Add support for multiple file systems
3. Improve process scheduling algorithm
4. Add networking support
5. Implement a GUI
6. Add support for mice.