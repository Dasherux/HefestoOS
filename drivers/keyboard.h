#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum {
    KEY_NONE,
    KEY_CHAR,

    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_ESCAPE,

    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT

} KeyType;

typedef struct {
    KeyType type;
    char character;

} KeyEvent;

KeyEvent keyboard_getkey(void);

#endif
