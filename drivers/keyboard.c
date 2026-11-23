#include "keyboard.h"
#include "../system/io.h"

static int shift = 0;
static int capslock = 0;

static char translate_letter(unsigned char scancode) {
    switch (scancode) {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        case 0x20: return 'd';
        case 0x12: return 'e';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x17: return 'i';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x32: return 'm';
        case 0x31: return 'n';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x10: return 'q';
        case 0x13: return 'r';
        case 0x1F: return 's';
        case 0x14: return 't';
        case 0x16: return 'u';
        case 0x2F: return 'v';
        case 0x11: return 'w';
        case 0x2D: return 'x';
        case 0x15: return 'y';
        case 0x2C: return 'z';

        default: return 0;
    }
}

static char translate_symbol(unsigned char scancode) {
    switch (scancode) {
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';

        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';

        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';

        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';

        case 0x2B: return shift ? '|' : '\\';

        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';

        case 0x39: return ' ';

        default: return 0;
    }
}

KeyEvent keyboard_getkey(void) {
    unsigned char scancode;
    char character;

    KeyEvent event;

    event.type = KEY_NONE;
    event.character = 0;

    /* Espera uma tecla */
    while (!(inb(0x64) & 1)) {}
    scancode = inb(0x60);
	
    /* Shift */
    if (scancode == 0x2A || scancode == 0x36) {shift = 1; return event;}
    if (scancode == 0xAA || scancode == 0xB6) {shift = 0; return event;}
	
    /* Caps Lock */
    if (scancode == 0x3A) {capslock = !capslock; return event;}
	
    /* Ignora key release */
    if (scancode & 0x80) {return event;}
	
    /* Enter */
    if (scancode == 0x1C) {event.type = KEY_ENTER; return event;}
	
    /* Backspace */
    if (scancode == 0x0E) {event.type = KEY_BACKSPACE; return event;}
	
    /* Tab */
    if (scancode == 0x0F) {event.type = KEY_TAB; return event;}
	
    /* Escape */
    if (scancode == 0x01) {event.type = KEY_ESCAPE; return event;}
	
    /* Letras */
    character = translate_letter(scancode);
    if (character != 0) {if (shift ^ capslock) {character -= 32;}
        event.type = KEY_CHAR;
        event.character = character;

        return event;
    }
    /* Símbolos */
    character = translate_symbol(scancode);
    if (character != 0) {
        event.type = KEY_CHAR;
        event.character = character;

        return event;
    }

    return event;
}