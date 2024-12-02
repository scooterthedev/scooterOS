#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

// Keys
#define KEY_ESCAPE    0x01
#define KEY_BACKSPACE 0x0E
#define KEY_TAB       0x0F
#define KEY_ENTER     0x1C
#define KEY_LCTRL     0x1D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_LALT      0x38
#define KEY_CAPSLOCK  0x3A
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44
#define KEY_NUMLOCK   0x45
#define KEY_SCROLLLOCK 0x46

typedef struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
} keyboard_state_t;

void keyboard_init(void);
void keyboard_update(void);
char keyboard_read_char(void);
keyboard_state_t keyboard_get_state(void);

#endif 