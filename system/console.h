#ifndef CONSOLE_H
#define CONSOLE_H

void repaint_screen(void);
void cls(void);
void putchar(char c);
void print(const char* str);
void input(char* buffer, unsigned int max_size);
void change_colors(char* args);
void reboot(void);

#endif