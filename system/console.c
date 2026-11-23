#include <stdint.h>

#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "io.h"

#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define WIDTH 80
#define HEIGHT 25

static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t color = 0x07;

static void update_cursor(void) {
	uint16_t pos = cursor_y * WIDTH + cursor_x;
	
	outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void scroll_screen(void) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			VGA_MEMORY[(y - 1) * WIDTH + x] = VGA_MEMORY[y * WIDTH + x];
		}
	}
	uint16_t blank = (uint16_t)color << 8 | ' ';
	for (int x = 0; x < WIDTH; x++) {
		VGA_MEMORY[(HEIGHT - 1) * WIDTH + x] = blank;
	}
	cursor_y = HEIGHT - 1;
}

void repaint_screen(void) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        uint8_t character = VGA_MEMORY[i] & 0xFF;
        VGA_MEMORY[i] = (uint16_t)color << 8 | character;
    }
}

void cls(void) {
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		VGA_MEMORY[i] = (uint16_t)color << 8 | ' ';
	}
	cursor_x = 0;
    cursor_y = 0;
	update_cursor();
}

void putchar(char c) {
	if (c == '\n') {
		cursor_x = 0;
		cursor_y++;
	} else if (c == '\b') {
		if (cursor_x > 0) {
			cursor_x--;
			VGA_MEMORY[cursor_y * WIDTH + cursor_x] = (uint16_t)color << 8 | ' ';
		}
	} else {
		VGA_MEMORY[cursor_y * WIDTH + cursor_x] = (uint16_t)color << 8 | c;
		cursor_x++;
	}
	
	if (cursor_x >= WIDTH) {
		cursor_x = 0;
		cursor_y++;
	}
	if (cursor_y >= HEIGHT) scroll_screen();
	
	update_cursor();
}

void print(const char* str) {
	while (*str) {
		putchar(*str++);
	}
	update_cursor();
}

void input(char* buffer, unsigned int max_size) {
    unsigned int pos = 0;
    while (1) {
        KeyEvent key = keyboard_getkey();
        if (key.type == KEY_ENTER) {
            buffer[pos] = '\0';
            putchar('\n');
            return;
        }
        if (key.type == KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                putchar('\b');
            }
        }
        if (key.type == KEY_CHAR) {
            if (pos < max_size - 1) {
                buffer[pos++] = key.character;
                putchar(key.character);
            }
        }
    }
}

enum color_codes {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW        = 14,
    VGA_COLOR_WHITE         = 15,
};

void set_color(enum color_codes fg, enum color_codes bg) {
    color = fg | (bg << 4);
}

int parse_color(char* args) {
	tolower(args);
	
	if (strcmp(args, "black"))           	return VGA_COLOR_BLACK;
	else if (strcmp(args, "blue"))       	return VGA_COLOR_BLUE;
	else if (strcmp(args, "green"))      	return VGA_COLOR_GREEN;
	else if (strcmp(args, "cyan"))       	return VGA_COLOR_CYAN;
	else if (strcmp(args, "red"))        	return VGA_COLOR_RED;
	else if (strcmp(args, "magenta"))  	 	return VGA_COLOR_MAGENTA;
	else if (strcmp(args, "brown"))      	return VGA_COLOR_BROWN;
	else if (strcmp(args, "lightgrey"))  	return VGA_COLOR_LIGHT_GREY;
	else if (strcmp(args, "darkgrey")) 		return VGA_COLOR_DARK_GREY;
	else if (strcmp(args, "lightblue"))	  	return VGA_COLOR_LIGHT_BLUE;
	else if (strcmp(args, "lightgreen")) 	return VGA_COLOR_LIGHT_GREEN;
	else if (strcmp(args, "lightcyan"))  	return VGA_COLOR_LIGHT_CYAN;
	else if (strcmp(args, "lightred"))   	return VGA_COLOR_LIGHT_RED;
	else if (strcmp(args, "lightmagenta"))	return VGA_COLOR_LIGHT_BLUE;
	else if (strcmp(args, "yellow")) 		return VGA_COLOR_YELLOW;
	else if (strcmp(args, "white"))  		return VGA_COLOR_WHITE;
	return -1;
}

void change_colors(char* args) {
	while (*args == ' ') args++;
	if (*args == '\0') {
		print(
			"Usage: color <text> <background>\n\n"
			"Colors: BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGREY, DARKGREY\n"
			"        LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE\n\n"
		);
	}
	
	char* fg_str = args;
	char* bg_str = 0;
	
	while (*args != '\0' && *args != ' ') args++;
	if (*args == ' ') {
		*args = '\0';
		args++;
		while (*args == ' ') args++;
		if (*args != '\0') {
			bg_str = args;
		}
	}
	
	int fg = parse_color(fg_str);
	int bg = (bg_str != 0) ? parse_color(bg_str):(color >> 4);
	
	if (fg == -1 || bg == -1) {
		print("Unknown color. Type 'color' for available colors.\n");
		return;
	}
	
	set_color((enum color_codes)fg, (enum color_codes)bg);
	repaint_screen();
}

void reboot(void) {
	unsigned char good;
	do {good = inb(0x64);}
	while (good & 0x02);
	outb(0x64, 0xFE);
	
	while (1) {
		__asm__ volatile ("hlt");
	}
}