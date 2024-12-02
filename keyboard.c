#include "keyboard.h"
#include "io.h"

#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_BUFFER_SIZE  256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_start = 0;
static int buffer_end = 0;
static keyboard_state_t keyboard_state = {0};

// Regular scancode to ASCII mapping
static const char scancode_to_ascii[] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' '
};

// Shift mapping for special characters
static const char shift_mapping[] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' '
};

void keyboard_init(void) {
    buffer_start = 0;
    buffer_end = 0;
    
    keyboard_state.shift_pressed = false;
    keyboard_state.ctrl_pressed = false;
    keyboard_state.alt_pressed = false;
    keyboard_state.caps_lock = false;
    keyboard_state.num_lock = false;
    keyboard_state.scroll_lock = false;
}

static void keyboard_buffer_write(char c) {
    int next_end = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
    if (next_end != buffer_start) {
        keyboard_buffer[buffer_end] = c;
        buffer_end = next_end;
    }
}

void keyboard_update(void) {
    // Check if there's data to read
    if (!(inb(KEYBOARD_STATUS_PORT) & 1)) return;
    
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    bool release = scancode & 0x80;
    scancode &= 0x7F;  // Remove release bit

    // Handle modifier keys
    switch(scancode) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            keyboard_state.shift_pressed = !release;
            return;
        case KEY_LCTRL:
            keyboard_state.ctrl_pressed = !release;
            return;
        case KEY_LALT:
            keyboard_state.alt_pressed = !release;
            return;
        case KEY_CAPSLOCK:
            if (!release) keyboard_state.caps_lock = !keyboard_state.caps_lock;
            return;
        case KEY_NUMLOCK:
            if (!release) keyboard_state.num_lock = !keyboard_state.num_lock;
            return;
        case KEY_SCROLLLOCK:
            if (!release) keyboard_state.scroll_lock = !keyboard_state.scroll_lock;
            return;
    }

    // Only process key press events
    if (!release && scancode < sizeof(scancode_to_ascii)) {
        char ascii;
        if (keyboard_state.shift_pressed) {
            ascii = shift_mapping[scancode];
        } else {
            ascii = scancode_to_ascii[scancode];
            if (keyboard_state.caps_lock && ascii >= 'a' && ascii <= 'z') {
                ascii -= 32;  // Convert to uppercase
            }
        }
        
        if (ascii) {
            keyboard_buffer_write(ascii);
        }
    }
}

char keyboard_read_char(void) {
    keyboard_update();
    
    if (buffer_start == buffer_end) {
        return 0;
    }
    
    char c = keyboard_buffer[buffer_start];
    buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

keyboard_state_t keyboard_get_state(void) {
    return keyboard_state;
} 