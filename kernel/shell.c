/*************************
*   Name: Jianfei Zhao   *
*   ID: 918126149        *
*************************/

/************** header file, macros, and function declarations **************/

#include <kernel.h>

#define MAX_CMD 32 // max 32 commands
#define MAX_LEN 64 // max 63 chars in each command, '\0' reserved at the end

// calculate the length of string
int str_len(char* str);

// compare the first token of two strings
int compare_string(char* str1, char* str2);

// get substring and copy to destination
void get_substring(char* start, char* end, char* dest);  

// copy source string to destination        
void copy_string(char* src, char* dest);

// convert string to int
int str_to_int(char* str);

// print help info
void print_help(int window_id);

// print author info
void print_about(int window_id);

// print process heading
void printsh_process_heading(int window_id);

// print process details
void printsh_process_details(int window_id, PROCESS proc);

// print all processes
void printsh_all_processes(int window_id);

// print all commands
void print_all_commands(int window_id, char commands[][MAX_LEN]);

// print echo
void print_echo(int window_id, char* echo);

// parse command
char* parse_command(int window_id, int* ptr_number, char commands[][MAX_LEN]);

// execute command
void execute_command(int window_id, char* cmd, char commands[][MAX_LEN]);

// entry point of shell process
void shell_process();

// start shell
void start_shell();



/****************************** implementation ******************************/

// calculate the length of string (str)
int str_len(char* str) {
	int len = 0;
	while (*str++ != 0) {
		len++;
	}
	return len;
}

// compare the first token of two strings
// split each string by using delimiters (' ' and '\0')
int compare_string(char* str1, char* str2) {
	// trim whitespaces at the beginning
	while (*str1 == ' ') {
		str1++;
	}
	while (*str2 == ' ') {
		str2++;
	}

	// compare
	while (*str1 != 0 && *str1 != ' ' && *str2 != 0 && *str2 != ' ' && *str1 == *str2) {
		str1++;
		str2++;
	}
	return (*str1 == 0 || *str1 == ' ') && (*str2 == 0 || *str2 == ' ');
}

// get substring (from start to end)
// copy the substring to destination (dest)
void get_substring(char* start, char* end, char* dest) {
	while (*start != 0 && start != end) {
		*dest++ = *start++;
	}
	*dest = 0;
}

// copy source string (src) to destination (dest)
void copy_string(char* src, char* dest) {
	while (*src != 0) {
		*dest++ = *src++;
	}
	*dest = 0;
}

// convert string (str) into integer
int str_to_int(char* str) {
	int num = 0;
	while (*str >= '0' && *str <= '9') {
		num = num * 10 + (*str - '0');
		str++;
	}
	return num;
}

