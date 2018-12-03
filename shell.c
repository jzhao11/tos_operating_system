#include <kernel.h>

#define MAX_CMD 32 // max 32 commands
#define MAX_LEN 32 // max 32 characters for each command

char commands[MAX_CMD][MAX_LEN]; // rolling array to store commands
int last;                        // index of the last command

// compare two strings
int compare_string(char* str1, char* str2) {
	while (*str1 != 0 && *str2 != 0 && *str1 == *str2) {
		str1++;
		str2++;
	}
	return *str1 == 0 && *str2 == 0;
}

void print_help(int window_id){
	wm_print(window_id, "Welcome to TOS Shell!\n");
	wm_print(window_id, "Need help?\n");
	wm_print(window_id, "Below you could find some information that might be helpful.\n");
	wm_print(window_id, "help -- Print out the supported commands in this shell.\n");
	wm_print(window_id, "cls -- Clear the screen.\n");
	wm_print(window_id, "shell -- Launch another shell.\n");
	wm_print(window_id, "pong -- Launch the PONG game.\n");
	wm_print(window_id, "echo <str> -- Echo the string <str> to the screen\n");
	wm_print(window_id, "ps -- Print out the process table.\n");
	wm_print(window_id, "history -- Print out all the commands that have been typed.\n");
	wm_print(window_id, "!<number> -- Repeat the command with the given number.\n");
	wm_print(window_id, "about -- Print out the information about author.\n");
}

char* parse_command(int window_id) {
	last = (last + 1) % MAX_CMD;
	int index = 0;
	char ch;
	while ((ch = keyb_get_keystroke(window_id, TRUE)) != 0x0D) {
		if (ch == ' ' || index >= MAX_LEN) {
			wm_print(window_id, "%c", ch);
			continue;
		} else if (ch == 0x08) {
			commands[last][--index] = '\0';
			wm_print(window_id, "\b");
		} else {
			wm_print(window_id, "%c", ch);
			commands[last][index++] = ch;
		}
	}
	wm_print(window_id, "\n");

	if (index == 0) {
		last = (last - 1) % MAX_CMD;
		return "";
	} else {
		return commands[last];
	}
}

void execute_command(int window_id, char* cmd) {
	//wm_print(window_id, "get command: %s ", cmd);
	//wm_print(window_id, "last: %d ", last);
	if (compare_string(cmd, "pong")) {
		start_pong();
	} else if (compare_string(cmd, "cls")) {
		wm_clear(window_id);
	}
}

// entry point of shell
void start_shell() {
	int window_id = wm_create(10, 3, 50, 17);
	last = -1;
	wm_print(window_id, "Hello! This is TOS Shell!\n");
	
	while (1) {
		char* cmd = parse_command(window_id);
		execute_command(window_id, cmd);
	}
}
