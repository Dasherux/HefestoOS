#include "../system/console.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"

static char* getargs(char* str) {
	int i = 0;
    while (str[i] != '\0' && str[i] != ' ') i++;
    if (str[i] == ' ') return &str[i + 1];

    return "";
}

static void welcome_screen(void) {
	cls();
	print(
		"==================================\n"
		"           Hefesto OS\n"
		"==================================\n"
		"      2026, Made by Dasherux\n"
		"- v0.1\n"
		"\n"
        " Kernel........................OK\n"
        " Drivers.......................OK\n"
        " C System and Shell............OK\n"
        "\n"
        "   Press any key to continue..."
	);
	while (1) {
		KeyEvent key = keyboard_getkey();
		if (key.type != KEY_NONE) {break;}
	}
}

void shell_main(void) {
	welcome_screen();
	cls();									
	
	print("2026, Dasherux\nType 'help' for available commands.\n\n");
	
	char user_input[32];
	char command[128];
	int initial_prompt = 1;
	
    while (1) {
		if (initial_prompt == 1) print("> ");
		input(user_input, sizeof(command));
		
		int i = 0;
		while (user_input[i] != '\0' && user_input[i] != ' ' && i < 32 - 1) {
			char c = user_input[i];
			if (c >= 'A' && c <= 'Z') c += 32;
			command[i] = c;
			i++;
		}
		command[i] = '\0';
		
		if (strcmp(command, "help")) {
			print(
				"Available commands:\n\n"
				"HELP.........Shows internal commands available in the Shell.\n"
				"CLS..........Cleans the shell.\n"
				"COLOR........Changes the foreground shell color.\n"
				"ECHO.........Repeats the entered message.\n"
				"REBOOT.......Restart the system.\n"
				"VER..........Displays the current system version."
				"\n\n"
			);
		} 
		else if (strcmp(command, "cls")) {cls();}
		else if (strcmp(command, "color")) {
			char* args = getargs(user_input);
			change_colors(args);
		}
		else if (strcmp(command, "echo")) {
			const char* args = getargs(user_input);
			if (strcmp(args, "@off")) initial_prompt = 0;
			else if (strcmp(args, "@on")) initial_prompt = 1;
			else {
				print(args);
				putchar('\n');
			}
		}
		else if (strcmp(command, "reboot")) {reboot();}
		else if (strcmp(command, "ver")) {print("Hefesto OS v0.1 - 2026, Made by Dasherux.\n");}
		else if (strcmp(command, "")) {}
		else {
			print("Unknown command. Type 'help' for available commands.\n");
		}
    }
}