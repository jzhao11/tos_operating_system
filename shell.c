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

// print help info
void print_help(int window_id){
	wm_print(window_id, "Need some help?\n");
	wm_print(window_id, "Here you can find information that may be helpful.\n");
	wm_print(window_id, "help -- Print out all the supported commands.\n");
	wm_print(window_id, "cls -- Clear the screen.\n");
	wm_print(window_id, "shell -- Launch another shell.\n");
	wm_print(window_id, "pong -- Launch the PONG game.\n");
	wm_print(window_id, "echo <str> -- Echo the given string to screen.\n");
	wm_print(window_id, "ps -- Print out the process table.\n");
	wm_print(window_id, "history -- Print out all the typed commands.\n");
	wm_print(window_id, "!<number> -- Repeat command with the given index.\n");
	wm_print(window_id, "about -- Print out author's information.\n");
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
			if (index > 0) {
				commands[last][--index] = '\0';
			}
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

// repeat the command
void repeat_command(int window_id, char* cmd) {
	// calculate the index after "!"
	int index = 0;
	while (*cmd >= '0' && *cmd <= '9') {
		index = index * 10 + (*cmd - '0');
		cmd++;
	}

	// if the index is within bound, execute the command
	// if the index is out of bound, prompt an error msg
	if (index <= last) {
		wm_print(window_id, "%s\n", commands[index]);
		execute_command(window_id, commands[index]);
	} else {
		wm_print(window_id, "Invalid index!\n");
	}
}

// execute the command
void execute_command(int window_id, char* cmd) {
	if (compare_string(cmd, "help")) {
		print_help(window_id);
	} else if (compare_string(cmd, "cls")) {
		wm_clear(window_id);
	} else if (compare_string(cmd, "pong")) {
		start_pong();
	} else if (compare_string(cmd, "about")) {
		print_about(window_id);
	} else if (compare_string(cmd, "shell")) {
		start_shell();
	} else if (cmd[0] == '!') {
		repeat_command(window_id, cmd + 1);
	} else if (cmd[0] != 0) {
		wm_print(window_id, "%s Command not found!\n", cmd);
	}
}

// print author's info
void print_about(int window_id) {
	wm_print(window_id, "***************************************\n");
	wm_print(window_id, "*  Hello! This is TOS shell for fun!  *\n");
	wm_print(window_id, "*  Author:  Jianfei (Christian) Zhao  *\n");
	wm_print(window_id, "*  Contact: christianz0991@gmail.com  *\n");
	wm_print(window_id, "*  Intro:   The author is seeking an  *\n");
	wm_print(window_id, "*           summer internship.        *\n");
	wm_print(window_id, "*  Hobby:   Non-Mickey-Mouse Courses  *\n");
	wm_print(window_id, "***************************************\n");
}

// entry point of shell
void start_shell() {
	int window_id = wm_create(10, 3, 50, 17);
	last = -1;
	print_about(window_id);

	while (1) {
		char* cmd = parse_command(window_id);
		execute_command(window_id, cmd);
	}
}