// print help info
void print_help(int window_id) {
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

// print author info
void print_about(int window_id) {
	wm_print(window_id, "***************************************\n");
	wm_print(window_id, "*  Hello! This is TOS shell for fun!  *\n");
	wm_print(window_id, "*  Author:  Jianfei (Christian) Zhao  *\n");
	wm_print(window_id, "*  Contact: christianz0991@gmail.com  *\n");
	wm_print(window_id, "*  Intro:   The author is seeking an  *\n");
	wm_print(window_id, "*           internship position.      *\n");
	wm_print(window_id, "*  Hobby:   Non-Mickey-Mouse Courses  *\n");
	wm_print(window_id, "***************************************\n");
}

// shell (sh) version of print_process_heading()
void printsh_process_heading(int window_id) {
	wm_print(window_id, "State           Active Prio Name\n");
	wm_print(window_id, "------------------------------------------------\n");
}

// shell (sh) version of print_process_details()
void printsh_process_details(int window_id, PROCESS proc) {
	static const char *state[] = {"READY          ",
		"ZOMBIE         ", "SEND_BLOCKED   ",
		"REPLY_BLOCKED  ", "RECEIVE_BLOCKED",
		"MESSAGE_BLOCKED", "INTR_BLOCKED   "
	};
	if (!proc->used) {
		wm_print(window_id, "PCB slot unused!\n");
		return;
	}
	wm_print(window_id, state[proc->state]);      // print state
	if (proc == active_proc) {
		wm_print(window_id, " *      ");          // print flag of active_proc
	} else {
		wm_print(window_id, "        ");
	}
	wm_print(window_id, "  %2d", proc->priority); // print priority
	wm_print(window_id, " %s\n", proc->name);     // print name
}

// shell (sh) version of print_all_processes()
void printsh_all_processes(int window_id) {
	int i;
	PCB* proc = pcb;
	printsh_process_heading(window_id);
	for (i = 0; i < MAX_PROCS; i++, proc++) {
		if (proc->used) {
			printsh_process_details(window_id, proc);
		}
	}
}

// print all commands
void print_all_commands(int window_id, char commands[][MAX_LEN]) {
	wm_print(window_id, "Index Command\n");
	wm_print(window_id, "--------------------------------------\n");

	for (int i = 0; i < MAX_CMD; ++i) {
		if (commands[i][0] == 0) {
			break;
		}
		wm_print(window_id, "%2d    %s\n", i, commands[i]);
	}
}

// print echo
// ignore redundant whitespaces in between
void print_echo(int window_id, char* str) {
	char* echo = str;
	if (*echo++ == ' ') {
		while (*echo != 0 && *echo != ';') {
			if (*echo == ' ' && *(echo - 1) == ' ') {
				echo++;
			} else {
				wm_print(window_id, "%c", *echo++);
			}
		}
	}
	wm_print(window_id, "\n");
}

// parse input as command
// update the total number (*ptr_number) of commands
char* parse_command(int window_id, int* ptr_number, char commands[][MAX_LEN]) {
	char ch;
	char* cmd = (char*) malloc(sizeof(char) * MAX_LEN);
	char* tmp = cmd;
	while ((ch = keyb_get_keystroke(window_id, TRUE)) != 0x0D) {
		if ((ch == ' ' && tmp == cmd) || (tmp - cmd >= MAX_LEN - 1)) {
			wm_print(window_id, "%c", ch);
			continue;
		} else if (ch == 0x08) {
			if (tmp > cmd) {
				*(--tmp) = 0;
			}
			wm_print(window_id, "\b");
		} else {
			wm_print(window_id, "%c", ch);
			*tmp++ = ch;
		}
	}
	wm_print(window_id, "\n");
	*tmp = 0;

	//wm_print(window_id, "command: %s,", cmd);         // uncomment to verify command
	//wm_print(window_id, "number: %d\n", *ptr_number); // uncomment to verify number

	if (tmp == cmd) {
		free(cmd);
		return "";
	} else if (*cmd == '!') {
		// calculate the index after "!"
		// if the index is out of bound, prompt an error msg
		// if the index is within bound, save it in history
		int index = str_to_int(cmd + 1);
		if (index >= *ptr_number) {
			wm_print(window_id, "Invalid index! Index of last command: %d\n", *ptr_number - 1);
			free(cmd);
			return "";
		} else {
			// consider corner cases (possibly remaining part after !<number>)
			char* ptr1 = cmd + 1;
			while (*ptr1 >= '0' && *ptr1 <= '9') {
				ptr1++;
			}
			int remaining_len = str_len(ptr1);
			int command_len = str_len(commands[index % MAX_CMD]);

			// replace !<number> with the retrieved command
			// if (command + remaining part) is out of bound, ignore it
			// if (command + remaining part) is within bound, save it in history
			if (command_len + remaining_len > MAX_LEN - 1) {
				free(cmd);
				return "";
			} else {
				char* buffer = (char*) malloc(sizeof(char) * MAX_LEN);
				char* ptr2 = buffer;
				copy_string(commands[index % MAX_CMD], buffer);
				while (*ptr2 != 0) {
					ptr2++;
				}
				copy_string(ptr1, ptr2);
				wm_print(window_id, "%s\n", buffer);
				copy_string(buffer, commands[*ptr_number % MAX_CMD]);
				free(buffer);
				free(cmd);
				return commands[(*ptr_number)++ % MAX_CMD];
			}
		}
	} else {
		copy_string(cmd, commands[*ptr_number % MAX_CMD]);
		free(cmd);
		return commands[(*ptr_number)++ % MAX_CMD];
	}
}

// execute command (cmd)
void execute_command(int window_id, char* cmd, char commands[][MAX_LEN]) {
	char* token = (char*) malloc(sizeof(char) * MAX_LEN);
	char* tmp = cmd;

	// split command into tokens by using delimiters (';' and '\0')
	while (1) {
		// ignore the white spaces at the beginning of each command
		while (*cmd == ' ') {
			tmp = ++cmd;
		}

		if (*tmp == ';' || *tmp == 0) {
			// get substring of command as token
			get_substring(cmd, tmp, token);

			// execute the token
			if (compare_string(token, "help")) {
				print_help(window_id);
			} else if (compare_string(token, "ps")) {
				printsh_all_processes(window_id);
			} else if (compare_string(token, "history")) {
				print_all_commands(window_id, commands);
			} else if (compare_string(token, "about")) {
				print_about(window_id);
			} else if (compare_string(token, "cls")) {
				wm_clear(window_id);
			} else if (compare_string(token, "echo")) {
				print_echo(window_id, cmd + 4);
			} else if (compare_string(token, "pong")) {
				start_pong();
			} else if (compare_string(token, "shell")) {
				start_shell();
			} else if (compare_string(token, "train")) {
				init_train();
			} else if (token[0] != 0) {
				wm_print(window_id, "Command not found!\n");
			}

			// if not the end, update the start of cmd
			// if the end, terminate the loop
			if (*tmp == 0) {
				break;
			} else {
				cmd = tmp + 1;
			}
		}
		tmp++;
	}

	free(token);
}

// shell process
void shell_process() {
	char commands[MAX_CMD][MAX_LEN]; // rolling array to save commands
	int number = 0;                  // total number of saved commands
	int window_id = wm_create(10, 2, 50, 20);
	print_about(window_id);

	while (1) {
		char* cmd = parse_command(window_id, &number, commands);
		execute_command(window_id, cmd, commands);
	}
}

// entry point of shell
void start_shell() {
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
